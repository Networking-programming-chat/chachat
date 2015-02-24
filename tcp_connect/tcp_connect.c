//
//tcp_connect function:
// create a socket
// connection to the server
//
//argu: char *host is the string of server's name
//     char *serv is the port of server in string format

//return: the sock desciptor of connection
#include "tcp_connect.h"
#include <stdio.h>
#include <sys/socket.h> //socket.
#include <strings.h>  //bzero
#include <netdb.h> // addrinfo
#include <unistd.h> //close

int tcp_connect(const char *host, const char *serv){
    int n,sockfd;
    struct addrinfo hints, *res,*ressave;
    
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0) {
        fprintf(stderr, "tcp_connect error for %s, %s: %s\n",
                host, serv, gai_strerror(n));
        return -1;
    }
    
    if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0) {
        fprintf(stderr, "tcp_connect error for %s, %s: %s\n",
                host, serv, gai_strerror(n));
        return -1;
    }
    
    ressave = res;

    
    do {
        sockfd = socket(res->ai_family, res->ai_socktype,
                        res->ai_protocol);
        if (sockfd < 0)
            continue;	/* ignore this one */
        
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;		/* success */
        
        close(sockfd);	/* ignore this one */
    } while ( (res = res->ai_next) != NULL);
    
    if (res == NULL) {	/* errno set from final connect() */
        fprintf(stderr, "tcp_connect error for %s, %s\n", host, serv);
        sockfd = -1;
    } else {
        printf("connect to server success\n");
    }
    
    freeaddrinfo(ressave);
    
    return(sockfd);
}

