#include "client.h"


int get_timestamp(char buffer [20]){
    time_t rawtime;
    struct tm * timeinfo;
    
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (buffer,20,"%H:%M:%S",timeinfo);
    return 0;
    
}

int client_nick(int sockfd, char *nickname){
    
    char nick[MAX];
    ssize_t n1;
    int len, nickCnt=3;
    char *lf;
    //printf("To start chatting type your nickname below.\n");
    //set content to zero
    //memset(nickname, 0, sizeof(nickname));
    //give user 3 chances to give correct nickname otherwise this connect them
    while(nickCnt != 0){
        printf ("Enter your nickname: ");
        fgets(nick,sizeof(nick),stdin);
        len = strlen(nick);
        if ((lf=memchr(nick, '\n', MAX)) != NULL){
            *lf='\0';
        }
        
        //printf ("The nickname entered is %d characters long.\n",len);
        
        if(len < MAX_NICKLEN+1){
            // printf("what a nice nick %s  ",nick);
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
    if ((n1=write(sockfd, nick, strlen(nick)))<0) {
        printf("read client name error\n");
        return -1;
    }
    
    //printf ("Your nickname is %s\n",nickname);
    //existCnt++;
    // Empty the buffer used in storing the nickname.
    //bzero(nick,sizeof(nick));
    
    uint16_t existCnt=0;
    char nickExist;
    //stores the return value received after sending the nickname to the server.
    if ((n1=read(sockfd, &nickExist, sizeof(nickExist)))<0) {
        printf("read client name error\n");
        return -1;
    }
    
    
    
    while(nickExist=='2'){
        printf ("Nickname already in use, enter a new nickname: ");
        fgets(nick,sizeof(nick),stdin);
        if ((lf=memchr(nick, '\n', MAX)) != NULL){
            *lf='\0';
        }
        if ((n1=write(sockfd, nick, strlen(nick)))<0) {
            printf("send client name error\n");
            return -1;
        }
        
        if ((n1=read(sockfd, &nickExist, sizeof(nickExist)))<0) {
            printf("read client name error\n");
            return -1;
        }
        
        existCnt++;
        //break the loop after user sends an existing 3tims to the server.
        if(existCnt==2){
            //close();
            break;
        }
        
    }
	   if(nickExist=='1'){
           printf("Name successfully added\n");
       }
    sprintf(nickname,"%s",nick);
    
    return len;
}

void printChatRule(){
    
    printf("###########################################################\n");
    printf("%sThe following are standard rules:\n",COLOR_NRM);
    printf("%sTo start chatting type your nickname below.\n",COLOR_RED );
    printf("%sTo chat with a friend type /chat @recipient\n",COLOR_GRN);
    printf("%sTo join a channel type /join #channel\n",COLOR_YEL);
    printf("%sTo exit type /exit\n",COLOR_BLU);
    printf("%sTo quit type /quit\n",COLOR_RED);
    printf("%sTo continue chat with a friend after joining a channel,\nsimply issue the chat command again\n",COLOR_BLU);
    printf("%sTo continue a channel discussion after starting a chat with a friend,\nsimply issue the join command again\n",COLOR_CYN);
    printf("%sAnd you can only make mistakes 3 times.\n",COLOR_RED);
    printf("%sIf you have any problem, simply type: help\n",COLOR_WHT);
    printf("%sDo have fun! using our service. :)\n",COLOR_YEL);
    printf("%sAnd chachat channel is for internal use only\n",COLOR_RED);
    printf("%s###########################################################\n",COLOR_RESET);
    
}

//Handling client's private chat command and message
int chatMessageHandle(int connfd, char *send, Msgheader chatheader){
    
    int n=1;
    
    //always send the string length of the sent msg
    chatheader.msglen=strlen(send);
    
    //sends source client's message
    if ((n=pass_message(connfd, send, &chatheader))<0) {
        perror("pass_message:");
        return -1;
    }
    
    
    /* send[n] = 0;        // null terminate
     if (fputs(send, stdout) == EOF) {
     fprintf(stderr, "fputs error\n");
     return -1;
     } */
    
    return 0;
}

//Handling client's channel join command message
int chanMessageHandle(int connfd, char *send, Msgheader chatheader){
    
    int n=1;
    
    //always send the string length of the sent msg
    chatheader.msglen=strlen(send);
    
    //sends source client's message
    if ((n=pass_message(connfd, send, &chatheader))<0) {
        perror("pass_message:");
        return -1;
    }
    
    send[n] = 0;        // null terminate
    if (fputs(send, stdout) == EOF) {
        fprintf(stderr, "fputs error\n");
        return -1;
    }
    
    return 0;
}

int send_message(int connect,char *nickname){
    
    int inpCnt=2;
    int cht=0, chn=0, mstk=0, chnCnt=0, chtCnt=0;
    char cmd[6], name[MAX_NICKLEN], nameR[MAX_NICKLEN], sender[MAX_NICKLEN],c1, c2, buf[20];
    char *search, sentMsg[MAXMSG];
    Msgheader clientsChat;

    {
        
        //Header initialization
        clientsChat.firstbyte='0';
        clientsChat.msglen=0;
        clientsChat.recipient_id="0";
        clientsChat.sender_id=nickname;
        memset(sentMsg, 0, sizeof(sentMsg));
        
        
        while(clientsChat.firstbyte != '4' || mstk != 3){
            
            
        CHATTING:
            get_timestamp(buf);
            printf("x%sx %s>%s:< ",buf,COLOR_CYN,sender);
            fgets(sentMsg,sizeof(sentMsg),stdin);
            
            search = strchr(sentMsg,'/');
            
            if(search == NULL && cht==1 && clientsChat.firstbyte=='1' && chtCnt!=0){
                clientsChat.msglen=strlen(sentMsg);
                clientsChat.recipient_id=nameR;
                clientsChat.sender_id=nickname;
                //printf("User: %s , opt=%c\n", sentMsg, clientsChat.firstbyte);
                //printf("chat=%d and chan=%d\n", chtCnt, chnCnt);
               
                chatMessageHandle(connect, sentMsg, clientsChat); //chat messenger function
                memset(sentMsg, 0, sizeof(sentMsg)); //empty message buffer
                continue;
                
            }else if(search == NULL && chn==1 && clientsChat.firstbyte=='2' && chnCnt!=0){
                clientsChat.msglen=strlen(sentMsg);
                clientsChat.recipient_id=nameR;
                clientsChat.sender_id=nickname;
                //printf("User: %s , opt=%c\n", sentMsg, clientsChat.firstbyte);
                //printf("chat=%d and chan=%d\n", chtCnt, chnCnt);
               
                chanMessageHandle(connect, sentMsg, clientsChat); //chan messenger function
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
                        clientsChat.sender_id=nickname;
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
                    clientsChat.sender_id=nickname;
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
                                clientsChat.sender_id=nickname;
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
                                clientsChat.sender_id=nickname;
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

