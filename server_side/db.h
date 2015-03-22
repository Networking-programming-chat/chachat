
#ifndef __Chachat__db__
#define __Chachat__db__

#include <stdlib.h>

#include "sqlite3.h"

// User and channel structs

typedef struct cc_user_struct {
    int user_id;
    int server_id;
    size_t nick_len;
    char *nick;
    
    // For linked lists
    struct cc_user_struct *next_user;
} cc_user ;

typedef struct cc_channel_struct {
    int channel_id;
    size_t name_len;
    size_t topic_len;
    char *name;
    char *topic;
    
    struct cc_channel_struct *next_channel;
} cc_channel ;

void free_cc_user(cc_user *user);
void free_cc_channel(cc_channel *channel);

// Test functions for printing data
void print_user(cc_user* user);
void print_user_list(cc_user* user);
void print_channel(cc_channel *channel);
void print_channel_list(cc_channel *channel);

void init_db();                 // Will initialize the db (create if not present and load)
void init_db2(sqlite3 *db);     // Will initialize the db (create if not present and load)

void clear_db();

void close_db();                // Will close the db in use

// Search functions
cc_user * get_user_by_nick(const char *);
cc_user * get_user_by_id(int);

cc_channel * get_channel_by_name(const char *);
cc_channel * get_channel_by_id(int);

cc_user * get_users_by_channel_id(int);
cc_user * get_users_by_channel_name(const char *channel_name);

cc_channel * get_channels_of_user_id(int);
cc_channel * get_channels_of_user_nick(const char *);

cc_user * get_all_users();
cc_channel * get_all_channels();

// Insert functions
int add_user(const char *user_nick);
int add_channel(const char *channel_name);
int join_channel(const char *user_nick, const char *channel_name);

// Remove functions
int part_channel(const char *user_nick, const char *channel_name);
int remove_user(const char *user_nick);
int remove_channel(const char *channel_name);

// Edit functions
int set_channel_topic(const char *channel_name, const char *topic);
int change_nick(const char *current_nick, const char *new_nick);

#endif /* defined(__Chachat__db__) */
