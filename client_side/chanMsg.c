#include "client.h"
#include <stdlib.h>




//Handling client's channel join command message
int chanMessageHandle(int connfd, char *send, char *recv, Msgheader chatheader){

int n=1;
pthread_t read;
struct threadParam tParams;
recv = (char *) malloc(MAXMSG);
tParams.header=chatheader;
tParams.recvbuf=recv;
tParams.conn=connfd;

thread_s *thread;
thread = malloc(sizeof(thread_s));



			//always send the string length of the sent msg
	        chatheader.msglen=strlen(send); 
	        
	     //while(n!=0){
	        //sends source client's message
			if (n=pass_message(connfd, send, &chatheader)<0) {
				perror("pass_message:");
				return -1;
			}
			printf("fd should be: %d\n",tParams.conn);
			
/*			//receives destination client's message	*/
			thread->socketfd = connfd;
			thread->recvbuf = recv;
			thread->header = chatheader;
            n = pthread_create(&(thread->thread_id), NULL, reader_thread, (void*)thread);
	
			recv[n] = 0;        // null terminate 
			if (fputs(recv, stdout) == EOF) {
				fprintf(stderr, "fputs error\n");
				return -1;
			}
	free(recv);
 return 0;
}
