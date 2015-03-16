#ifndef CHACHAT_NETPROTO_H
#define CHACHAT_NETPROTO_H

#include <arpa/inet.h>	//htonl
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>		//check this for errors

#define MAX_NICKLEN 20	//weird and arbitrary, need comments
#define HDRSIZE 43 // use this
#define MAXMSG 65535 // maximum message length to be sent around 2^16-1 bytes.


//client sends NICKNAME as the first thing after connect

//the universal msgheader, firstbyte sets action, recipient field, sender field, plus the message length, aka. bytes to read after.
typedef struct{
	char firstbyte;				//0x00 == normal message, 0x01 == channel, 0x02 == command!, 0x03 == something else
	uint16_t msglen;			//truncate messages to fit 16bit ~65k chars.
	char* recipient_id;			//nick specified, "nulled 20byte array"
	char* sender_id;			//nick specified, "nulled 20byte array"
}Msgheader;					//struct size: 20+20+2+1 bytes = HDRSIZE bytes.

//prints one passed msgheader to stdout!;
void print_hdr(Msgheader* n);

//fill a Msgheader from HDRSIZE bytes string; return said header pointer; argument is the buffer location;
Msgheader* buffer_to_hdr(char *str);

//frees the allocated memory of a header
void free_hdr(Msgheader *hdr);

//pass in a filled header and an empty buffer to create a transmittable string of length HDRSIZE.
char* serialize_hdr(char* buffer, Msgheader* hdr);

//read a messagefrom socket, returns pointer to said string;
int read_message(int fd, char * buffer, Msgheader *hdr);

//writes a normal chat message to socket, returns pointer to said string;
int pass_message(int fd, const char * message, Msgheader* hdr);

#endif
