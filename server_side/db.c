//
//  db.c
//  Chachat
//

#include "db.h"

#include <stdio.h>
#include <stdlib.h> // exit()

typedef void (*db_nick_callback)(cc_user*);

// This module takes care of the db handle

static sqlite3 *db_handle;



void free_cc_user(cc_user *user)
{
    free(user->nick);
    free(user);
}

void free_cc_channel(cc_channel *channel)
{
    free(channel->name);
    free(channel->topic);
    free(channel);
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
static int nick_callback(void *data, int argc, char **argv, char **azColName){
    int i;
    db_nick_callback callback;
    
    callback = (db_nick_callback)data;
    
    // Iterate the arguments to show the query results
    for(i = 0; i < argc; ++i){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    return 0;
}

void get_user_nick(const char *nick, db_nick_callback callback)
{
    int status;
    char *errmsg;
    char query[128];
    
    // Find the user by nick
    sprintf(query, "SELECT * FROM users WHERE nick LIKE '%s'", nick);
    status = sqlite3_exec(db_handle, query, nick_callback, (void*)callback, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
}

void get_user_id(int user_id, db_nick_callback callback)
{
    int status;
    char *errmsg;
    char query[128];
    
    // Find the user by id
    sprintf(query, "SELECT * FROM users WHERE id = %d", user_id);
    status = sqlite3_exec(db_handle, query, nick_callback, (void*)callback, &errmsg);
    printsql(status, errmsg, "sqlite3_exec fail");
}




