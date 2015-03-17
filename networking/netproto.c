#include "netproto.h"
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>


//NULL if fail, sets errno?
//RETURN VALUE: ptr to Msgheader struct;
void print_hdr(Msgheader* n){
	if(n){
	printf("----header-----\n");
	printf("firstbyte set to: 0x%02X\n", n->firstbyte);
	printf("msglen: ""%"PRIu16"\n", n->msglen);
	printf("sender: %s\n", n->sender_id);
	printf("recipient: %s\n\n", n->recipient_id);
	}
}

//rudimentary
void is_valid_hdr(Msgheader* n){
	if(n){
	printf("----header-----\n");
	printf("firstbyte set to: 0x%02X\n", n->firstbyte);
	printf("msglen: ""%"PRIu16"\n", n->msglen);
	printf("sender: %s\n", n->sender_id);
	printf("recipient: %s\n\n", n->recipient_id);
	}
}


void buffer_to_hdr(char *str, Msgheader* hdr)
{
	uint16_t* s_len_p;
	uint16_t s_len;
	char *a,*b;
	
	s_len_p = (uint16_t*)&str[1];	//parsing messagelength uint16_t from source. bytes [1], [2].
	s_len = ntohs(*s_len_p);
	printf("setting msglen to: %hu\n", s_len);
	a=&str[3];		//recipient_id
	b=&str[23];		//sender_id
	
	hdr->firstbyte = str[0];
	hdr->msglen = s_len;
	hdr->recipient_id = strndup(a, MAX_NICKLEN);
	hdr->sender_id = strndup(b, MAX_NICKLEN);
}

//give it a header, and a buffer[HDRSIZE]. returns the buffer filled, ready for transmission.
char* serialize_hdr(char* buffer, Msgheader* hdr)
{
	uint16_t tmp = htons(hdr->msglen);
	printf("tmp: %hu\n", tmp);
	memset(buffer, 0, HDRSIZE);	//redundant zeroing. probably.
	memset(buffer, hdr->firstbyte, 1);
	memcpy(&buffer[1], &tmp, 2);//1,2
	
	if(hdr->recipient_id)
		strncpy(&buffer[3], hdr->recipient_id, MAX_NICKLEN);//3-22
	if(hdr->sender_id)
		strncpy(&buffer[23], hdr->sender_id, MAX_NICKLEN);//23-42
	return buffer;
}

void free_hdr(Msgheader *hdr)
{
	free(hdr->recipient_id);
	free(hdr->sender_id);
}

//reads messages from socket; thread doesnt know the buffer size needed. Less memory fragmentation? reserve 65k or such :D stack overflow? store header to n;
//RETURN VALUE: pointer to message string;
int read_message(int fd, char * msg_dest, Msgheader *hdr_dest){
	int totbytes=0,n=0;
	char hdrbuf[HDRSIZE],buffer[MAXMSG+1];
	
	
	if(!hdr_dest || !msg_dest){
		fprintf(stderr, "allocate memory for header/messagebuffer!\n");
		return -1;
	}
	//reading header;
	while ( (n = read(fd, &hdrbuf[totbytes], HDRSIZE)) > 0) {
		totbytes += n;
		if (totbytes >= HDRSIZE) break;
	}
	if (n < 0) {
        perror("hdr_read error");
		return -1;
    }
	
	buffer_to_hdr(hdrbuf, hdr_dest);
	//read msg; msglen bytes;
	totbytes=0;n=0;
	if(hdr_dest->msglen>MAXMSG) hdr_dest->msglen=MAXMSG;
	while ( (n = read(fd, &buffer[totbytes], hdr_dest->msglen)) > 0) {
		totbytes += n;
		if (totbytes >= hdr_dest->msglen) break;
	}
	if (n < 0) {
        perror("msgread error");
		return -1;
    }
	buffer[totbytes]=0;
	printf("read buffer: %s\n", buffer);
	strncpy(msg_dest, buffer, hdr_dest->msglen);
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
	n = write(fd, message, msgsize);
    // Check errors
    if (n < 0) {
        perror("write error with message data");
		return -1;
    }
	return n;
}
