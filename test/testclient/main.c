//
//  main.c
//  testclient
//
//  Created by Judy on 3/13/15.
//  Copyright (c) 2015 Judy. All rights reserved.
//

#include <stdio.h>
#include <sys/socket.h> //socket.
#include <strings.h>  //bzero
#include <netdb.h> // addrinfo
#include <unistd.h> //close
#include <arpa/inet.h> //inet_ntop
#include <netinet/in.h>  //htonl
#include "netproto.h"

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

int tcp_connect(const char *host, const char *serv)//This function is borrowed from the lecture examples
{
    int sockfd, n;
    struct addrinfo	hints, *res, *ressave,*ressave1;
    
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0) {
        fprintf(stderr, "tcp_connect error for %s, %s: %s\n",
                host, serv, gai_strerror(n));
        return -1;
    }
    
    ressave = res;
    ressave1 =res;
    
    printf("The ip addresses of the server are\n");
    do{
        
        print_address(ressave1);
    }while( (ressave1=ressave1->ai_next) !=NULL);
    
    do {
        sockfd = socket(res->ai_family, res->ai_socktype,
                        res->ai_protocol);
        if (sockfd < 0)
            continue;	/* ignore this one */
        
        print_address(res);
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;		/* success */
        printf("connect failed\n");
        
        close(sockfd);	/* ignore this one */
    } while ( (res = res->ai_next) != NULL);
    
    if (res == NULL) {	/* errno set from final connect() */
        fprintf(stderr, "tcp_connect error for %s, %s\n", host, serv);
        sockfd = -1;
    } else {
        printf("we are using:\n");
        print_address(res);
    }
    
    freeaddrinfo(ressave);
    
    return(sockfd);
}


int main(int argc, const char * argv[]) {
 
    char nickname[20]="nickname";
    char recipient[20]="recipient";
    char dest[20]="destination";
    char message[]="hello";
    Msgheader header;
    
    header.firstbyte='0';
    header.msglen=strlen(message);
    header.recipient_id=recipient;
    header.sender_id=dest;
    
    
    int socked;
    ssize_t n1;
    
    if ((socked=tcp_connect(argv[1], argv[2]))<0) {
        perror("tcp connection error\n");
        return -1;
    }
    
    //printf("the connected sockfd is: %d\n",socked);
    
    if ((n1=write(socked, nickname, 20))<0) {
        perror("write nickname wrong\n");
        return -1;
    }
    
    if ((n1=pass_message(socked, message, &header))<0) {
        perror("pass_message\n");
        return -1;
    }
    
    
    return 0;
}
