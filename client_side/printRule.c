#include "client.h"



void printChatRule(){
	
	printf("###########################################################\n");
	printf("%sThe following are standard rules:\n",COLOR_NRM);
	printf("%sTo start chatting type your nickname below.\n",COLOR_RED );
	printf("%sTo chat with a friend type /chat \n",COLOR_GRN);
	printf("%sTo join a channel type /join \n",COLOR_YEL);
	printf("%sTo exit type /exit\n",COLOR_BLU);
	printf("%sTo quit type /quit\n",COLOR_RED);
	printf("%sTo continue chat with a friend after joining a channel,\nsimply issue the chat command again\n",COLOR_BLU);
	printf("%sTo continue a channel discussion after starting a chat with a friend,\nsimply issue the join command again\n",COLOR_CYN);
	printf("%sAnd you can only make mistakes 3 times.\n",COLOR_RED);
	printf("%sIf you have any problem, simply type: help\n",COLOR_WHT);
	printf("%sDo have fun! using our service. :)\n",COLOR_YEL);
	printf("%s###########################################################\n",COLOR_RESET);
	
}


void *threadRead()
{
	int connfd, n=0, i = 0;
	char *recv;
	Msgheader chatheader;
	
	while(1)
	{
		usleep(READ_TIMEOUT_USEC);
		printf(" says: %d\n",i);
/*		if ((n = read_message(connfd, recv, &chatheader))<0) {*/
/*				perror("read_message:");*/
/*				return -1;*/
/*		}*/
		
		++i;
	}

	return NULL;
}
