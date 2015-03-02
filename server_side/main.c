//
//  main.c
//  Chachat_1
//
//  Created by Judy on 3/1/15.
//  Copyright (c) 2015 Judy. All rights reserved.
//

#include <stdio.h>
#include "serv.h"

//type ip address(or not), port number.
int main(int argc, const char * argv[]) {
    int listenfd;
    socklen_t addrlen;
    
    if (argc==2)
        listenfd=serv_listen(NULL, argv[1], &addrlen);
    if (argc==3)
        listenfd=serv_listen(argv[1], argv[2], &addrlen);
    
    printf("listenfd: %d",listenfd);
        
    }
