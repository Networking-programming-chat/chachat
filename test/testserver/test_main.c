//
//  main.c
//  chachat1
//
//  Created by Judy on 3/13/15.
//  Copyright (c) 2015 Judy. All rights reserved.
//

#include <stdio.h>
#include "serv.h"
#include <unistd.h> //read


//type ip address(or not), port number.
int main(int argc, const char * argv[]) {
    int listenfd;
    int connfd;
    socklen_t clilen;
    struct sockaddr *cliaddr=NULL;
    
    char *mesbuff;
    mesbuff=(char *)malloc(MAXMSG*sizeof(char));
    Msgheader *mesheader;
    
    //open listenfd for clients
    if (argc==2)
        listenfd=serv_listen(NULL, argv[1]);
    else if (argc==3)
        listenfd=serv_listen(argv[1], argv[2]);
    else {
        fprintf(stderr, "usage: ./main <host> <port#>\n ");
        return -1;
        
    }
    printf("listenfd: %d\n",listenfd);
    
    /*waiting for coming connection*/
    
    
    
    if ((connfd=accept(listenfd,cliaddr, &clilen))<0) {
        perror("accept error\n");
        return -1;
    }
    client_nick(connfd);
        
    
   // for (; ; ) {
        if(read_message(connfd,mesbuff,mesheader)<0)
            return -1;
        
        printf("message buffer %s\n",mesbuff);
    
    print_hdr(mesheader);
    
        if (mesheader->firstbyte=='0') {//client sends normal message
            normalMessageHandle(mesbuff,mesheader);
        }
        
        if (mesheader->firstbyte=='2') {//client sends command
            commandMessageHandle(connfd,mesbuff,mesheader);
        }
        
        
        
   // }
    
}

