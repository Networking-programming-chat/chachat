#ifndef COMMON_H
#define COMMON_H

#include <sys/socket.h>
#include "netproto.h"
#include "db.h"
#include "msg_buffers.h"

#define LISTENQ 5

/*Establis a listening socket
*argu: char *host is the string of server's name
* char *serv is the port of server in string format
* return: the listening sock desciptor
*/
int serv_listen(const char *host, const char *serv);

/*print the connected address*/
void print_address(const struct addrinfo * res);

/*read nickname from the client
 */
int read_nickname(int socket,char *);

/*add client nickname in the database*/
int client_nick(int socket,char *nickname,cc_user *user1);

//Handling client's private message
void chatMessageHandle(int connfd, char *mesbuff, Msgheader *mesheader);

//Handling client's channel message
void chanMessageHandle(int connfd,char *mesbuff, Msgheader *mesheader);

//Handling client's exit channel message
void exitChanMessageHandle(int connfd,char *mesbuff, Msgheader *mesheader);

//Handling client's quite command message
void quitMessageHandle(int connfd,char *mesbuff, Msgheader *mesheader);


#endif
