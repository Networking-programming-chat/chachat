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
	sprintf(nickname, "%s", nick);
	
return len;
}