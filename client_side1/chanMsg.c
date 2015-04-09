#include "client.h"
#include <stdlib.h>




//Handling client's channel join command message
int chanMessageHandle(int connfd, char *send, Msgheader chatheader){

<<<<<<< HEAD
int n=1;
=======
int n=0;
//pthread_t read;
//struct threadParam tParams;
recv = (char *) malloc(MAXMSG);
/*tParams.header=chatheader;*/
/*tParams.recvbuf=recv;*/
/*tParams.conn=connfd;*/

thread_s *thread;
thread = malloc(sizeof(thread_s));
thread->socketfd = connfd;
thread->recvbuf = recv;
thread->header = chatheader;

>>>>>>> 7da416bb3dae1c028b3fbd03489f547a9cd42c51

			//always send the string length of the sent msg
	        chatheader.msglen=strlen(send); 
	        
	        //sends source client's message
			if ((n=pass_message(connfd, send, &chatheader))<0) {
				perror("pass_message:");
				return -1;
			}
<<<<<<< HEAD
			
=======
			//printf("fd should be: %d\n",tParams.conn);
			
/*			//receives destination client's message	*/
			
            n = pthread_create(&(thread->thread_id), NULL, reader_thread, (void*)thread);
>>>>>>> 7da416bb3dae1c028b3fbd03489f547a9cd42c51
	
			send[n] = 0;        // null terminate 
			if (fputs(send, stdout) == EOF) {
				fprintf(stderr, "fputs error\n");
				return -1;
			}
	
 return 0;
}
