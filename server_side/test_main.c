//
//  main.c
//  chachat1
//
//  Created by Judy on 3/13/15.
//  Copyright (c) 2015 Judy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <unistd.h> //read
#include <string.h>

#include "serv.h"
#include "db.h"

//type ip address(or not), port number.
int main(int argc, const char * argv[]) {
    int listenfd;
    int connfd;
    socklen_t clilen;
    struct sockaddr *cliaddr=NULL;
    
    char *mesbuff,*nickname;
    mesbuff=malloc(MAXMSG*sizeof(char)+1);
    Msgheader mesheader;
    
    // Initialize database
    init_db();
    
    //open listenfd for clients
    if (argc==2)
        listenfd=serv_listen(NULL, argv[1]);
    else if (argc==3)
        listenfd=serv_listen(argv[1], argv[2]);
    else {
        fprintf(stderr, "usage: %s <host> <port#>\n ", argv[0]);
        return -1;
        
    }
    printf("listenfd: %d\n",listenfd);
    
    /*waiting for coming connection*/
    
    if ((connfd=accept(listenfd,cliaddr, &clilen))<0) {
        perror("accept error\n");
        return -1;
    }
		
    
    if((nickname=client_nick(connfd))==NULL){
        printf("client set nick name error\n");
        return -1;
    }
    
   // while (1) {
        
        memset(&mesheader, 0, sizeof(mesheader));
        memset(&mesbuff, 0, sizeof(mesbuff));
        
        if(read_message(connfd, mesbuff, &mesheader)<0){
            perror("read from client error\n");
            return -1;
        }
        
        if (mesheader.firstbyte=='1') {//client sends private chat message
            chatMessageHandle(connfd, mesbuff, &mesheader);
            
            
        }else if (mesheader.firstbyte=='2') {//client sends channel chat message
            chanMessageHandle(connfd,mesbuff,&mesheader);
            
            
        } else if (mesheader.firstbyte=='3') {//client sends quit command
            quitMessageHandle(connfd,mesbuff,&mesheader);
            
            
        } else{
            printf("There must be error somewhere arrgh \n");
            
        }
  //  }
    
    
    
    close_db();
    
    free(mesbuff);
    
    return 0;

    
}

