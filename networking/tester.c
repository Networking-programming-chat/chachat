// struct tester! -A

#include <sys/socket.h>  // defines socket, connect, ...
#include <netinet/in.h>  // defines sockaddr_in
#include <string.h>     // defines memset
#include <stdio.h>       // defines printf, perror, ...
#include <arpa/inet.h>   // inet_pton, ...
#include <unistd.h>      // read, write,..
#include "netproto.h"
#include <stdlib.h>
#include <openssl/sha.h>
#include <sys/stat.h>		//open
#include <fcntl.h>			//open

     
#define SHA_DIGEST_LENGTH 20
 
/*
SHA1 prototype:
unsigned char *SHA1(const unsigned char *d, unsigned long n, unsigned char *md);
*/
 
//cuid = chachat user ID
//cuid.len = SHA_DIGEST_LENGTH
 
 
void generate_cuid(const unsigned char* str, unsigned char* cuid){
	SHA1(str, sizeof(str) - 1, cuid);
}


int main(int argc, char **argv)
{
	int outfd,n;
	Msgheader lol;
	unsigned char cuid1[SHA_DIGEST_LENGTH],cuid2[SHA_DIGEST_LENGTH];
	uint16_t msgsize;
	
	if (argc != 4) {
        fprintf(stderr, "usage: ./tester [user1] [user2] message\n");
        return 1;
    }
	
	memset(&lol, 0, sizeof(Msgheader));
	msgsize=strlen(argv[3]);
    generate_cuid((unsigned char*)argv[1],cuid1);
	generate_cuid((unsigned char*)argv[2],cuid2);
	
	
	lol.msglen=htonl(msgsize);
	lol.sender_id=&cuid1;
	lol.recipient_id=&cuid2;
	
	
	outfd=open("written.txt", O_WRONLY | O_APPEND);
    if(outfd < 0)
        return 1;
	n=pass_message(outfd, argv[3], lol);
	printf("msglen was %d bytes\n", n);
	
	return 0;
}
