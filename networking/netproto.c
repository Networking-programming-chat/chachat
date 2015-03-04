#include "netproto.h"


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

void free_hdr(Msgheader *n)
{
	free(n.recipient_id);
	free(n.sender_id);
	free(n);
}
//reads messages from socket; thread doesnt know the buffer size needed. Less memory fragmentation? reserve 65k or such :D stack overflow? store header to n;
//RETURN VALUE: pointer to message string;
char* read_message(int fd, char * buffer, int bufsize, Msgheader *n){
	int totbytes=0,n=0;
	size_t hdrsize=sizeof(Msgheader);
	char hdrbuf[hdrsize];
	
	
	//reading header;
	while ( (n = read(fd, &hdrbuf[totbytes], hdrsize)) > 0) {
		totbytes += n;
		if (totbytes > hdrsize) break;
	}
	if (n < 0) {
        perror("hdr_read error");
		return NULL;
    }
	hdr = parse_hdr(hdrbuf);
	
	//read msg; msglen bytes;
	totbytes=0;n=0;
	while ( (n = read(fd, &buffer[totbytes], bufsize)) > 0) {
		totbytes += n;
		if (totbytes > hdr.msglen) break;
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
int pass_message(int fd, const char * message, Msgheader *n){
	
	int n,msgsize;
	char msg[msgsize];
	Msgheader sendme;
	
	memset(&sendme, 0, sizeof(Msgheader));
	msgsize=strlen(message);
    
	sendme.msglen=htonl(msgsize);
	sendme->sender_id=n->sender_id;
	sendme->recipient_id=n->recipient_id;
	
	n = write(fd, (const void*) &sendme, sizeof(Msgheader));
    // Check errors
    if (n < 0) {
        perror("write error with Msgheader");
		return -1;
    }
	n = write(fd, (const void*) message, msgsize);
    // Check errors
    if (n < 0) {
        perror("write error with message data");
		return -1;
    }
	return n;
	
}
