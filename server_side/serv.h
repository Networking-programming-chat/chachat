#ifndef COMMON_H
#define COMMON_H

#include <sys/socket.h>

#define LISTENQ 5


/*Establis a listening socket
*argu: char *host is the string of server's name
* char *serv is the port of server in string format
* return: the listening sock desciptor
*/
int serv_listen(const char *host, const char *serv, socklen_t *addrlenp);


#endif
