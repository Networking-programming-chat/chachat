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
#include <time.h>
#define READ_TIMEOUT 100000000
#define MAX 50
#define COLOR_NRM   "\x1B[0m"
#define COLOR_RED   "\x1B[31m"
#define COLOR_GRN   "\x1B[32m"
#define COLOR_YEL   "\x1B[33m"
#define COLOR_BLU   "\x1B[34m"
#define COLOR_MAG   "\x1B[35m"
#define COLOR_CYN   "\x1B[36m"
#define COLOR_WHT   "\x1B[37m"
#define COLOR_RESET "\033[0m"
#define BRIGHT 1
#define THREAD_CNT 2


struct threadParam {
    int conn;
    char *recvbuf;
    Msgheader header;
};

typedef struct {
    pthread_t thread_id;
    int socketfd;
	char *recvbuf;
	Msgheader header;
} thread_s;

//client function:
int client(const char *servName, const char *servPort);

//prints chat rules and help
void printChatRule();

//prints the timestamp
char* get_timestamp(char buffer [20]);

//thread for reading incoming messages
void *reader_thread(void *arg);

//handles input at the start of chat 
int startChat(char *getCmd, char *getName);

//handles the client's nickname
int client_nick(int sockfd, char *getName);

//handles client's connection to the server
int client_connect(const char *servName, const char *servPort);

//Handling client's normal message
int chatMessageHandle(int connfd, char *send, Msgheader chatheader);

//Handling client's channel join command message
int chanMessageHandle(int connfd, char *send, Msgheader chatheader);

//Handling client's exits command message
void exitMessageHandle(int connfd, Msgheader chatheader);

//Handling client's quite command message
void quitMessageHandle(int connfd, Msgheader chatheader);
#endif
