#ifndef CLIENT_H 
#define CLIENT_H

#include <sys/socket.h>
#include "../networking/netproto.h"
#include <stdio.h>
#include <sys/socket.h> //socket.
#include <strings.h> //bzero
#include <netdb.h> // addrinfo
#include <unistd.h> //close
#include <stdlib.h>  
#include <fcntl.h> 
#include <string.h>
#include <pthread.h>

#define MAX 50

//client function:
int client(const char *servName, const char *servPort);

//prints chat rules and help
void printChatRule();

//thread for reading incoming messages
void *threadRead();

//handles input at the start of chat 
int startChat(char *getCmd, char *getName);

//handles the client's nickname
char *client_nick(int sockfd);

//handles client's connection to the server
int client_connect(const char *servName, const char *servPort);

//Handling client's normal message
int chatMessageHandle(int connfd, char *send, char *recv, Msgheader chatheader);

//Handling client's channel join command message
int chanMessageHandle(int connfd, char *send, char *recv, Msgheader chatheader);

//Handling client's exits command message
void exitMessageHandle(int connfd, Msgheader chatheader);

//Handling client's quite command message
void quitMessageHandle(int connfd, Msgheader chatheader);
#endif
