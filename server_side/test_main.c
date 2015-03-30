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
    
    char *mesbuff;
    mesbuff=malloc(MAXMSG*sizeof(char)+1);
    Msgheader mesheader;
   // mesheader=malloc(sizeof(Msgheader)); //this sizeof has sizes of nick pointers.
    
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
	
	//set content to zero
	memset(&mesheader, 0, sizeof(mesheader));
    mesheader.firstbyte='0';
	mesheader.msglen=0;
	mesheader.recipient_id="0";
    mesheader.sender_id=client_nick(connfd);
    
    // for (; ; ) {
    /* if(read_message(connfd,mesbuff,mesheader)<0){
        perror("read_message pox!\n");
        return -1;
    } */
	//printf("----header-----\n");
	printf("firstbyte set to: %c\n", mesheader.firstbyte);
	printf("msglen: %u\n", mesheader.msglen);
	printf("sender: %s", mesheader.sender_id);
	printf("recipient: %s\n",mesheader.recipient_id);
    //print_hdr(&mesheader);
	
	memset(&mesbuff, 0, sizeof(mesbuff));
	/* if(read(connfd,mesbuff,sizeof(mesbuff))<0){
        perror("read_message box!\n");
        return -1;
	}	 */
	read_message(connfd, mesbuff, &mesheader);
	
	
		    
	/* if (strcmp(mesbuff,"chat")==0) {//client sends private chat message
			printf(" private chat from %s  :)\n", mesheader.sender_id);
			chatMessageHandle(connfd, mesbuff, &mesheader); */
    
	if (mesheader.firstbyte=='1') {//client sends channel chat message 
		chatMessageHandle(connfd, mesbuff, &mesheader);
	
    
   }else if (mesheader.firstbyte=='2') {//client sends channel chat message 
		chanMessageHandle(connfd,mesbuff,&mesheader);
				
				
   } else if (mesheader.firstbyte=='3') {//client sends channel chat message
		quitMessageHandle(connfd,mesbuff,&mesheader);
			
			
   } else{
		printf("There must be error somewhere arrgh :(\n");
				
	}
    
	 
    

    
        
   // }
    free(mesbuff);
    //free_hdr(&mesheader);
    return 0;

    
}

