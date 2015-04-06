#include "client.h"
  

//Handling client's quite command message
void quitMessageHandle(int connfd, Msgheader chatheader){

	
	printf("Thanks for using our service\n We are always available to serve you.\n");
	close(connfd);
	
}








