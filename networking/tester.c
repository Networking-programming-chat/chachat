// struct tester! -A

#include <sys/socket.h>  // defines socket, connect, ...
#include <netinet/in.h>  // defines sockaddr_in
#include <string.h>     // defines memset
#include <stdio.h>       // defines printf, perror, ...
#include <arpa/inet.h>   // inet_pton, ...
#include <unistd.h>      // read, write,..
#include "netproto.h"
#include <stdlib.h>
//#include <openssl/sha.h>
#include <sys/stat.h>		//open
#include <fcntl.h>			//open

     
#define SHA_DIGEST_LENGTH 20
 
/*
SHA1 prototype:
unsigned char *SHA1(const unsigned char *d, unsigned long n, unsigned char *md);
*/
 
//cuid = chachat user ID
//cuid.len = SHA_DIGEST_LENGTH
int serverconnect(int sockfd, char* ex_id,  char* serverip){
	
	//char serverip[] = "195.148.124.76";
    struct sockaddr_in servaddr;
	
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(5000); /* assignment server port = 5000 */
    if (inet_pton(AF_INET, serverip, &servaddr.sin_addr) <= 0) {
        fprintf(stderr, "inet_pton error for %s\n", serverip);
        return 1;
    }

    // Connect to IP address and port indicated by servaddr
    // Check if it was succesful
    if (connect(sockfd,
                (struct sockaddr *) &servaddr,
                sizeof(servaddr)) < 0) {
        perror("connect error");
        return 1;
    }	
	return 0;
}

int write_to_db(int fd, char cuid, void * db){
	return 0;
}
 
void hexprinter(char* str,int num){
	int i;
	for (i = 0;i<num;i++){
		printf("%02X ", (unsigned int)str[i]);
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	int sockfd,n;
	//FILE* testifilee;
	Msgheader* lol;

	printf("uint16_t: %lu bytes\n", sizeof(uint16_t));
	// Open a stream (TCP) IPv4 socket, and check if succesful
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return 1;
    }
	//connect to localhost port 5000, post some gibberish for the lulz;
	n = serverconnect(sockfd, "gibberish\n", "127.0.0.1");
	//check basics
	if (n > 0) {
		return 1;
	}
	
	lol=malloc(sizeof(Msgheader));
	//unsigned char cuid1[SHA_DIGEST_LENGTH],cuid2[SHA_DIGEST_LENGTH];
	uint16_t msgsize;
	if (argc != 4) {
        fprintf(stderr, "usage: ./tester [user1] [user2] message\n");
        return 1;
    }
	
	//memset(&lol, 0x30, sizeof(Msgheader));
	
	msgsize=strlen(argv[3]);
    //allocate_id((unsigned char*)argv[1],cuid1);
	//allocate_id((unsigned char*)argv[2],cuid2);
	
	
	lol->firstbyte=0x02;
	lol->msglen=msgsize;
	lol->sender_id=argv[1];
	lol->recipient_id=argv[2];
	
	hexprinter((char*)lol, 44);
	print_hdr(lol);
/* 	printf("trying to open a file!\n");
	testifilee=fopen("message.txt", "w");
    outfd=fileno(testifilee);
	
	if(!testifilee){
		perror("lold\n");
        return 1;
	} */
	printf("writing stuff to socket\n");
	n=pass_message(sockfd, argv[3], lol);
	printf("msglen was %d bytes\n", n);
	free(lol);
	return 0;
}