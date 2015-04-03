#include "client.h"
#include <stdlib.h>

//Handling client's private chat command and message
int chatMessageHandle(int connfd, char *send, char *recv, char *getCmd, Msgheader chatheader){

int n;
    //sends the chat channel command message to the server.
	if(pass_message(connfd, getCmd, &chatheader)<0){
		printf("there was problem connecting to a chat client\n");
	}
		printf("%s:",chatheader.sender_id);
		memset(send, 0, sizeof(send));
		
		//a loop to continuously send message to the other client through the server.
		while (fgets(send, MAXMSG, stdin) != NULL) {
					
			printf("%s: ",chatheader.sender_id);
			//always send the string length of the sent msg
	        chatheader.msglen=strlen(send); 
	        
	        //sends source client's message
			if (pass_message(connfd, send, &chatheader)<0) {
				perror("sendto");
				return -1;
			}
					
			printf("%s: ",chatheader.recipient_id);
			//receives destination client's message	
			if ((n = read_message(connfd, recv, &chatheader))<0) {
				perror("recvfrom");
				return -1;
			}
	
			recv[n] = 0;        // null terminate 
			if (fputs(recv, stdout) == EOF) {
				fprintf(stderr, "fputs error\n");
				return -1;
			}
		}
 return 0;
}

