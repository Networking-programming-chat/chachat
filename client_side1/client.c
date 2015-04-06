#include "client.h"
#include "../networking/netproto.h"


#define MAXCHAT 3
#define MAXCHAN 3

int client(const char *servName, const char *servPort) {


int i, connect, nickSend, inpCnt=2;
int cht=0, chn=0, mstk=0, chnCnt=0, chtCnt=0;
char cmd[6], name[MAX_NICKLEN], c1;
char *search, *namSaved[MAXCHAN], sentMsg[MAXMSG], recvMsg[MAXMSG], *nick_name;
Msgheader clientsChat;

	if ((connect=client_connect(servName, servPort))<0) {
		perror("tcp connection error\n");
	return -1;
	}
	
	
	//set content to zero
	memset(&clientsChat, 0, sizeof(clientsChat));
	
	printChatRule();
    //check if sending nick to the server was successful
    nick_name = client_nick(connect);
	if(strlen(nick_name) < 0){
		printf("There is problem writing nickname to the server\n");
	}else{
	    
	    //Header initialization
	    clientsChat.firstbyte='0';
		clientsChat.msglen=0;
		clientsChat.recipient_id="0";
		clientsChat.sender_id=nick_name;
		memset(sentMsg, 0, sizeof(sentMsg));
		
		
		
	while(clientsChat.firstbyte != '4' || mstk != 3){	
		
		
		CHATTING:
		printf("%s:> ",nick_name);
		fgets(sentMsg,sizeof(sentMsg),stdin);
		
		search = strchr(sentMsg,'/');
		
	    if(search == NULL && cht==1 && clientsChat.firstbyte=='1' && chtCnt!= 0){
			clientsChat.msglen=strlen(sentMsg);
			clientsChat.recipient_id=name;
			clientsChat.sender_id=nick_name;
			printf("User: %s , opt=%c\n", sentMsg, clientsChat.firstbyte);
			printf("chat=%d and chan=%d\n", chtCnt, chnCnt);
		
			chatMessageHandle(connect, sentMsg, recvMsg, clientsChat); //chat messenger function
			memset(sentMsg, 0, sizeof(sentMsg)); //empty message buffer
			continue;	
			
		}else if(search == NULL && chn==1 && clientsChat.firstbyte=='2' && chnCnt!=0){
			clientsChat.msglen=strlen(sentMsg);
			clientsChat.recipient_id=name;
			clientsChat.sender_id=nick_name;
			printf("User: %s , opt=%d\n", sentMsg, clientsChat.firstbyte);
			printf("chat=%d and chan=%d\n", chtCnt, chnCnt);
			
			chanMessageHandle(connect, sentMsg, recvMsg, clientsChat); //chat messenger function
			memset(sentMsg, 0, sizeof(sentMsg)); //empty message buffer
			continue;
		 
		}else if((search = strstr(sentMsg,"exit")) != NULL){
				sscanf(sentMsg, " %c %s", &c1, cmd);

				if(strcmp(cmd,"exit")!=0){
					printf("Invalid command!\n");
					inpCnt = 0;
					mstk+=1;
				}else{
					clientsChat.firstbyte=clientsChat.firstbyte;
					if (clientsChat.firstbyte=='1' && chtCnt>1){
						printf("You've stopped a private chat.\n");
						chtCnt-=1;
		
						//to support multple private sessions at a time
						if(chtCnt==0){
							printf("You've stopped all private chat.\n");
							cht=0;
						}
						inpCnt = 0;
						
						//to continue group chat
						goto CHATTING;
					}else if (clientsChat.firstbyte=='2' && chnCnt>1){
						printf("You've stopped a channel discussion.\n");
						chnCnt-=1;
						
						//to support multple channels at a time
						if(chnCnt==0){
							printf("You've stopped all channel discussion.\n");
							chn=0;
						}
						inpCnt = 0;
						
						//to continue private chat
						goto CHATTING;
					}else if ((clientsChat.firstbyte=='0' && chtCnt==0) || (clientsChat.firstbyte=='0' && chnCnt==0)){
						printf("You leave without chatting. \n");
						cht=0;
						inpCnt = 0;
						return -1;
					}
				}
			}else if((search = strstr(sentMsg,"help")) != NULL){
				sscanf(sentMsg, " %c %s ", &c1, cmd);
				if(strcmp(cmd,"help")!=0){
					printf("Invalid command!\n");
					inpCnt = 0;
					mstk+=1;
				}else{
					clientsChat.firstbyte='3';
					printChatRule();
					continue;	
				}
			
			}else if((search = strstr(sentMsg,"quit")) != NULL){
				sscanf(sentMsg, " %c %s ", &c1, cmd);
				if(strcmp(cmd,"quit")!=0){
					printf("Invalid command!\n");
					inpCnt = 0;
					mstk+=1;
				}else{
					clientsChat.firstbyte='4';
					mstk=3;
					continue;	
				}
			
		}else{
			inpCnt=2;
			while((inpCnt != 0 && chtCnt != MAXCHAT) || (inpCnt != 0 && chnCnt != MAXCHAN)){	
			
				if(strlen(sentMsg) <= 25){
								 
						if((search = strstr(sentMsg,"chat")) != NULL){
							sscanf(sentMsg, " %c %s %s", &c1, cmd, name);
							if(strcmp(cmd,"chat")==0 && strchr(name,'@')!=NULL){
								clientsChat.firstbyte='1';
								clientsChat.msglen=0;
								clientsChat.recipient_id=name;
								clientsChat.sender_id=nick_name;
								cht=1;
								inpCnt = 0;
								chtCnt +=1;
								memset(sentMsg, 0, sizeof(sentMsg));
/*								for(i=0; i<MAXCHAT; i++){*/
/*								    namSaved[i]=name;*/
/*								    if(name==namSaved[i]){*/
/*								       chtCnt--;*/
/*								    }*/
/*								}*/
								
								
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
						}else if((search = strstr(sentMsg,"join")) != NULL){
							sscanf(sentMsg, " %c %s %s", &c1, cmd, name);
							if(strcmp(cmd,"join")==0 && strchr(name,'#')!=NULL){
								clientsChat.firstbyte='2';
								clientsChat.msglen=0;
								clientsChat.recipient_id=name;
								clientsChat.sender_id=nick_name;
								chn=1;
								inpCnt = 0;
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
						
						}else if((clientsChat.firstbyte!='1' && cht!=1) || (clientsChat.firstbyte!='2' && chn!=1) || (clientsChat.firstbyte!='3') || (clientsChat.firstbyte!='4') || (strlen(cmd)!=4)){
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

	}	
	

return 0;
}

   
