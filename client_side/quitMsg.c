#include "client.h"
  

//Handling client's quite command message
void quitMessageHandle(int connfd,char *getCmd, Msgheader chatheader){


	if(pass_message(connfd, getCmd, &chatheader)<0){
		printf("there was problem quiting\n");
	}
	
	printf("Thanks for using our service\n We are always available to serve you.\n");
	close(connfd);
	
}








