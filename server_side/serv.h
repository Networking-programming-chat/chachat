#ifndef COMMON_H
#define COMMON_H

#include <sys/socket.h>

#define LISTENQ 5

//client sends this as the first thing after connect, perhaps optional
typedef struct{
    char nickname[20];//user specified screen name. 1 byte per char or something.
}Clienthello;

/*Establis a listening socket
*argu: char *host is the string of server's name
* char *serv is the port of server in string format
* return: the listening sock desciptor
*/
int serv_listen(const char *host, const char *serv);

void print_address(const struct addrinfo * res);//print the connected address

/*Deal with the comming connection
 */
int read_nickname(int socket);

#endif
