//
//tcp_connect function:
// create a socket
// connection to the server
//
//argu: char *servName is the string of server's name
//     char *servPort is the port of server in string format

//return: the sock desciptor of connection
#include "client.h"
#include <stdio.h>
#include <sys/socket.h> //socket.
#include <strings.h>  //bzero
#include <netdb.h> // addrinfo
#include <unistd.h> //close

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

int client(const char *servName, const char *servPort){
    int n,sockfd;
    struct addrinfo hints, *res,*ressave;
    
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    
    
    if ( (n = getaddrinfo(servName, servPort, &hints, &res)) != 0) {
        fprintf(stderr, "tcp_connect error for %s, %s: %s\n",
                servName, servPort, gai_strerror(n));
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
        fprintf(stderr, "tcp_connect error for %s, %s\n", servName, servPort);
        sockfd = -1;
    } else {
        printf("connect to server success\n");
    }
    
    freeaddrinfo(ressave);
    
    return(sockfd);
}

