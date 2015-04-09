#include <stdio.h>
#include <sys/socket.h> //socket.
#include <strings.h>
#include <stdlib.h> // malloc
#include <string.h>  // memset (do not use bzero!)
#include <netdb.h> // addrinfo
#include <unistd.h> //close
#include <arpa/inet.h> //inet_ntop
#include <string.h>

#include "serv.h"



/*Opening an listened fd.
 */
 
 
int serv_listen(const char *host, const char *serv, socklen_t *addrlenp){
    int listenfd, n;
    const int on = 1;
    struct addrinfo hints, *res, *ressave;
    memset(&hints,0, sizeof(struct addrinfo));
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0) {
        fprintf(stderr, "tcp_listen error for %s, %s: %s",
                host, serv, gai_strerror(n));
        return -1;
    }
    ressave = res;
    
    do {
        listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (listenfd < 0)
            continue;               /* error, try next one */
        
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
            break;                  /* success */
        
        close(listenfd);        /* bind error, close and try next one */
    } while ( (res = res->ai_next) != NULL);
    
    if (res == NULL) {        /* errno from final socket() or bind() */
        fprintf(stderr, "tcp_listen error for %s, %s", host, serv);
        return -1;
    }
    
    
    if (listen(listenfd, LISTENQ) < 0) {
        perror("listen");
        return -1;
    }
	if (addrlenp)
		*addrlenp = res->ai_addrlen;    /* return size of protocol address */
    
    printf("The ip address we are using is: ");
    print_address(res);
    
    freeaddrinfo(ressave);
    
    return(listenfd);
}

/*print the address that server use*/
void print_address(const struct addrinfo *res)
{
    char outbuf[80];
    struct sockaddr_in *sin = (struct sockaddr_in *)res->ai_addr;
    struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)res->ai_addr;
    void *address;
    
    if (res->ai_family == AF_INET)
        address = &(sin->sin_addr);
    else if (res->ai_family == AF_INET6)
        address = &(sin6->sin6_addr);
    else {
        printf("Unknown address\n");
        return;
    }
    
    const char *ret = inet_ntop(res->ai_family, address,
                                outbuf, sizeof(outbuf));
    printf("%s\n",ret);
}

/*read the nickname from the client*/
int client_nick(int socket,char *nick,cc_user *user1){
    
    char nickname[MAX_NICKLEN];
    char response;
    int i=0,flag;
    cc_user *user;
    ssize_t n1;
    
    do {
        flag=1;
        memset(nickname, 0, sizeof(nickname));
        if ((n1=read(socket,nickname,sizeof(nickname)-1))<0) {
            perror("read client name error\n");
            return -1;
        }
        
       // printf("sender's_id is %s\n", nickname);
        
        user= add_user_server(nickname,0);
        
        if (user == NULL) {
            printf("Nickname already in use.\n");
            response='2';
            
            if ((n1=write(socket,&response,sizeof(response)))<0) {
                perror("write nickname response to client fail\n");
                return -1;
            }
            
            flag=0;
        }
        
    }while (flag==0&&(++i)<3);
    
    if (i==3) {
        return -1;
    }
    if (flag==1) {
        
        response='1';
        if ((n1=write(socket,&response,sizeof(response)))<0) {
            perror("write nickname response to client fail\n");
            return -1;
        }
        printf("write to client response success\n");
        
    }
    
    memcpy(user1, user, sizeof(cc_user));
    free(user);
    return 0;
}


void hexprinter(char* str,int num){
    for (int i = 0;i<num;i++){
        printf("%02X", str[i]);
    }
    printf("\n");
}


