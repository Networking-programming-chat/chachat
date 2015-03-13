//
//  main.c
//  Chachat_1
//
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
                
    for (; ; ) {
        if ((connfd=accept(listenfd,cliaddr, &clilen))<0) {
            perror("accept error\n");
            return -1;
        }
        
         client_nick(connfd);
        
    }
    
}
            
