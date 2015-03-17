
#ifndef __Chachat__db__
#define __Chachat__db__

#include <stdlib.h>
#include "sqlite3.h"

// User and channel structs

typedef struct {
    int user_id;
    int server_id;
    size_t nick_len;
    char *nick;
} cc_user ;

typedef struct {
    int channel_id;
    size_t name_len;
    size_t topic_len;
    char *name;
    char *topic;
} cc_channel ;

void free_cc_user(cc_user *user);
void free_cc_channel(cc_channel *channel);

// Test functions for printing data
void print_user(cc_user* user);
void print_channel(cc_channel *channel);


void init_db();                 // Will initialize the db (create if not present and load)
void init_db2(sqlite3 *db);     // Will initialize the db (create if not present and load)

void close_db();                // Will close the db in use

// Search functions
void get_user_nick(const char *, void(*)(cc_user *));
void get_user_id(int, void(*)(cc_user *));

void get_channel_name(const char *, void(*)(cc_channel *));
void get_channel_id(int, void(*)(cc_channel *));

// TODO: what functions do we need
/*
 Examples:
 
 struct user get_user(const char *id);
 struct channel get_channel(const char *id);
 
 int add_user(struct user u);
 int add_channel(struct channel c);
 
 int remove_user(const char *id);
 int remove_channel(const char *id);
 
 void list_users(const char *channel_id, function callback);
 etc...
 
 */

#endif /* defined(__Chachat__db__) */
