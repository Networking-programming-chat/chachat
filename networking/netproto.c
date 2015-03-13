#include "netproto.h"
#include <unistd.h>
#include <stdlib.h>


//NULL if fail, sets errno?
//RETURN VALUE: ptr to Msgheader struct;
Msgheader* create_hdr(const char *str)
{
	uint16_t s_len;
	char a[20],b[20];
	Msgheader* n=malloc(sizeof(Msgheader));	

	if ((sscanf(str, "%hu %s %s", &s_len, &a, &b)) < 3){
		perror("parse error");
		return NULL;
	}
	n.msglen = ntohl(s_len);
	n->recipient_id = strdup(a);
	n->sender_id = strdup(b);
	return n;
}

void free_hdr(Msgheader *hdr)
{
	free(hdr->recipient_id);
	free(hdr->sender_id);
	free(hdr);
}

//give it a header, and a buffer[42]. returns the buffer filled, ready for transmission.
char* serialize_hdr(char* buffer, Msgheader* hdr)
{
	uint16_t tmp = htonl(hdr->msglen);
	strncpy(buffer, (void*)&hdr->firstbyte, 1); //0
	strncpy(&buffer[1], (void*)&tmp, 2);//1,2
	strncpy(&buffer[3], hdr->recipient_id, HASHLEN);//3-22
	strncpy(&buffer[23], hdr->sender_id, HASHLEN);//23-42
	return buffer;
}


//reads messages from socket; thread doesnt know the buffer size needed. Less memory fragmentation? reserve 65k or such :D stack overflow? store header to n;
//RETURN VALUE: pointer to message string;
char* read_message(int fd, char * buffer, int bufsize, Msgheader *hdr){
	int totbytes=0,n=0;
	char hdrbuf[HDRSIZE];
	//reading header;
	while ( (n = read(fd, &hdrbuf[totbytes], HDRSIZE)) > 0) {
		totbytes += n;
		if (totbytes > HDRSIZE) break;
	}
	if (n < 0) {
        perror("hdr_read error");
		return NULL;
    }
	hdr = create_hdr(hdrbuf);
	
	//read msg; msglen bytes;
	totbytes=0;n=0;
	while ( (n = read(fd, &buffer[totbytes], bufsize)) > 0) {
		totbytes += n;
		if (totbytes > hdr->msglen) break;
	}
	if (n < 0) {
        perror("msgread error");
		return NULL;
    }
	return buffer;
}

//writes a normal chat message to socket, returns pointer to said string; requires set sender_id and recipient_id;
//header and message can be reused;
//RETURN VALUE: message length on succes, -1 on error
int pass_message(int fd, const char * message, Msgheader *hdr){
	
	int n,msgsize;
	char* msg;
	char hdrbuf[HDRSIZE];
	Msgheader sendme;
		
	memset(&sendme, 0, sizeof(Msgheader));
	
	msg = malloc(120*sizeof(char));
	memset(msg, 1, 100);
	msgsize=strlen(message);
   	printf("themessage is actually %d bytes long\n", msgsize);

   	serialize_hdr(hdrbuf, hdr);
	n = write(fd, (const void*) hdrbuf, HDRSIZE);
    // Check errors
    if (n < 0) {
        perror("write error with Msgheader");
		return -1;
    }
	n = write(fd, (const void*) msg, msgsize);
    // Check errors
    if (n < 0) {
        perror("write error with message data");
		return -1;
    }
	return n;
	
}
