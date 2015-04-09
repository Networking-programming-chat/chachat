#include "client.h"
#include "../networking/netproto.h"


#define MAXCHAT 3
#define MAXCHAN 3

int client(const char *servName, const char *servPort) {


int connect, nickSend, inpCnt=2;
int cht=0, chn=0, mstk=0, chnCnt=0, chtCnt=0;
char cmd[6], name[MAX_NICKLEN], nameR[MAX_NICKLEN], sender[MAX_NICKLEN],c1, c2, *tStamp, buf[20];
char *search, sentMsg[MAXMSG], recvMsg[MAXMSG], getnick[MAX_NICKLEN];
Msgheader clientsChat;

	if ((connect=client_connect(servName, servPort))<0) {
		perror("tcp connection error\n");
	return -1;
	}
	
	
	//set content to zero
	memset(&clientsChat, 0, sizeof(clientsChat));
	
	printChatRule();
    //check if sending nick to the server was successful
    nickSend = client_nick(connect, getnick);
    strcpy(sender,getnick);
    //printf("your nick is %s ",getnick);
	if(strlen(nickSend) < 0){
		printf("There is problem writing nickname to the server\n");
	}else{
	    
	    //Header initialization
	    clientsChat.firstbyte='0';
		clientsChat.msglen=0;
		clientsChat.recipient_id="0";
		clientsChat.sender_id=getnick;
		memset(sentMsg, 0, sizeof(sentMsg));
		
		
	while(clientsChat.firstbyte != '4' || mstk != 3){	
		
		
		CHATTING:
		tStamp=get_timestamp(buf);	
		printf("x%sx %s>%s:< ",tStamp,COLOR_CYN,sender);
		//printf("x%sx %s>%s:< ",tStamp,COLOR_CYN,sender);
		fgets(sentMsg,sizeof(sentMsg),stdin);
		
		search = strchr(sentMsg,'/');
		
	    if(search == NULL && cht==1 && clientsChat.firstbyte=='1' && chtCnt!=0){
			clientsChat.msglen=strlen(sentMsg);
			clientsChat.recipient_id=nameR;
			clientsChat.sender_id=getnick;
			//printf("User: %s , opt=%c\n", sentMsg, clientsChat.firstbyte);
			//printf("chat=%d and chan=%d\n", chtCnt, chnCnt);
		
			chatMessageHandle(connect, sentMsg, recvMsg, clientsChat); //chat messenger function
			memset(sentMsg, 0, sizeof(sentMsg)); //empty message buffer
			continue;	
			
		}else if(search == NULL && chn==1 && clientsChat.firstbyte=='2' && chnCnt!=0){
			clientsChat.msglen=strlen(sentMsg);
			clientsChat.recipient_id=nameR;
			clientsChat.sender_id=getnick;
			//printf("User: %s , opt=%c\n", sentMsg, clientsChat.firstbyte);
			//printf("chat=%d and chan=%d\n", chtCnt, chnCnt);
			
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
					if(clientsChat.firstbyte=='2' && chnCnt>=1){
						printf("You've stopped a channel discussion.\n");
						chnCnt-=1;
						clientsChat.firstbyte='3';
						clientsChat.msglen=0;
						clientsChat.recipient_id=NULL;
						clientsChat.sender_id=getnick;
						pass_message(connect, sentMsg, &clientsChat);
						
					//to support multple channels at a time
					if(chnCnt==0){
						printf("You've stopped all channel discussion.\n");
						chn=0;
					}
						inpCnt = 0;
						
						//to continue private chat
						goto CHATTING;
					}else if(clientsChat.firstbyte=='0' && chnCnt==0){
						printf("You left without chatting or joining a channel.\n");
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
					clientsChat.firstbyte='5';
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
					clientsChat.msglen=0;
					clientsChat.recipient_id=NULL;
					clientsChat.sender_id=getnick;
					pass_message(connect, sentMsg, &clientsChat);
					mstk=3;
					return-1;	
				}
			
		}else{
			inpCnt=2;
			while((inpCnt != 0 && chtCnt != MAXCHAT) || (inpCnt != 0 && chnCnt != MAXCHAN)){	
			
				if(strlen(sentMsg) <= 25){
								 
						if((search = strstr(sentMsg,"chat")) != NULL){
							sscanf(sentMsg, " %c %s %s", &c1, cmd, name);
							if(strcmp(cmd,"chat")==0 && strchr(name,'@')!=NULL){
								sscanf(name,"%c %s", &c2, nameR);
								clientsChat.firstbyte='1';
								clientsChat.msglen=0;
								clientsChat.recipient_id=nameR;
								clientsChat.sender_id=getnick;
								cht=1;
								inpCnt = 0;
								chtCnt +=1;
								memset(sentMsg, 0, sizeof(sentMsg));
								pass_message(connect, sentMsg, &clientsChat);
								//printf("You have %d private chats  ON :)\n",chtCnt);
															
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
								sscanf(name,"%c %s", &c2, nameR);
								clientsChat.firstbyte='2';
								clientsChat.msglen=0;
								clientsChat.recipient_id=nameR;
								clientsChat.sender_id=getnick;
								chn=1;
								inpCnt = 0;
								chnCnt +=1;
								memset(sentMsg, 0, sizeof(sentMsg));
								pass_message(connect, sentMsg, &clientsChat);
								//printf("You have joined %d channel\n",chnCnt);
									
								
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
						
						}else if((clientsChat.firstbyte!='1' && cht!=1) || (clientsChat.firstbyte!='2' && chn!=1) || (clientsChat.firstbyte!='3') || (clientsChat.firstbyte!='4') || (clientsChat.firstbyte!='5') || (strlen(cmd)!=4)){
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

   
