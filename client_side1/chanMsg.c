#include "client.h"
#include <stdlib.h>




//Handling client's channel join command message
int chanMessageHandle(int connfd, char *send, Msgheader chatheader){

int n=1;

			//always send the string length of the sent msg
	        chatheader.msglen=strlen(send); 
	        
	        //sends source client's message
			if ((n=pass_message(connfd, send, &chatheader))<0) {
				perror("pass_message:");
				return -1;
			}
			
	
			send[n] = 0;        // null terminate 
			if (fputs(send, stdout) == EOF) {
				fprintf(stderr, "fputs error\n");
				return -1;
			}
	
 return 0;
}