/*Handling client's private message*/
void chatMessageHandle(int connfd, char *mesbuff, Msgheader *mesheader){
    
    if (mesbuff==NULL) {
        return;
    }
    char response[50];
    Msgheader *respheader;
    char *message;
    ssize_t n1;
    //int blocklen;
   
    //blocklen = mesheader->msglen + 43;
    message = malloc((MAXMSG+HDRSIZE)*sizeof(char));
    memset(message, 0, MAXMSG+HDRSIZE);
    //merge the header and the message body
    //print_hdr(mesheader);
    serialize_everything(message, mesbuff,mesheader);
    hexprinter(message, 43);
    
    printf("message body2: %s\n",mesbuff);
    
    buffer_to_hdr(message, mesheader);
   // print_hdr(mesheader);
    
    
    //print_hdr(mesheader);
    
    respheader = malloc(sizeof(Msgheader));
    
    printf("client send private chat message\n");
    
    cc_user *destuser;
    destuser=get_user_by_nick(mesheader->recipient_id);
    
    if (destuser==NULL) {
        sprintf(response,"%s doesn't exit!\n",mesheader->recipient_id);
        
        respheader->firstbyte = '1';
        respheader->msglen = strlen(response);
        respheader->recipient_id = mesheader->sender_id;
        respheader->sender_id = mesheader->recipient_id;
    	print_hdr(respheader);
        
        if((n1=pass_message(connfd,response,respheader))<0){
            perror("write response to client fail\n");
        }
        
        
		return;
    }
    
    else if(destuser!=NULL){
        printf("dest user:\n");
        print_user(destuser);
    }
	write_to_buffer(destuser->user_id, message, MAXMSG+HDRSIZE);
    
    free(message);
    free(respheader);
}

void client_to_channel(Msgheader * mesheader){

    
    if (get_channel_by_name(mesheader->recipient_id)==NULL) {
        add_channel(mesheader->recipient_id);
        
    }
    
    cc_user * chanuser=get_users_by_channel_name(mesheader->recipient_id);
    
    while (chanuser!=NULL) {
        if (strncmp(chanuser->nick,mesheader->sender_id,chanuser->nick_len)==0) {
            printf("The user already exits in %s\n",mesheader->recipient_id);
            return;
        }
        chanuser=chanuser->next_user;
    }
    if (chanuser==NULL) {
         printf("The user doesn't exits in %s\n",mesheader->recipient_id);
         join_channel(mesheader->sender_id,mesheader->recipient_id);
    }
    
}

//Handling client's channel message
void chanMessageHandle(int connfd,char *mesbuff, Msgheader *mesheader){ ///join channel message
    
    char *message1;
    //int blocklen;
    
    //blocklen = mesheader->msglen + 43;
    
    client_to_channel(mesheader);
    
    cc_user * chanuser;
    
    message1=(char *)malloc((MAXMSG+HDRSIZE)*sizeof(char)+1);
    memset(message1, 0, MAXMSG+HDRSIZE+1);
    
    //merge the header and the message body
    serialize_everything(message1, mesbuff,mesheader);

  //  strncpy(message, serialize_everything(mesbuff,mesheader), MAXMSG+HDRSIZE);
    
    
    
    chanuser = get_users_by_channel_name(mesheader->recipient_id);
    
    
    
    printf("The users in channel %s",mesheader->recipient_id);
    
    if (chanuser==NULL) {
        printf("There is no such channel\n");
    }
    
    while (chanuser!=NULL) {
    	//skip echo self
    	if(( strncmp(chanuser->nick, mesheader->sender_id, chanuser->nick_len)) == 0){
    		chanuser=chanuser->next_user;
        	continue;
    	}
    	printf("write to %s buffer\n",chanuser->nick);
        write_to_buffer(chanuser->user_id,message1, MAXMSG+HDRSIZE+1);
        chanuser=chanuser->next_user;
    }
    
    free(message1);
    
}

//Handling client's exit channel message
void exitChanMessageHandle(int connfd,char *mesbuff, Msgheader *mesheader){
    
    printf("exit command\n");
    
    part_channel(mesheader->sender_id,mesheader->recipient_id);
    
    printf("exit %s success\n",mesheader->recipient_id);
}

//Handling client's quite command message
void quitMessageHandle(int connfd,char *mesbuff, Msgheader *mesheader){//quit command
    
    printf("quit command\n");
    
    remove_user(mesheader->sender_id);
    
}

