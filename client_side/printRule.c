#include "client.h"



int printChatRule( char *getCmd, char *getName){

	printf("###########################################################\n");
	printf("The following are standard rules:\n");
	printf("To chat with a friend type /chat @receipient's_nickname\n");
	printf("To join a channel type /join #channel_name\n");
	printf("To quit type /quit\n");
	printf("###########################################################\n");
	
	//string formating going on
		char command[30], cmd[6], name[21], c1, c2;
		memset(command, 0, sizeof(command));
		
		//function to get command from users
		fgets(command,sizeof(command),stdin);
		//detach the (/) xter and the (@) xter 
		sscanf(command, " %c %s %c %s", &c1, cmd, &c2, name);
		//set content of command to zero and write to it again
		memset(command, 0, sizeof(command));
		sprintf(command, "%s", cmd);
		
		int cas=0;
		if(strcmp(cmd,"chat")==0){
			cas=1;
			printf(" You would like to chat with %s ? \n", name);
		}else if(strcmp(cmd,"join")==0){
		    cas=2;
		    printf(" You would like to join %s group chat ? \n", name);
		}else if(strcmp(cmd,"quit")==0){
		    cas=3;
		    printf(" You would like to quit chating ? \n");
		}
		
		//copy command and name into getCmd and getName respectively
		sprintf(getCmd, "%s", cmd);
		sprintf(getName, "%s", name);
		
 return cas;
}