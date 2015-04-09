#ifndef CLIENT_H 
#define CLIENT_H

#include <sys/socket.h>
#include <stdio.h>
#include <sys/socket.h> //socket.
#include <strings.h> //bzero
#include <netdb.h> // addrinfo
#include <unistd.h> //close 
#include <fcntl.h> 
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "netproto.h"
#include <sys/types.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>
#include <stdarg.h> // va_ args
#include <stdlib.h> // exit()
#include "terminal_color.h"



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
#define MAXCHAN 3
#define MAXCHAT 3

//prints the timestamp
int get_timestamp(char buffer [20]);

//set nickname
int client_nick(int sockfd, char *nickname);

void printChatRule();

//Handling client's private chat command and message
int chatMessageHandle(int connfd, char *send, Msgheader chatheader);

//Handling client's channel join command message
int chanMessageHandle(int connfd, char *send, Msgheader chatheader);

//handling sennding message to server
int send_message(int connect,char *nickname);

#endif
