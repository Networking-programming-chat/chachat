#include "client.h"

int client_nick(int sockfd, char *nickname){

char nick[MAX];
ssize_t n1;
int len, nickCnt=3;
    printf("To start chatting type your nickname below.\n");
    //set content to zero
    //memset(nickname, 0, sizeof(nickname));
    //give user 3 chances to give correct nickname otherwise this connect them
    while(nickCnt != 0){
	  printf ("Enter your nickname: ");
	  fgets(nick,sizeof(nick),stdin);
	  len = strlen(nick);
	  //printf ("The nickname entered is %d characters long.\n",len);
  
  	  if(len < MAX_NICKLEN+1){
		printf("what a nice nick %s  ",nick);
		nickCnt = 0;
	  }else{
		printf ("Your nickname is too long\n");
		nickCnt--;
		printf("you have %d options left.\n", nickCnt);
		if(nickCnt==0){
			printf("you ran out of options?\n Try reconnect again :)\n");
		              }
	  }
    }
	if ((n1=write(sockfd, nick, strlen(nick)))<0) {
		printf("read client name error\n");
	return -1;
	}
	// Empty the buffer used in storing the nickname.
	bzero(nick,sizeof(nick));
	
	uint16_t nickExist, existCnt=0;
	//stores the return value received after sending the nickname to the server.
	if ((n1=read(sockfd, &nickExist, strlen(nickExist)))<0) {
		printf("read client name error\n");
	return -1;
	}
	
	nickExist = ntohs(nickExist);
	while(nickExist== 2){
	  printf ("Nickname already in use, enter a new nickname: ");
	  fgets(nick,sizeof(nick),stdin);
	  if ((n1=write(sockfd, nick, strlen(nick)))<0) {
			printf("send client name error\n");
			return -1;
	  }
	  
	  if ((n1=read(sockfd, &nickExist, strlen(nickExist)))<0) {
		printf("read client name error\n");
		return -1;
	  }
	  existCnt++;
	  //break the loop after user sends an existing 3tims to the server.          
	  if(existCnt==3){
		  printf("you ran out of options?\n Try reconnect again :)\n");
		  break;
	  }	  
	}
	
	
	
	sprintf(nickname, "%s", nick);
	
return len;
}
