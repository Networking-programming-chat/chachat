//
//  db.c
//  Chachat
//

#include "db.h"

#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h>

typedef void (*db_user_callback)(cc_user*);
typedef void (*db_channel_callback)(cc_channel*);

// This module takes care of the db handle

static sqlite3 *db_handle;

// TODO: think about threading
static int query_answered;


void free_cc_user(cc_user *user)
{
    // Clear possible linked list
    if (user->next_user != NULL) {
        cc_user *next = user->next_user;
        user->next_user = NULL;
        free_cc_user(next);
    }

    free(user->nick);
    free(user);
}

void free_cc_channel(cc_channel *channel)
{
    // Clear possible linked list
    if (channel->next_channel != NULL) {
        cc_channel *next = channel->next_channel;
        channel->next_channel = NULL;
        free_cc_channel(next);
    }

    free(channel->name);
    free(channel->topic);
    free(channel);
}

void print_user(cc_user* user)
{
    printf("User data:\nid: %d\nnick: %s\nserverid: %d\n", user->user_id, user->nick, user->server_id);
}

void print_channel(cc_channel *channel)
{
    printf("Channel data:\nid: %d\nname: %s\ntopic: %s\n", channel->channel_id, channel->name, channel->topic);
}

// Function to test a condition
static void checksql (int test, sqlite3 * db, const char * message, ...)
{
    if (test != SQLITE_OK) {
        va_list args;
        
        // Iterate the arguments
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        
        // Print error message to stderr
        fprintf (stderr, ": %s\n", sqlite3_errmsg(db));
        
        // Failure
        // TODO: do not crash here?
        exit (1);
    }
}

// Function to test a condition
static int printsql (int test, char *errmsg, const char * message, ...)
{
    if (test != SQLITE_OK) {
        va_list args;
        
        // Iterate the arguments
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        
        // Print error message to stderr
        fprintf (stderr, ": %s\n", errmsg);
    }
    return test;
}

