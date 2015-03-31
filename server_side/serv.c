#include <stdio.h>
#include <sys/socket.h> //socket.
#include <strings.h>  //bzero
#include <netdb.h> // addrinfo
#include <unistd.h> //close
#include <arpa/inet.h> //inet_ntop

#include "serv.h"
#include "db.h"
#include "msg_buffers.h"



/*This function is borrowed from example
 *Opening an listened fd.
 */
int serv_listen(const char *host, const char *serv){
    int listenfd, n;
    const int on = 1;
    struct addrinfo hints, *res, *ressave;
    
    bzero(&hints, sizeof(struct addrinfo));
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
    
    printf("The ip address we are using is: ");
    print_address(res);
    
    freeaddrinfo(ressave);
    
    return(listenfd);
}

void print_address(const struct addrinfo *res)//this function is based on lecture example
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
    
    //struct sockaddr_in *sin = (struct sockaddr_in *)res->ai_addr;
    const char *ret = inet_ntop(res->ai_family, address,
                                outbuf, sizeof(outbuf));
    printf("%s\n",ret);
}


int client_nick(int socket,char *nick){
    
    char nickname[MAX_NICKLEN];
    char response[50];
    int v,i=0,flag=1;
    ssize_t n1;
    
    do {
        flag=1;
        if ((n1=read(socket,nickname,sizeof(nickname)))<0) {
            perror("read client name error\n");
            return -1;
        }
        
        printf("sender's_id is %s\n", nickname);
        
        v = add_user(nickname);
        
        if (v != 0) {
            printf("Nickname already in use.\n");
            sprintf(response,"Nickname already in use\n");
            
            if ((n1=write(socket,response,sizeof(response)))<0) {
                perror("write nickname response to client fail\n");
                return -1;
            }
            
            flag=0;
            i++;
        }
        
    }while (flag==0&&i<3);
    
    if (i==3) {
        sprintf(response,"Nickname set times out\n");
        return 0;
    }
    if (flag!=0) {
        sprintf(response,"Nickname set success\n");
        if ((n1=write(socket,response,sizeof(response)))<0) {
            printf("write nickname response to client fail\n");
            return -1;
        }
        
    }
    return 0;
}


void chatMessageHandle(int connfd, char *mesbuff, Msgheader *mesheader){
    char response[50];
    ssize_t n1;
    
    printf("client send private chat message\n");
    printf("dest: %s",mesheader->recipient_id);
    
    cc_user *destuser;
    
    destuser=get_user_by_nick(mesheader->recipient_id);
    
    if (destuser==NULL) {
        sprintf(response,"%s doesn't exit!\n",mesheader->recipient_id);
        if((n1=write(connfd, response, strlen(response)+1))<0){
            perror("write response to client fail\n");
            return;
        }
    }
    
    write_to_buffer(destuser->user_id, mesbuff);
    
    
}

void chanMessageHandle(int connfd,char *mesbuff, Msgheader *mesheader){ ///join channel message
    
    cc_user * user_list;
    
    user_list=get_all_users();
    
    print_user_list(user_list);
    printf("\n");
    
    
}



void quitMessageHandle(int connfd,char *mesbuff, Msgheader *mesheader){//quit command
    
    printf("quit command\n");
    
    remove_user(mesheader->sender_id);
    
    close(connfd);
    
}

