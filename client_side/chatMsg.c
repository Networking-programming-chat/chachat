#include "client.h"
#include <stdlib.h>




//Handling client's private chat command and message
int chatMessageHandle(int connfd, char *send, char *recv, Msgheader chatheader){

int n=0;
//pthread_t read;

recv = (char *) malloc(MAXMSG);
send = (char *) malloc(MAXMSG);

thread_s *thread;
thread = malloc(sizeof(thread_s));
thread->socketfd = connfd;
thread->recvbuf = recv;
thread->writbuf = send;
thread->header = chatheader;


			//always send the string length of the sent msg
	        chatheader.msglen=strlen(send); 
	        if ((n=pass_message(connfd, send, &chatheader))<0) {
				perror("pass_message:");
				return -1;
			}
	            //n = pthread_create(&(thread->thread_id), NULL, writer_thread, (void*)thread);
            	n = pthread_create(&(thread->thread_id), NULL, reader_thread, (void*)thread);
	        
			recv[n] = 0;        // null terminate 
			if (fputs(recv, stdout) == EOF) {
				fprintf(stderr, "fputs error\n");
				return -1;
			}
	free(recv);
	free(send);
 return 0;
}



