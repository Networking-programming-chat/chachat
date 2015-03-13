#include <stdio.h>
#include "client_connect.h"
#include "networking/netproto.h"

int main(void) {
    char *hostAddr="nwprog1.netlab.hut.fi";
    char *hostPort="5000";
    
    int connect, nickSend;
	Msgheader clientsName;
	char message[MAXMSG];
    
    if ((connect=client_connect(host, service))<0) {
        perror("tcp connection error\n");
        return -1;
    }else{	
		printf("To start chatting type /nick your_nickname\n");	
	}
	
	memset(clientsName, 0, sizeof(clientsName));
	scanf("%s",clientsName->sender_id);
    if(nickSend = write(connect, clientsName->sender_id, sizeof(clientsName->sender_id) < 0){
	   printf("There is problem writing nickname to the server\n");	
	}else{
	    printf("###########################################################");
		printf("The following are standard rules:");
	    printf("To chat with a friend type /chat @receipient's_nickname\n");
		printf("To join a channel type /join %channel_name\n");
		printf("To quit type /quit\n");
        printf("###########################################################");
	}
	
	memset(message, 0, sizeof(message));
	//while(1){
		scanf("%s", message);
		pass_message(connect, message, clientsName);
		
		memset(message, 0, sizeof(message));
		read_message(connect,  message, clientsName);
		
	//}
	
    return 0;
}
