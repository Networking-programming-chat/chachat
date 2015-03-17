#include <stdio.h>
#include "client.h"
#include "netproto.h"


int main(void) {
	//indents make code easier to read;
	char *hostAddr="localhost";		//tested with localhost
	char *hostPort="3333", nickName[20];
	int connect, nickSend;
	Msgheader clientsName;
	char message[MAXMSG];
	
if ((connect=client(hostAddr, hostPort))<0) {
perror("tcp connection error\n");
return -1;
}else{
printf("To start chatting type /nick your_nickname\n");
}

//sizeof(nickName) is the size of 64bit memory pointer, so might break stuff.
fgets(nickName,sizeof(nickName),stdin);

memset(&clientsName, 0, sizeof(clientsName));
clientsName.sender_id=nickName;
if(nickSend = write(connect, clientsName.sender_id, sizeof(clientsName.sender_id)) < 0){
printf("There is problem writing nickname to the server\n");
}else{
printf("###########################################################\n");
printf("The following are standard rules:\n");
printf("To chat with a friend type /chat @receipient's_nickname\n");
printf("To join a channel type /join #channel_name\n");
printf("To quit type /quit\n");
printf("###########################################################\n");
}

//sizeof(message) is the size of 64bit memory pointer, so might break stuff.
memset(message, 0, sizeof(message));
//while(1){
fgets(message,sizeof(message),stdin);
pass_message(connect, message, &clientsName);
memset(message, 0, sizeof(message));
read_message(connect, message, &clientsName);
//}
return 0;
}

   

