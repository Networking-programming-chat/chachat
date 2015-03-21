#ifndef CHACHAT_CLIENT_H 
#define CHACHAT_CLIENT_H

#include <sys/socket.h>
#include "../networking/netproto.h"
#include <stdio.h>
#include <sys/socket.h> //socket.
#include <strings.h> //bzero
#include <netdb.h> // addrinfo
#include <unistd.h> //close
#include <unistd.h>  
#include <fcntl.h> 

#define MAX 50

//client function:
int client(const char *servName, const char *servPort);

//prints chat rules and help
int printChatRule(char *getCmd, char *getName);

//handles the client's nickname
int client_nick(int sockfd, char *nickname);

//handles client's connection to the server
int client_connect(const char *servName, const char *servPort);

//Handling client's normal message
void chatMessageHandle(int connfd, char *send, char *recv, char *getCmd, Msgheader chatheader);

//Handling client's channel join command message
void chanMessageHandle(int connfd, char *send, char *recv, char *getCmd, Msgheader chatheader);

//Handling client's quite command message
void quitMessageHandle(int connfd,char *mesbuff, Msgheader chatheader);
#endif
