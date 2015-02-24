#include <stdio.h>
#include "tcp_connect.h"


int main(void) {
    char *host="nwprog1.netlab.hut.fi";
    char *service="5000";
    
    int socked;
    
    if ((socked=tcp_connect(host, service))<0) {
        perror("tcp connection error\n");
        return -1;
    }
    
    printf("the connected sockfd is: %d",socked);
    return 0;
}