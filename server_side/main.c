//
//  main.c
//  Chachat_1
//
//  Created by Judy on 3/1/15.
//  Copyright (c) 2015 Judy. All rights reserved.
//

#include <stdio.h>
#include "serv.h"
#include <pthread.h> //thread
#include <unistd.h> //read

//type ip address(or not), port number.
int main(int argc, const char * argv[]) {
    int listenfd;
    int connfd;
    socklen_t addrlen, clilen;
    struct sockaddr *cliaddr=NULL;
    
    // for database
    sqlite3 *db;
    
    //under discuss when to connect to servers??
    
    //stat_routine for thread1 is stat_routine1();
    //argu to the stat_routine1 is connfd1
     connfd1=connect to server2;
    pthread_create(thread1,stat_routine1,connfd1);
    
    //stat_routine for thread1 is stat_routine1();
    //argu to the stat_routine1 is connfd1
    connfd2=connect to server3;
    pthread_create(thread2,stat_routine2,connfd2);
    
    
    //open listenfd for clients
    if (argc==2)
        listenfd=serv_listen(NULL, argv[1], &addrlen);
    else if (argc==3)
        listenfd=serv_listen(argv[1], argv[2], &addrlen);
    else {
        fprintf(stderr, "usage: ./main <host> <port#>\n ");
        return -1;
        
    }
    printf("listenfd: %d",listenfd);
    
    //Initial a database and the table
    //need Jussi help
    initial_db(db);
    
    /*waiting for coming connection*/
    
    for (; ; ) {
        if ((connfd=accept(listenfd,cliaddr, &clilen))<0) {
            perror("accept error\n");
            return -1;
        }
        
        fork new thread for the client
            //the start_routine for pthread_create()
            //the argu to the stat_routine is the connfd
        
    }
}

int stat_routine1(int connfd1){
    
    for (; ; ) {
        read from the server2
        parse the data
        if (synchronise database) {
            update database
        }
        else if (forward private message) {
            check destination in this server or not
            if (yes) {
                forward message;
            }else{
                error;
            }
        }
       else if (forward channel message) {
            select the clients where channel==given channel number
            write message to these clients
        }
    }
    
}

int stat_routine2(int connfd2){
    
    for (; ; ) {
        read from the server3
        parse the data
        if (synchronise database) {
            update database
        }
        else if (forward private message) {
            check destination in this server or not
            if (yes) {
                forward message;
            }else{
                error;
            }
        }
        else if (forward channel message) {
            select the clients where channel==given channel number
            write message to these clients
        }
    }

    
}


int stat_routine(int connfd){
    
    
    //generate the id and send it to the client
    //need allocate id protocol
    allocate_id(connfd);
    
    //read from the client
    //need client information protocol
    read(connfd,buffer,size);
    
    //update the database
    insert_db();
    
    int i=1;
    while i is less a new server numbers
        //need server to server synchronise clients' information protocol
           write the client's information to server[i], invloving communication of threads
            i++
            
    while(1){
        read from the client;
        parse the header;
        
        if (send private message) {
            check database the server of destination
            if (at the same server) {
                communicate to the destination thread
            }
            else{
                 //need server to server private message protocol
                communicate to the destination server
            }
        }
        else if(send channel message){
            check database select servers column where channel==given channel number
            //need server to server channel message protocol
            forward the message to these servers
        }
        else if(quit){
            close the thread
            remove the client from the database
            return 0;
        }
        
    }

}