void init_db2(sqlite3 *db)
{
    int status, flags;
    char *errmsg, querymsg[1024];
    
    // Open / create database
    flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI;
    status = sqlite3_open_v2("file:test.db", &db, flags, NULL);
    checksql(status, db, "sqlite3_open_v2 error");
    
    db_handle = db;

    // Create tables if not present
    sprintf(querymsg, "CREATE TABLE IF NOT EXISTS users (id INTEGER NOT NULL PRIMARY KEY, nick TEXT, serverid INTEGER)");
    status = sqlite3_exec(db, querymsg, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    sprintf(querymsg, "CREATE TABLE IF NOT EXISTS servers (id INTEGER NOT NULL PRIMARY KEY, ip TEXT)");
    status = sqlite3_exec(db, querymsg, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    sprintf(querymsg, "CREATE TABLE IF NOT EXISTS channels (id INTEGER NOT NULL PRIMARY KEY, name TEXT, topic TEXT)");
    status = sqlite3_exec(db, querymsg, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    sprintf(querymsg, "CREATE TABLE IF NOT EXISTS joined (userid INTEGER NOT NULL, channelid INTEGER NOT NULL, PRIMARY KEY (userid,channelid))");
    status = sqlite3_exec(db, querymsg, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    // SELECT name FROM sqlite_master WHERE type='table' AND name='table_name';
}

void init_db()
{
    init_db2(db_handle);
}

void close_db()
{
    int status;
    
    // Close the database
    status = sqlite3_close_v2(db_handle);
    checksql(status, db_handle, "sqlite3_close_v2 error");

}

/*
 *  Search functions
 */

// Callback function for reading data
static int user_callback(void *data, int argc, char **argv, char **azColName){
    int i;
    cc_user **user_ptr;
    cc_user *user;
    
    user_ptr = (cc_user **)data;
    
    user = malloc(sizeof(cc_user));
    *user_ptr = user;
    
    // Iterate the arguments to show the query results
    for(i = 0; i < argc; ++i){
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        
        if (strcmp(azColName[i], "id") == 0 && argv[i]) {
            user->user_id = atoi(argv[i]);
        }
        else if (strcmp(azColName[i], "nick") == 0 && argv[i]) {
            size_t len = strlen(argv[i]);
            user->nick_len = len;
            user->nick = malloc(sizeof(char) * len);
            strncpy(user->nick, argv[i], len);
        }
        else if (strcmp(azColName[i], "serverid") == 0 && argv[i]) {
            user->server_id = atoi(argv[i]);
        }
    }
    
    printf("\n");
    return 0;
}

cc_user * get_user_nick(const char *nick)
{
    int status;
    char *errmsg;
    char query[128];
    cc_user **user_ptr;
    
    user_ptr = malloc(sizeof(cc_user*));
    *user_ptr = NULL;
    
    // Find the user by nick
    sprintf(query, "SELECT * FROM users WHERE nick LIKE '%s'", nick);
    status = sqlite3_exec(db_handle, query, user_callback, (void*)user_ptr, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");

    // TODO: Cannot handle db threading
    cc_user *user = *user_ptr;
    free(user_ptr);
    
    return user;
}

cc_user * get_user_id(int user_id)
{
    int status;
    char *errmsg;
    char query[128];
    cc_user **user_ptr;
    
    user_ptr = malloc(sizeof(cc_user*));
    *user_ptr = NULL;
    
    // Find the user by id
    sprintf(query, "SELECT * FROM users WHERE id = %d", user_id);
    status = sqlite3_exec(db_handle, query, user_callback, (void*)user_ptr, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    // TODO: Cannot handle db threading
    cc_user *user = *user_ptr;
    free(user_ptr);
    
    return user;
}

static int channel_callback(void *data, int argc, char **argv, char **azColName){
    int i;
    cc_channel **channel_ptr;
    cc_channel *channel;
    
    channel_ptr = (cc_channel **)data;
    
    channel = malloc(sizeof(cc_channel));
    *channel_ptr = channel;
    
    // Iterate the arguments to show the query results
    for(i = 0; i < argc; ++i){
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        
        if (strcmp(azColName[i], "id") == 0 && argv[i]) {
            channel->channel_id = atoi(argv[i]);
        }
        else if (strcmp(azColName[i], "name") == 0 && argv[i]) {
            size_t len = strlen(argv[i]);
            channel->name_len = len;
            channel->name = malloc(sizeof(char) * len);
            strncpy(channel->name, argv[i], len);
        }
        else if (strcmp(azColName[i], "topic") == 0 && argv[i]) {
            size_t len = strlen(argv[i]);
            channel->topic_len = len;
            channel->topic = malloc(sizeof(char) * len);
            strncpy(channel->topic, argv[i], len);
        }
    }
    
    printf("\n");
    return 0;
}


cc_channel * get_channel_name(const char *name)
{
    int status;
    char *errmsg;
    char query[128];
    cc_channel **channel_ptr;
    
    channel_ptr = malloc(sizeof(cc_channel*));
    *channel_ptr = NULL;
    
    // Find the channel by name
    sprintf(query, "SELECT * FROM channels WHERE name LIKE '%s'", name);
    status = sqlite3_exec(db_handle, query, channel_callback, (void*)channel_ptr, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    cc_channel *channel = *channel_ptr;
    free(channel_ptr);
    
    return channel;
}

cc_channel * get_channel_id(int channel_id)
{
    int status;
    char *errmsg;
    char query[128];
    cc_channel **channel_ptr;
    
    channel_ptr = malloc(sizeof(cc_channel*));
    *channel_ptr = NULL;
    
    // Find the channel by id
    sprintf(query, "SELECT * FROM channels WHERE id = %d", channel_id);
    status = sqlite3_exec(db_handle, query, channel_callback, (void*)channel_ptr, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    cc_channel *channel = *channel_ptr;
    free(channel_ptr);
    
    return channel;
}



