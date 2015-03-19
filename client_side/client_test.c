#include <stdio.h>
#include "client.h"
#include "netproto.h"
#define MAX 50


int main(void) {
char *hostAddr="localhost";
char *hostPort="3333", nick_name[MAX];
int connect, nickSend, nickCnt=3, len, n;
Msgheader clientsChat;
char sentMsg[MAXMSG], recvMsg[MAXMSG];
	if ((connect=client_connect(hostAddr, hostPort))<0) {
		perror("tcp connection error\n");
	return -1;
	}else{
		printf("To start chatting type /nick your_nickname\n");
	}
    //set content to zero
    memset(nick_name, 0, sizeof(nick_name));
    //give user 3 chances to give correct nickname otherwise this connect them
    while(nickCnt != 0){
	  printf ("Enter your nickname: ");
	  fgets(nick_name,sizeof(nick_name),stdin);
	  len = strlen(nick_name);
	  //printf ("The nickname entered is %d characters long.\n",len);
  
  	  if(len < MAX_NICKLEN ){
		printf("what a nice nick! :)\n");
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
        //set content to zero
	memset(&clientsChat, 0, sizeof(clientsChat));
	//set message header fields appropriately
	clientsChat.sender_id=nick_name;
	clientsChat.firstbyte='0';
	clientsChat.msglen=0;
	clientsChat.recipient_id=0;
        //use to send just the nick name to the server
	if(nickSend = write(connect, clientsChat, sizeof(clientsChat)) < 0){
		printf("There is problem writing nickname to the server\n");
	}else{
		printf("###########################################################\n");
		printf("The following are standard rules:\n");
		printf("To chat with a friend type /chat @receipient's_nickname\n");
		printf("To join a channel type /join #channel_name\n");
		printf("To quit type /quit\n");
		printf("###########################################################\n");
	
		
		//set content to zero
		memset(&clientsChat, 0, sizeof(clientsChat));
		//string formating going on
		char command[30], cmd[6], name[21], c1, c2;
		fgets(command,sizeof(command),stdin);
		//detach the (/) xter and the (@) xter 
		sscanf(command, " %c %s %c %s", &c1, cmd, &c2, name);
		//set content of command to zero and write to it again
		memset(command, 0, sizeof(command));
		sprintf(command, "%s %s", cmd, name);
		
		int cas;
		if(strcmp(cmd,"chat")==0){
			cas=1;
			
		}else if(strcmp(cmd,"join")==0){
		    cas=2;
		    
		}else if(strcmp(cmd,"quit")==0){
		    cas=3;
		    
		}
		
		
		switch(cas){
		
		case 1:
		        //set message header fields appropriately
			clientsChat.sender_id=nick_name;
			clientsChat.firstbyte='1';
			clientsChat.msglen=MAXMSG;
			clientsChat.recipient_id=name;
			
			if(pass_message(connect, command, &clientsChat)<0){
				printf("there was problem connecting to a chat client\n");
			}
			
			 
				while (fgets(sentMsg, MAXMSG, stdin) != NULL) {
	
					if (pass_message(connect, sentMsg, &clientsChat)<0) {
					    perror("sendto");
					    return;
					}
	
					if (n = read_message(connect, recvMsg, &clientsChat)<0) {
					    perror("recvfrom");
					    return;
					}
	
					recvMsg[n] = 0;        /* null terminate */
					if (fputs(recvMsg, stdout) == EOF) {
					    fprintf(stderr, "fputs error\n");
					    return;
					}
				}
			
			break;
		case 2:
		        //set message header fields appropriately
			clientsChat.sender_id=nick_name;
			clientsChat.firstbyte='2';
			clientsChat.msglen=MAXMSG;
			clientsChat.recipient_id=name;
			
			if(pass_message(connect, command, &clientsChat)<0){
				printf("there was problem joining a chat channel\n");
			}
			
			 
				while (fgets(sentMsg, MAXMSG, stdin) != NULL) {
	
					if (pass_message(connect, sentMsg, &clientsChat)<0) {
					    perror("sendto");
					    return;
					}
	
					if (n = read_message(connect, recvMsg, &clientsChat)<0) {
					    perror("recvfrom");
					    return;
					}
	
					recvMsg[n] = 0;        /* null terminate */
					if (fputs(recvMsg, stdout) == EOF) {
					    fprintf(stderr, "fputs error\n");
					    return;
					}
				}
			
			break;
		case 3:
		        //set message header fields appropriately
			clientsChat.sender_id=nick_name;
			clientsChat.firstbyte='3';
			clientsChat.msglen=0;
			clientsChat.recipient_id=0;
			//use to send the quit command to the server
			if(pass_message(connect, command, &clientsChat)<0){
				printf("there was problem quiting\n");
			}
				printf("Thanks for using our service\n We are always available for you.\n");
			//server replies with a disconnection message
		    //read_message(connect, command, clientsChat);
			break;
			
		default:	
			printf("That was an invalid input, try reconnecting :)\n");
		}
	}	
	
/*	memset(message, 0, sizeof(message));*/
/*	//while(1){*/
/*		fgets(message,sizeof(message),stdin);*/
/*		pass_message(connect, message, &clientsName);*/
/*		memset(message, 0, sizeof(message));*/
/*		read_message(connect, message, &clientsName);*/
/*	//}*/
return 0;
}

   

