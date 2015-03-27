#include "client.h"
#include "../networking/netproto.h"



int client(const char *servName, const char *servPort) {

char nick_name[MAX];
int connect, nickSend, cas;
Msgheader clientsChat;
char sentMsg[MAXMSG], recvMsg[MAXMSG], command[10], name[MAX_NICKLEN];

	if ((connect=client_connect(servName, servPort))<0) {
		perror("tcp connection error\n");
	return -1;
	}
	
	
	//set content to zero
	memset(&clientsChat, 0, sizeof(clientsChat));
    //check if sending nick to the server was successful
	if((nickSend = client_nick(connect, nick_name)) < 0){
		printf("There is problem writing nickname to the server\n");
	}else{
	    
	    clientsChat.firstbyte='0';
		clientsChat.msglen=0;
		clientsChat.recipient_id="0";
		clientsChat.sender_id=nick_name;
		memset(sentMsg, 0, sizeof(sentMsg));
		
		//prints the chat rules and handles the switch condition
	    cas=startChat(command, name);
		
		switch(cas){
		
		case 1:
		    //set message header fields appropriately
			clientsChat.firstbyte='1';
			clientsChat.msglen=strlen(command);
			clientsChat.sender_id=nick_name;
			clientsChat.recipient_id=name;
			//private chat function called
			chatMessageHandle(connect, sentMsg, recvMsg, command, clientsChat);

			break;
		case 2:
		    //set message header fields appropriately
			clientsChat.firstbyte='2';
			clientsChat.msglen=strlen(command);
			clientsChat.sender_id=nick_name;
			clientsChat.recipient_id=name;
			//channel chat function called
			chanMessageHandle(connect, sentMsg, recvMsg, command, clientsChat);
			
			break;
		case 3:
		    //set message header fields appropriately
			clientsChat.firstbyte='3';
			clientsChat.msglen=0;
			clientsChat.sender_id=nick_name;
			clientsChat.recipient_id="0";
			//function to quit chatting called
			quitMessageHandle(connect, command, clientsChat);
			
			break;
			
		default:	
			printf(" The user doesn't exist, try again later. Thank you :)\n");
			return -1;
		}
	}	
	

return 0;
}

   
