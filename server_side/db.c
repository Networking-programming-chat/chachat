//
//  db.c
//  Chachat
//

#include "db.h"

#include <stdio.h>
#include <stdlib.h> // exit()
#include <string.h>
#include <inttypes.h>

typedef void (*db_user_callback)(cc_user*);
typedef void (*db_channel_callback)(cc_channel*);

// This module takes care of the db handle

static sqlite3 *db_handle;

static unsigned long next_user_id;
static unsigned long next_channel_id;
static unsigned long next_join_id;

static int server_id;

void free_cc_user(cc_user *user)
{
    if (user == NULL) {
        return;
    }
    
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
    if (channel == NULL) {
        return;
    }

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
    if (user == NULL) {
        printf("No user data to print\n");
        return;
    }
    
    printf("User data:\nid: %d\nnick: %s\nserverid: %d\n", user->user_id, user->nick, user->server_id);
}

void print_user_list(cc_user* user)
{
    if (user == NULL) {
        printf("No user data to print\n");
        return;
    }
    
    printf("User data:\nid: %d\nnick: %s\nserverid: %d\n", user->user_id, user->nick, user->server_id);
    
    if (user->next_user != NULL) {
        print_user_list(user->next_user);
    }
}

void print_channel(cc_channel *channel)
{
    if (channel == NULL) {
        printf("No channel data to print\n");
        return;
    }
    
    printf("Channel data:\nid: %d\nname: %s\ntopic: %s\n", channel->channel_id, channel->name, channel->topic);
}

