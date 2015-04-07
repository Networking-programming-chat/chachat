#include "client.h"
#include <stdlib.h>




//Handling client's private chat command and message
int chatMessageHandle(int connfd, char *send, char *recv, Msgheader chatheader){

int n=0;
connfd=0;
pthread_t read;
struct arg_struct tParams;
recv = (char *) malloc(MAXMSG);
tParams.header=chatheader;
tParams.recvbuf=recv;
tParams.conn=connfd;
			//always send the string length of the sent msg
	        chatheader.msglen=strlen(send); 
	        
	        //sends source client's message
			if (pass_message(connfd, send, &chatheader)<0) {
				perror("pass_message:");
				return -1;
			}
			
			
			//receives destination client's message	
			pthread_create(&read,NULL,threadRead,&tParams);
	
			recv[n] = 0;        // null terminate 
			if (fputs(recv, stdout) == EOF) {
				fprintf(stderr, "fputs error\n");
				return -1;
			}
	free(recv);
 return 0;
}
