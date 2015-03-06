#ifndef CHACHAT_NETPROTO_H
#define CHACHAT_NETPROTO_H

#include <arpa/inet.h>	//htonl
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>		//check this for errors

#define MAX_NICKLEN 13	//weird and arbitrary, need comments

//client sends NICKNAME as the first thing after connect

//the universal msgheader, firstbyte sets action, recipient field, sender field, plus the message length, aka. bytes to read after.
typedef struct{
	char firstbyte;				//0x00 == normal message, 0x01 == channel, 0x02 == command!, 0x03 == something else
	uint16_t msglen;			//truncate messages to fit 16bit ~65k chars.
	char* recipient_id;			//server-generated, SHA1 of the nick specified
	char* sender_id;			//server-generated, SHA1 of the nick specified
}Msgheader;					//struct size: 20+20+2+1 bytes = 43 bytes.

//fill a Msgheader from 43 bytes string; return said header pointer; second argument is the storage location;
Msgheader* parse_hdr(const char *str, Msgheader *n);

//read a messagefrom socket, returns pointer to said string;
char* read_message(int fd, char * buffer, int bufsize, Msgheader *n);

//writes a normal chat message to socket, returns pointer to said string;
char* pass_message(int fd, const char * buffer, int msglen);

#endif
