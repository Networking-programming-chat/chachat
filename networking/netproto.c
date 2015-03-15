#include "netproto.h"
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>


//NULL if fail, sets errno?
//RETURN VALUE: ptr to Msgheader struct;
void print_hdr(Msgheader* n){
	printf("----header-----\n");
	printf("firstbyte set to: 0x%02X\n", n->firstbyte);
	printf("msglen: ""%"PRIu16"\n", n->msglen);
	printf("sender: %s\n", n->sender_id);
	printf("recipient: %s\n\n", n->recipient_id);
}

Msgheader* buffer_to_hdr(char *str)
{
	//char firstbyte;
	//uint16_t s_len;
	char *a,*b;
	//a[20],b[20];
	Msgheader* hdr=malloc(sizeof(Msgheader)); //this sizeof has sizes of nick pointers.

	/* if ((sscanf(str, "%c %hu", &firstbyte, &s_len)) < 2){
		perror("parse error");
		return NULL;
	} */
	a=&str[3];
	b=&str[23];
	hdr->firstbyte = str[0];
	hdr->msglen = ntohs((uint16_t)str[1]);
	hdr->recipient_id = strndup(a, MAX_NICKLEN);
	hdr->sender_id = strndup(b, MAX_NICKLEN);
	return hdr;
}

void free_hdr(Msgheader *hdr)
{
	free(hdr->recipient_id);
	free(hdr->sender_id);
	free(hdr);
}

//give it a header, and a buffer[HDRSIZE]. returns the buffer filled, ready for transmission.
char* serialize_hdr(char* buffer, Msgheader* hdr)
{
	uint16_t tmp = htons(hdr->msglen);
	printf("tmp: %hu\n", tmp);
	memset(buffer, hdr->firstbyte, 1);
	memcpy(&buffer[1], &tmp, 2);//1,2
	strncpy(&buffer[3], hdr->recipient_id, MAX_NICKLEN);//3-22
	strncpy(&buffer[23], hdr->sender_id, MAX_NICKLEN);//23-42
	return buffer;
}


//reads messages from socket; thread doesnt know the buffer size needed. Less memory fragmentation? reserve 65k or such :D stack overflow? store header to n;
//RETURN VALUE: pointer to message string;
int read_message(int fd, char * buffer, Msgheader *hdr){
	int totbytes=0,n=0;
	char hdrbuf[HDRSIZE];
	
	//reading header;
	while ( (n = read(fd, &hdrbuf[totbytes], HDRSIZE)) > 0) {
		totbytes += n;
		if (totbytes > HDRSIZE) break;
	}
	if (n < 0) {
        perror("hdr_read error");
		return -1;
    }
	hdr = buffer_to_hdr(hdrbuf);
	
	//read msg; msglen bytes;
	totbytes=0;n=0;
	while ( (n = read(fd, &buffer[totbytes], MAXMSG)) > 0) {
		totbytes += n;
		if (totbytes > hdr->msglen) break;
	}
	if (n < 0) {
        perror("msgread error");
		return -1;
    }
	return n;
}

//writes a normal chat message to socket, returns pointer to said string; requires set sender_id and recipient_id;
//header and message can be reused;
//RETURN VALUE: message length on succes, -1 on error
int pass_message(int fd, const char * message, Msgheader* hdr){
	
	int n,msgsize;
	char hdrbuf[HDRSIZE];
	Msgheader sendme;
		
	memset(&sendme, 0, sizeof(Msgheader));
	if(message)	msgsize=strlen(message);
	else msgsize=0;
	if(msgsize>MAXMSG) msgsize=MAXMSG;
   	printf("the message is actually %d bytes long\n", msgsize);

	//serialize does the byteorders n stuffs.
   	serialize_hdr(hdrbuf, hdr);
	n = write(fd, (const void*) hdrbuf, HDRSIZE);
    // Check errors
    if (n < 0) {
        perror("write error with Msgheader");
		return -1;
    }
	printf("writing %s to file\n", message);
	n = write(fd, message, msgsize);
    // Check errors
    if (n < 0) {
        perror("write error with message data");
		return -1;
    }
	return n;
}
