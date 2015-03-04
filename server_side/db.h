
#ifndef __Chachat__db__
#define __Chachat__db__

#include "sqlite3.h"

void init_db(sqlite3 *db);      // Will initialize the db (create if not present and load)

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
