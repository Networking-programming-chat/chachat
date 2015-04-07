#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAXCHAT 5
#define MAXCHAN 5
#define MAX 50


int main(){


char*search, sentMsg[MAX];
int option=0, cht=0, chn=0, mstk=0, chnCnt=0, chtCnt=0;
char command[30], cmd[6], name[2], c1, c2;
int inpCnt=2, cas=0;
char chat[]="/chat";
char join[]="/join";
char quit[]="/quit";
char exit[]="/exit";
//prints the chat rules and handles the switch condition
	   
		
	printChatRule();
	
		
	while(option != 4 || mstk != 3){	
		CHATTING:
		printf("Me:> ");
		fgets(sentMsg,sizeof(sentMsg),stdin);
		
		search = strchr(sentMsg,'/');
		
	    if(search == NULL && cht==1 && option==1 && chtCnt!= 0){
			//option =option;
			//printf("cas=%d",cas);
			//sprintf(msgBeffer, "%s", msg);
			printf("User: %s , opt=%d\n", sentMsg, option);
			printf("chat=%d and chan=%d\n", chtCnt, chnCnt);
			memset(sentMsg, 0, sizeof(sentMsg));
			continue;	
		}else if(search == NULL && chn==1 && option==2 && chnCnt!=0){
		
			printf("User: %s , opt=%d\n", sentMsg, option);
			printf("chat=%d and chan=%d\n", chtCnt, chnCnt);
			memset(sentMsg, 0, sizeof(sentMsg));
			continue;
		 
		}else if((search = strstr(sentMsg,exit)) != NULL){
				sscanf(sentMsg, " %c %s", &c1, cmd);
/*				printf("c1 = %c", c1);*/
/*				printf("cmd = %s", cmd);*/
				if(strcmp(cmd,"exit")!=0){
					printf("Invalid command!\n");
					inpCnt = 0;
					mstk+=1;
				}else{
					option=option;
					if (option==1 && chtCnt>1){
						printf("You've stopped a private chat.\n");
						chtCnt-=1;
						//cht=0;
						//to support multple private sessions at a time
						if(chtCnt==0){
							printf("You've stopped all private chat.\n");
							cht=0;
						}
						inpCnt = 0;
						//option=2; //to continue group chat
						goto CHATTING;
					}else if (option==2 && chnCnt>1){
						printf("You've stopped a channel discussion.\n");
						chnCnt-=1;
						//chn=0;
						//to support multple channels at a time
						if(chnCnt==0){
							printf("You've stopped all channel discussion.\n");
							chn=0;
						}
						inpCnt = 0;
						//option=1; //to continue private chat
						goto CHATTING;
					}else if (option==0 && chtCnt==0 || option==0 && chnCnt==0){
						printf(" You leave without chatting. \n");
						cht=0;
						inpCnt = 0;
						return;
					}
				}

			}else if((search = strstr(sentMsg,quit)) != NULL){
				sscanf(sentMsg, " %c %s ", &c1, cmd);
				if(strcmp(cmd,"quit")!=0){
					printf("Invalid command!\n");
					inpCnt = 0;
					mstk+=1;
				}else{
					option=4;
					mstk=3;
					continue;	
				}
			
		}else{
			inpCnt=2;
			while(inpCnt != 0 && chtCnt != MAXCHAT || inpCnt != 0 && chnCnt != MAXCHAN){	
			
				if(strlen(sentMsg) <= 25){
								 
						if((search = strstr(sentMsg,chat)) != NULL){
							sscanf(sentMsg, " %c %s %s", &c1, cmd, name);
							if(strcmp(cmd,"chat")==0 && strchr(name,'@')!=NULL){
								option=1;
								cht=1;
								inpCnt = 0;
								//printf("You have a private chat  ON :)\n");
								chtCnt +=1;
								memset(sentMsg, 0, sizeof(sentMsg));
								//to support multple private sessions at a time
							    if(chtCnt>MAXCHAT){
									printf("You have %d private chats  ON already\n",MAXCHAT);
									printf("and that is the max. You can't chat more friends\n");
									printf("until you exit at least one. :)\n");
									
									chtCnt -=1;
									goto CHATTING;
								}else{
									printf("You have %d private chats  ON :)\n",chtCnt);
									
								}
								
								if(chnCnt==MAXCHAN){
									printf("You can't join more chats at this time.\n");
									printf("Server overloaded. You should exit a channel.\n");
									goto CHATTING;
								}
								
								continue;
							}else{
								printf("You entered a wrong character before the name\n");
								mstk +=1;
								inpCnt=0;
								if(mstk==3){
									printf("Do you have problems with your sight?\n");
									return -1;
								}
							
							}
						}else if((search = strstr(sentMsg,join)) != NULL){
							sscanf(sentMsg, " %c %s %s", &c1, cmd, name);
							if(strcmp(cmd,"join")==0 && strchr(name,'#')!=NULL){
								option=2;
								chn=1;
								inpCnt = 0;
								//printf("You have joined a channel\n");
								chnCnt +=1;
								memset(sentMsg, 0, sizeof(sentMsg));
								//to support multple channels at a time
								if(chnCnt>MAXCHAN){
									printf("You have joined %d channels already\n",MAXCHAN);
									printf("and that is the max. You can't join more channels\n");
									printf("until you exit at least one. :)\n");
									
									chnCnt -=1;
									goto CHATTING;
								}else{
									printf("You have joined %d channel\n",chnCnt);
									
								}
								if(chtCnt==MAXCHAN){
									printf("You can't join more channels at this time.\n");
									printf("Server overloaded. You should exit a chat.\n");
									goto CHATTING;
								}
								
								continue;
							}else{
								printf("You entered a wrong character before the name\n");
								mstk +=1;
								inpCnt=0;
								if(mstk==3){
									printf("Do you have problems with your sight?\n");
									return -1;
								}
							}
						
						}else if((option!=1 && cht!=1 || option!=2 && chn!=1 || option!=3 || option!=4 || strlen(cmd)!=4)){
								printf("Invalid command!\n");
								memset(sentMsg, 0, sizeof(sentMsg));
								memset(cmd, 0, sizeof(cmd));
								mstk +=1;
								inpCnt=0;
								if(mstk==3){
									printf("Do you have problems with your sight?\n");
									return -1;
								}
							
						}
					
					
					}else{
							printf("The name is too long and doesn't exist\n");
							printChatRule();
							mstk +=1;
							inpCnt=0;
							if(mstk==3){
									printf("Do you have problems with your sight?\n");
									return -1;
								}
					}	
				}	
		}
		
		
	}

return 0;
}


