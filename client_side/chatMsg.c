#include "client.h"



void chatMessageHandle(int connfd, char *send, char *recv, char *getCmd, Msgheader chatheader){

int n;

	if(pass_message(connfd, getCmd, &chatheader)<0){
		printf("there was problem connecting to a chat client\n");
	}
		printf("%s: ",chatheader.sender_id);
		memset(send, 0, sizeof(send));
		while (fgets(send, MAXMSG, stdin) != NULL) {
					
			printf("%s: ",chatheader.sender_id);
			//always send the string length of the sent msg
	        chatheader.msglen=strlen(send); 
			if (pass_message(connfd, send, &chatheader)<0) {
				perror("sendto");
				return -1;
			}
					
			printf("%s: ",chatheader.recipient_id);
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
}