void print_channel_list(cc_channel *channel)
{
    if (channel == NULL) {
        printf("No channel data to print\n");
        return;
    }
    
    printf("Channel data:\nid: %d\nname: %s\ntopic: %s\n", channel->channel_id, channel->name, channel->topic);

    if (channel->next_channel != NULL) {
        print_channel_list(channel->next_channel);
    }
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

static int get_max_id(void *data, int argc, char **argv, char **azColName)
{
    uint64_t type;
    int i;
    
    type = (uint64_t)data;
    
    
    // Iterate the arguments to show the query results
    for(i = 0; i < argc; ++i){
        if (strncmp(azColName[i], "MAX", 3) == 0 && argv[i]) {
            if (type == 0) {
                next_user_id = atoi(argv[i]) + 1;
            } else if (type == 1) {
                next_channel_id = atoi(argv[i]) + 1;
            } else if (type == 2) {
                next_join_id = atoi(argv[i]) + 1;
            }
        }
    }
    
    return 0;

}

void init_db2(sqlite3 *db)
{
    int status, flags;
    char *errmsg, querymsg[1024];
    
    // Open / create database
    flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI;
    status = sqlite3_open_v2("file:chachat.db", &db, flags, NULL);
    checksql(status, db, "sqlite3_open_v2 error");
    
    db_handle = db;

    // Create tables if not present
    sprintf(querymsg, "CREATE TABLE IF NOT EXISTS users (id INTEGER NOT NULL PRIMARY KEY, nick TEXT UNIQUE, serverid INTEGER)");
    status = sqlite3_exec(db, querymsg, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    sprintf(querymsg, "CREATE TABLE IF NOT EXISTS servers (id INTEGER NOT NULL PRIMARY KEY, ip TEXT)");
    status = sqlite3_exec(db, querymsg, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    sprintf(querymsg, "CREATE TABLE IF NOT EXISTS channels (id INTEGER NOT NULL PRIMARY KEY, name TEXT UNIQUE, topic TEXT)");
    status = sqlite3_exec(db, querymsg, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    sprintf(querymsg, "CREATE TABLE IF NOT EXISTS joined (joinid INTEGER NOT NULL UNIQUE, userid INTEGER NOT NULL, channelid INTEGER NOT NULL, PRIMARY KEY(userid, channelid))");
    status = sqlite3_exec(db, querymsg, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    // SELECT name FROM sqlite_master WHERE type='table' AND name='table_name';
    
    next_channel_id = 0;
    next_user_id = 0;
    
    server_id = 0;
    
    // Get possible channel and user ids
    sprintf(querymsg, "SELECT MAX(id) FROM users");
    status = sqlite3_exec(db, querymsg, get_max_id, (void*)0, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    sprintf(querymsg, "SELECT MAX(id) FROM channels");
    status = sqlite3_exec(db, querymsg, get_max_id, (void*)1, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    sprintf(querymsg, "SELECT MAX(joinid) FROM joined");
    status = sqlite3_exec(db, querymsg, get_max_id, (void*)2, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
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

void clear_db()
{
    int status;
    char *errmsg, querymsg[1024];

    // Clear all tables
    sprintf(querymsg, "DELETE FROM users");
    status = sqlite3_exec(db_handle, querymsg, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    sprintf(querymsg, "DELETE FROM servers");
    status = sqlite3_exec(db_handle, querymsg, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    sprintf(querymsg, "DELETE FROM channels");
    status = sqlite3_exec(db_handle, querymsg, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    sprintf(querymsg, "DELETE FROM joined");
    status = sqlite3_exec(db_handle, querymsg, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    next_channel_id = 0;
    next_user_id = 0;
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
    user->next_user = NULL;
    *user_ptr = user;
    
    // Iterate the arguments to show the query results
    for(i = 0; i < argc; ++i){
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        
        if (strcmp(azColName[i], "id") == 0 && argv[i]) {
            user->user_id = atoi(argv[i]);
        }
        else if (strcmp(azColName[i], "nick") == 0 && argv[i]) {
            size_t len = strlen(argv[i]) + 1;
            user->nick_len = len;
            user->nick = malloc(sizeof(char) * len);
            strncpy(user->nick, argv[i], len);
            user->nick[len-1] = '\0';
        }
        else if (strcmp(azColName[i], "serverid") == 0 && argv[i]) {
            user->server_id = atoi(argv[i]);
        }
    }
    
    return 0;
}

cc_user * get_user_by_nick(const char *nick)
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

cc_user * get_user_by_id(int user_id)
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
    channel->next_channel = NULL;
    *channel_ptr = channel;
    
    // Iterate the arguments to show the query results
    for(i = 0; i < argc; ++i){
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        
        if (strcmp(azColName[i], "id") == 0 && argv[i]) {
            channel->channel_id = atoi(argv[i]);
        }
        else if (strcmp(azColName[i], "name") == 0 && argv[i]) {
            size_t len = strlen(argv[i]) + 1;
            channel->name_len = len;
            channel->name = malloc(sizeof(char) * len);
            strncpy(channel->name, argv[i], len);
            channel->name[len-1] = '\0';
        }
        else if (strcmp(azColName[i], "topic") == 0 && argv[i]) {
            size_t len = strlen(argv[i]) + 1;
            channel->topic_len = len;
            channel->topic = malloc(sizeof(char) * len);
            strncpy(channel->topic, argv[i], len);
            channel->topic[len-1] = '\0';
        }
    }
    
    return 0;
}


cc_channel * get_channel_by_name(const char *name)
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

cc_channel * get_channel_by_id(int channel_id)
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

static int multi_user_callback(void *data, int argc, char **argv, char **azColName){
    int i;
    cc_user **user_ptr;
    cc_user *user;
    
    user_ptr = (cc_user **)data;
    
    user = malloc(sizeof(cc_user));
    user->next_user = NULL;
    
    // Set first user
    if (user_ptr[0] == NULL) {
        user_ptr[0] = user;
    }
    
    // Update linked list
    if (user_ptr[1] != NULL) {
        (user_ptr[1])->next_user = user;
    }
    user_ptr[1] = user;
    
    // Iterate the arguments to show the query results
    for(i = 0; i < argc; ++i){
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        
        if ((strcmp(azColName[i], "ju.id") == 0 || strcmp(azColName[i], "id") == 0) && argv[i]) {
            user->user_id = atoi(argv[i]);
        }
        else if ((strcmp(azColName[i], "ju.nick") == 0 || strcmp(azColName[i], "nick") == 0) && argv[i]) {
            size_t len = strlen(argv[i]) + 1;
            user->nick_len = len;
            user->nick = malloc(sizeof(char) * len);
            strncpy(user->nick, argv[i], len);
            user->nick[len-1] = '\0';
        }
        else if ((strcmp(azColName[i], "ju.serverid") == 0 || strcmp(azColName[i], "serverid") == 0) && argv[i]) {
            user->server_id = atoi(argv[i]);
        }
    }
    
    return 0;
}

cc_user * get_users_by_channel_id(int channel_id)
{
    int status;
    char *errmsg;
    char query[256];
    cc_user **user_ptr;
    
    user_ptr = malloc(sizeof(cc_user*) * 2);
    user_ptr[0] = NULL;
    user_ptr[1] = NULL;
    
    // Find the user by id
    sprintf(query, "SELECT ju.id, ju.nick, ju.serverid FROM (users INNER JOIN joined ON users.id = joined.userid) AS ju INNER JOIN channels ON ju.channelid = channels.id WHERE channels.id = %d", channel_id);
    status = sqlite3_exec(db_handle, query, multi_user_callback, (void*)user_ptr, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    // TODO: Cannot handle db threading
    cc_user *user = *user_ptr;
    free(user_ptr);
    
    return user;
}

cc_user * get_users_by_channel_name(const char *channel_name)
{
    int status;
    char *errmsg;
    char query[256];
    cc_user **user_ptr;
    
    user_ptr = malloc(sizeof(cc_user*) * 2);
    user_ptr[0] = NULL;
    user_ptr[1] = NULL;
    
    // Find the user by id
    sprintf(query, "SELECT ju.id, ju.nick, ju.serverid FROM (users INNER JOIN joined ON users.id = joined.userid) AS ju INNER JOIN channels ON ju.channelid = channels.id WHERE channels.name LIKE '%s'", channel_name);
    status = sqlite3_exec(db_handle, query, multi_user_callback, (void*)user_ptr, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    // TODO: Cannot handle db threading
    cc_user *user = *user_ptr;
    free(user_ptr);
    
    return user;
}

cc_user * get_all_users()
{
    int status;
    char *errmsg;
    char query[256];
    cc_user **user_ptr;
    
    user_ptr = malloc(sizeof(cc_user*) * 2);
    user_ptr[0] = NULL;
    user_ptr[1] = NULL;
    
    // Find the user by id
    sprintf(query, "SELECT * FROM users");
    status = sqlite3_exec(db_handle, query, multi_user_callback, (void*)user_ptr, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    // TODO: Cannot handle db threading
    cc_user *user = *user_ptr;
    free(user_ptr);
    
    return user;
}

static int multi_channel_callback(void *data, int argc, char **argv, char **azColName){
    int i;
    cc_channel **channel_ptr;
    cc_channel *channel;
    
    channel_ptr = (cc_channel **)data;
    
    channel = malloc(sizeof(cc_channel));
    channel->next_channel = NULL;
    
    // Set first user
    if (channel_ptr[0] == NULL) {
        channel_ptr[0] = channel;
    }
    
    // Update linked list
    if (channel_ptr[1] != NULL) {
        (channel_ptr[1])->next_channel = channel;
    }
    channel_ptr[1] = channel;

    // Iterate the arguments to show the query results
    for(i = 0; i < argc; ++i){
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        
        if ((strcmp(azColName[i], "channels.id") == 0 || strcmp(azColName[i], "id") == 0) && argv[i]) {
            channel->channel_id = atoi(argv[i]);
        }
        else if ((strcmp(azColName[i], "channels.name") == 0 || strcmp(azColName[i], "name") == 0) && argv[i]) {
            size_t len = strlen(argv[i]) + 1;
            channel->name_len = len;
            channel->name = malloc(sizeof(char) * len);
            strncpy(channel->name, argv[i], len);
            channel->name[len-1] = '\0';
        }
        else if ((strcmp(azColName[i], "channels.topic") == 0 || strcmp(azColName[i], "topic") == 0) && argv[i]) {
            size_t len = strlen(argv[i]) + 1;
            channel->topic_len = len;
            channel->topic = malloc(sizeof(char) * len);
            strncpy(channel->topic, argv[i], len);
            channel->topic[len-1] = '\0';
        }
    }
    
    return 0;
}


cc_channel * get_channels_of_user_id(int user_id)
{
    int status;
    char *errmsg;
    char query[256];
    cc_channel **channel_ptr;
    
    channel_ptr = malloc(sizeof(cc_channel*) * 2);
    channel_ptr[0] = NULL;
    channel_ptr[1] = NULL;
    
    // Find the channel by id
    sprintf(query, "SELECT channels.id, channels.name, channels.topic FROM (users INNER JOIN joined ON users.id = joined.userid) AS ju INNER JOIN channels ON ju.channelid = channels.id WHERE ju.id = %d;", user_id);
    status = sqlite3_exec(db_handle, query, multi_channel_callback, (void*)channel_ptr, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    cc_channel *channel = *channel_ptr;
    free(channel_ptr);
    
    return channel;
}

cc_channel * get_channels_of_user_nick(const char *user_nick)
{
    int status;
    char *errmsg;
    char query[256];
    cc_channel **channel_ptr;
    
    channel_ptr = malloc(sizeof(cc_channel*) * 2);
    channel_ptr[0] = NULL;
    channel_ptr[1] = NULL;
    
    // Find the channel by id
    sprintf(query, "SELECT channels.id, channels.name, channels.topic FROM (users INNER JOIN joined ON users.id = joined.userid) AS ju INNER JOIN channels ON ju.channelid = channels.id WHERE ju.nick LIKE '%s';", user_nick);
    status = sqlite3_exec(db_handle, query, multi_channel_callback, (void*)channel_ptr, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    cc_channel *channel = *channel_ptr;
    free(channel_ptr);
    
    return channel;
}

cc_channel * get_all_channels()
{
    int status;
    char *errmsg;
    char query[256];
    cc_channel **channel_ptr;
    
    channel_ptr = malloc(sizeof(cc_channel*) * 2);
    channel_ptr[0] = NULL;
    channel_ptr[1] = NULL;
    
    // Find the channel by id
    sprintf(query, "SELECT * FROM channels AS channels");
    status = sqlite3_exec(db_handle, query, multi_channel_callback, (void*)channel_ptr, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    cc_channel *channel = *channel_ptr;
    free(channel_ptr);
    
    return channel;
}

int add_user(const char *user_nick)
{
    int status;
    unsigned long id;
    char *errmsg;
    char query[256];
    
    id = next_user_id++;

    // Add user nick
    sprintf(query, "INSERT INTO users VALUES(%lu, '%s', %d)", id, user_nick, server_id);
    status = sqlite3_exec(db_handle, query, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");

    return status;
}

cc_user * add_user_server(const char *user_nick, int server)
{
    int status;
    unsigned long id;
    char *errmsg;
    size_t len;
    char query[256];
    cc_user *user_data;
    
    id = next_user_id++;
    
    // Add user nick
    sprintf(query, "INSERT INTO users VALUES(%lu, '%s', %d)", id, user_nick, server);
    status = sqlite3_exec(db_handle, query, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    // Create the struct for return
    if (status != SQLITE_OK) {
        return NULL;
    }
    
    // Setup new user
    user_data = malloc(sizeof(cc_user));
    user_data->next_user = NULL;
    
    // Set nick
    len = strlen(user_nick) + 1;
    user_data->nick_len = len;
    user_data->nick = malloc(sizeof(char) * len);
    strncpy(user_data->nick, user_nick, len);
    user_data->nick[len-1] = '\0';
    
    // Set server
    user_data->server_id = server;
    
    // Set id
    user_data->user_id = (int)id;

    return user_data;
}

int add_channel(const char *name)
{
    int status;
    char *errmsg;
    char query[256];
    
    // Add channel name
    sprintf(query, "INSERT INTO channels VALUES(%lu, '%s', '')", next_channel_id++, name);
    status = sqlite3_exec(db_handle, query, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    return status;
}

int join_channel(const char *user_nick, const char *channel_name)
{
    int status;
    char *errmsg;
    char query[512];
    
    // Add user and channel to joined
    sprintf(query, "INSERT INTO joined (joinid, userid, channelid) SELECT %lu, users.id, channels.id FROM users LEFT OUTER JOIN channels ON 1 = 1 WHERE users.nick LIKE '%s' AND channels.name LIKE '%s'", next_join_id++, user_nick, channel_name);
    status = sqlite3_exec(db_handle, query, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    return status;

}

int part_channel(const char *user_nick, const char *channel_name)
{
    int status;
    char *errmsg;
    char query[512];
    
    // Remove line from joined
    sprintf(query, "DELETE FROM joined WHERE joinid IN (SELECT joinid FROM (joined INNER JOIN users ON users.id = joined.userid) AS ju INNER JOIN channels ON ju.channelid = channels.id WHERE ju.nick LIKE '%s' AND channels.name LIKE '%s')", user_nick, channel_name);
    status = sqlite3_exec(db_handle, query, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    return status;
}

int remove_user(const char *nick)
{
    int status;
    char *errmsg;
    char query[512];
    
    // Remove all lines for the user from joined and the line from users
    sprintf(query, "DELETE FROM joined WHERE joinid IN (SELECT joinid FROM (joined INNER JOIN users ON users.id = joined.userid) AS ju WHERE ju.nick LIKE '%s'); DELETE FROM users WHERE nick LIKE '%s'", nick, nick);
    status = sqlite3_exec(db_handle, query, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    return status;
}

int remove_channel(const char *channel_name)
{
    int status;
    char *errmsg;
    char query[512];
    
    // Remove all lines for the channel from joined and the line from channels
    sprintf(query, "DELETE FROM joined WHERE joinid IN (SELECT joinid FROM joined AS INNER JOIN channels ON joined.channelid = channels.id WHERE channels.name LIKE '%s'); DELETE FROM channels WHERE name LIKE '%s'", channel_name, channel_name);
    status = sqlite3_exec(db_handle, query, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    return status;
}

int set_channel_topic(const char *channel_name, const char *topic)
{
    int status;
    char *errmsg;
    char query[512];
    
    // Change topic
    sprintf(query, "UPDATE channels SET topic='%s' WHERE name LIKE '%s'", topic, channel_name);
    status = sqlite3_exec(db_handle, query, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    return status;
}

int change_nick(const char *current_nick, const char *new_nick)
{
    int status;
    char *errmsg;
    char query[512];
    
    // Change nick
    sprintf(query, "UPDATE users SET nick='%s' WHERE nick LIKE '%s'", new_nick, current_nick);
    status = sqlite3_exec(db_handle, query, NULL, NULL, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
    
    return status;
}
