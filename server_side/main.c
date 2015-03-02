//
//  main.c
//  Chachat_1
//
//  Created by Judy on 3/1/15.
//  Copyright (c) 2015 Judy. All rights reserved.
//

#include <stdio.h>
#include "serv.h"

//type ip address(or not), port number.
int main(int argc, const char * argv[]) {
    int listenfd;
    int connfd;
    socklen_t addrlen, clilen;
    struct sockaddr *cliaddr=NULL;
    
    if (argc==2)
        listenfd=serv_listen(NULL, argv[1], &addrlen);
    else if (argc==3)
        listenfd=serv_listen(argv[1], argv[2], &addrlen);
    else {
        fprintf(stderr, "usage: ./main <host> <port#>\n ");
        return -1;
        
    }
    
    printf("listenfd: %d",listenfd);
    
    /*waiting for coming connection*/
    
    for (; ; ) {
        if ((connfd=accept(listenfd,cliaddr, &clilen))<0) {
            perror("accept error\n");
            return -1;
        }
        
        //subthread for the connfd
        //......
    }
}
