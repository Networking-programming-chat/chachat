#ifndef COMMON_H
#define COMMON_H

#include <sys/socket.h>

#define LISTENQ 5
#define MAX_NICKLEN 13	//maxium length of nickname

/*Establis a listening socket
*argu: char *host is the string of server's name
* char *serv is the port of server in string format
* return: the listening sock desciptor
*/
int serv_listen(const char *host, const char *serv);

/*print the connected address*/
void print_address(const struct addrinfo * res);

/*readn nickname from the client
 */
int read_nickname(int socket,char *);

/*add client nickname in the database*/
int client_nick(int socket);


#endif
