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
int write_to_db(int fd, char cuid, void * db){
	return 0;
}
 
/* char* allocate_id(int fd, char* str){
	char cuid[SHA_DIGEST_LENGTH];
	//cuid = SHA1(str, sizeof(str) - 1, cuid);
	//write_to_db(fd, cuid, db);
	return cuid;
} */


int main(int argc, char **argv)
{
	int outfd,n;
	FILE* testifilee;
	Msgheader* lol;
	
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
	
	
	lol->msglen=msgsize;
	lol->sender_id=argv[1];
	lol->recipient_id=argv[2];

	printf("trying to open a file!\n");
	testifilee=fopen("written.txt", "w");
    outfd=fileno(testifilee);
	if(!testifilee){
		perror("lold\n");
        return 1;
	}
	
	n=pass_message(outfd, argv[3], lol);
	printf("msglen was %d bytes\n", n);
	free(lol);
	return 0;
}