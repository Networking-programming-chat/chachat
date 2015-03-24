#include "client.h"



int startChat( char *getCmd, char *getName){

	printf("###########################################################\n");
	printf("The following are standard rules:\n");
	printf("To chat with a friend type /chat @receipient's_nickname\n");
	printf("To join a channel type /join #channel_name\n");
	printf("To quit type /quit\n");
	printf("For help type ?help or simply type ???\n");
	printf("###########################################################\n");
	
	//string formating going on
		char command[30], cmd[6], name[21], c1, c2;
		int inpCnt=2, cas=0;
		
		memset(command, 0, sizeof(command));

		
		 while(inpCnt != 0){
			//function to get command from users
			fgets(command,sizeof(command),stdin);
			
			if(strlen(command) <= 25){
				
				//detach the (/) xter and the (@) xter 
				sscanf(command, " %c %s %c %s", &c1, cmd, &c2, name);
				//set content of command to zero and write to it again
				//memset(command, 0, sizeof(command));
				//sprintf(command, "%s", cmd);
				
				 
				if(strcmp(cmd,"chat")==0){
					cas=1;
					printf(" You would like to chat with %s ? \n", name);
					inpCnt = 0;
				}else if(strcmp(cmd,"join")==0){
					cas=2;
					printf(" You would like to join %s group chat ? \n", name);
					inpCnt = 0;
				}else if(strcmp(cmd,"quit")==0){
					cas=3;
					printf(" You would like to quit chating ? \n");
					inpCnt = 0;
				}else if((cas!=1 || cas!=2 || cas!=3 || strlen(cmd)!=4 )){
					printf("Invalid command!\n");
					printChatRule();
					memset(command, 0, sizeof(command));
					memset(cmd, 0, sizeof(cmd));
					inpCnt--;
					if(inpCnt==0){
						printf("The command is invalid!");
					}
				}
				
				
			
			}else{
				printf("The name is too long and doesn't exist\n");
				printChatRule();
				memset(command, 0, sizeof(command));
				memset(cmd, 0, sizeof(cmd));
				memset(name, 0, sizeof(name));
				return -1;
			}
		 }
		
		
		
		
		//copy command and name into getCmd and getName respectively
		sprintf(getCmd, "%s", cmd);
		sprintf(getName, "%s", name);
		
 return cas;
}

void printChatRule(){
	
	printf("###########################################################\n");
	printf("The following are standard rules:\n");
	printf("To chat with a friend type /chat @receipient's_nickname\n");
	printf("To join a channel type /join #channel_name\n");
	printf("To quit type /quit\n");
	printf("###########################################################\n");
	
}

