#include "client.h"
#include <stdlib.h>
#include <time.h>

char* get_timestamp(char buffer [20]){
   time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime (buffer,20,"%H:%M:%S",timeinfo);
  return buffer;
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


void *reader_thread(void *arg) {
    thread_s *id;
    int sockfd;
    ssize_t n;
    char buffer[1024];
    char buf[20];
    id = (thread_s*) arg;
    sockfd = id->socketfd;

    printf("reader socket: %d\n", id->socketfd);
    
    // Read socket in a loop
    for (;;) {
		printf(" x%sx %s<%s:> ",get_timestamp(buf),COLOR_GRN,&id->header.recipient_id);
		printf("file descriptor is: %d\n",sockfd);
		n = read_message(sockfd, buffer, &id->header);
        n = read(sockfd, buffer, 1023);
        if (n < 0) {
            perror("read error");
            break;
        }
        
        if (n > 0) {
            // Write to stdout
            printf("%s", buffer);
        }
    }
    
    printf("Reader thread finished\n");
    
    pthread_exit(arg);
}

/* void *threadRead(void *argmnt){
	
	struct threadParam *arg = argmnt;
	int n=0;
	char buf[20];
	while(1)
	{
		usleep(READ_TIMEOUT_USEC);
		printf(" x%sx %s<%s:> ",get_timestamp(buf),COLOR_GRN,&arg->header.recipient_id);
		printf("file descriptor is: %d\n",arg->conn);
		if ((n = read_message(arg->conn, arg->recvbuf, &arg->header))<0) {
				perror("read_message:");
				return -1;
		}
		
		
	}

	return NULL;
} */





