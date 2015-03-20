#ifndef CHACHAT_CLIENT_HANDLING_H
#define CHACHAT_CLIENT_HANDLING_H

#include <sys/socket.h>
#include "netproto.h"
#include "db.h"

/*functions for s2c threads! declare in this file;


void db_syncer();
void command_propagator();
void message_propagator();

db query functions in db.h;

 TODO: what functions do we need
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
void handle_command();
 
//passes data to other clients!
int pass_data(thread_id thread, char* data, Msgheader* hdr);

//copied the stat_routine3 here and started poking it.
int s2c_mainloop(int fd);

#endif
