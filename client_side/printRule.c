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
		int inpCnt=2, cas=0, ret, cmdlen, namlen;
		
		memset(command, 0, sizeof(command));

		
		 while(inpCnt != 0){
			//function to get command from users
			fgets(command,sizeof(command),stdin);
			cmdlen=strlen(command);
			
/*			if(cmdlen > 30){*/
/*				command[30]=NULL;*/
/*				cmdlen=30;*/
/*			}*/
			
			if(cmdlen <= 25){
				
				//detach the (/) xter and the (@) xter 
				sscanf(command, " %c %s %c %s", &c1, cmd, &c2, name);
				namlen=strlen(name);
				
				
				if((ret=strcmp(cmd,"chat"))==0){
					
					printf("Recipient's name is missing\n");
					printChatRule();
					inpCnt--;
					
					if((ret=strcmp(cmd,"chat"))==0 && namlen != 0){
						cas=1;
						printf(" You would like to chat with %s ? \n", name);
						inpCnt = 0;
					}
					
				}
				
				 
				if((ret=strcmp(cmd,"join"))==0){
				
					printf("channel's name is missing\n");
					printChatRule();
					inpCnt--;
					
					if((ret=strcmp(cmd,"join"))==0 && namlen != 0){
						cas=2;
						printf(" You would like to join %s's channel ? \n", name);
						inpCnt = 0;
					}
					
				}
				
				
			    if((ret=strcmp(cmd,"quit"))==0){
					cas=3;
					printf(" You would like to quit chating ? \n");
					inpCnt = 0;
					
				}
				
				/*if(ret != 0){
					printf("%s is not recognized\n", cmd);
					printChatRule();
					inpCnt--;
				}*/
				
				
			}else if(ret != 0){
					printf("%s is not recognized\n", cmd);
					printChatRule();
					inpCnt--;
			
			
			}else{
				
				printChatRule();
				inpCnt--;
			}
		 }
		
		
		
		
		//copy command and name into getCmd and getName respectively
		sprintf(getCmd, "%s", cmd);
		sprintf(getName, "%s", name);
		
 return cas;
}

void printChatRule(){
	
	//printf("%s is not recognized\n",wrongCmd);
	
	printf("###########################################################\n");
	printf("The following are standard rules:\n");
	printf("To chat with a friend type /chat @receipient's_nickname\n");
	printf("To join a channel type /join #channel_name\n");
	printf("To quit type /quit\n");
	printf("###########################################################\n");
	
}
