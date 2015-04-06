#include "client.h"



void printChatRule(){
	
	printf("###########################################################\n");
	printf("The following are standard rules:\n");
	printf("To start chatting type your nickname below.\n");
	printf("To chat with a friend type /chat \n");
	printf("To join a channel type /join \n");
	printf("To exit type /exit\n");
	printf("To quit type /quit\n");
	printf("You can only manage a max of 3 sessions of \neither chatting or channels at a time\n");
	printf("To continue chat with a friend after joining a channel,\n simply issue the chat command again\n");
	printf("To continue a channel discussion after starting a chat with a friend,\n simply issue the join command again\n");
	printf("If you have any problem, simply type: help\n");
	printf("Do have fun! using our service. :)\n");
	printf("###########################################################\n");
	
}


void *threadRead()
{
	int connfd, n=0, i = 0;
	char *recv;
	Msgheader chatheader;
	
	while(i < 10 )
	{
		usleep(1);
		printf(" says: %d\n",i);
/*		if ((n = read_message(connfd, recv, &chatheader))<0) {*/
/*				perror("read_message:");*/
/*				return -1;*/
/*		}*/
		
		++i;
	}

	return NULL;
}
