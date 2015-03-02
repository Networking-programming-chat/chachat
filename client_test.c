#include <stdio.h>
#include "client.h"


int main(void) {
    char *hostAddr="nwprog1.netlab.hut.fi";
    char *hostPort="5000";
    
    int socked;
    
    if ((socked=client(host, service))<0) {
        perror("tcp connection error\n");
        return -1;
    }
    
    printf("the connected sockfd is: %d",socked);
    return 0;
}
