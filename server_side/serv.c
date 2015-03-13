#include <stdio.h>
#include <sys/socket.h> //socket.
#include <strings.h>  //bzero
#include <netdb.h> // addrinfo
#include <unistd.h> //close
#include <arpa/inet.h> //inet_ntop
#include "serv.h"



 /*This function is borrowed from example
  *Opening an listened fd.
  */
int serv_listen(const char *host, const char *serv){
        int listenfd, n;
        const int on = 1;
        struct addrinfo hints, *res, *ressave;

        bzero(&hints, sizeof(struct addrinfo));
        hints.ai_flags = AI_PASSIVE;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0) {
                fprintf(stderr, "tcp_listen error for %s, %s: %s",
                                 host, serv, gai_strerror(n));
		return -1;
	}
        ressave = res;

        do {
                listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
                if (listenfd < 0)
                        continue;               /* error, try next one */

                setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
                if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
                        break;                  /* success */

                close(listenfd);        /* bind error, close and try next one */
        } while ( (res = res->ai_next) != NULL);

        if (res == NULL) {        /* errno from final socket() or bind() */
            fprintf(stderr, "tcp_listen error for %s, %s", host, serv);
            return -1;
        }

    
    if (listen(listenfd, LISTENQ) < 0) {
		perror("listen");
		return -1;
        }
    
    printf("The ip address we are using is: ");
    print_address(res);
    
    freeaddrinfo(ressave);
    
    return(listenfd);
}

void print_address(const struct addrinfo *res)//this function is based on lecture example
{
    char outbuf[80];
    struct sockaddr_in *sin = (struct sockaddr_in *)res->ai_addr;
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)res->ai_addr;
    void *address;
    
    if (res->ai_family == AF_INET)
        address = &(sin->sin_addr);
    else if (res->ai_family == AF_INET6)
        address = &(sin6->sin6_addr);
    else {
        printf("Unknown address\n");
        return;
    }
    
    //struct sockaddr_in *sin = (struct sockaddr_in *)res->ai_addr;
    const char *ret = inet_ntop(res->ai_family, address,
                                outbuf, sizeof(outbuf));
    printf("%s\n",ret);
}


int client_nick(int socket){
    char nickname[MAX_NICKLEN];
    if(read_nickname(socket,nickname)!=0)
        return -1;
    
    /*---check if the nickname is already used-----*/
    /*----The nickname is stored in char nickname[]------*/
    /*--If already used, return 1, if not, return 0*/
    
    return 0;
}

int read_nickname(int socket,char *nickname){
    ssize_t n1;
    
    bzero(nickname, sizeof(nickname));
    if ((n1=read(socket,nickname, sizeof(nickname)))<0) {
        printf("read client name error\n");
        return -1;
    }
    
    printf("%s\n",nickname);
    
    return 0;
}
