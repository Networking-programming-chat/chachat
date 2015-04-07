#include "client.h"
#include <stdlib.h>




//Handling client's private chat command and message
int chatMessageHandle(int connfd, char *send, char *recv, Msgheader chatheader){

int n=0;
pthread_t read;

			//always send the string length of the sent msg
	        chatheader.msglen=strlen(send); 
	        
	        //sends source client's message
			if (pass_message(connfd, send, &chatheader)<0) {
				perror("pass_message:");
				return -1;
			}
					
			printf("%s%s:> ",COLOR_GRN,chatheader.recipient_id);
			//receives destination client's message	
			//pthread_create(&read,NULL,threadRead,NULL);
	
			recv[n] = 0;        // null terminate 
			if (fputs(recv, stdout) == EOF) {
				fprintf(stderr, "fputs error\n");
				return -1;
			}
		//}
 return 0;
}

