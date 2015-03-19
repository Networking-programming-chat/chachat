#ifndef COMMON_H  //This is a preprocessor technique of preventing a header file from being included multiple times, which can be problematic for various reasons.
#define COMMON_H


#include <sys/socket.h>

//
//client function:
// create a socket
// connection to the server
//
//argu: char *host is the string of server's name
//     char *serv is the port of server in string format
//return: the sock desciptor
int client_connect(const char *servName, const char *servPort);


#endif
