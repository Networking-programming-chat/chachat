#ifndef CHACHAT_NETPROTO_H
#define CHACHAT_NETPROTO_H

#include <inttypes.h>


#define MAX_NICKLEN 20	//weird and arbitrary, need comments
#define HDRSIZE 43 // use this
#define MAXMSG 65535 // maximum message length to be sent around 2^16-1 bytes.
#define READ_TIMEOUT_USEC 1000000


//client sends NICKNAME as the first thing after connect

//the universal msgheader, firstbyte sets action, recipient field, sender field, plus the message length, aka. bytes to read after.
typedef struct{
	char firstbyte;				
	uint16_t msglen;			//truncate messages to fit 16bit ~65k chars.
	char* recipient_id;			//nick specified, "nulled 20byte array"
	char* sender_id;			//nick specified, "nulled 20byte array"
}Msgheader;					//struct size: 20+20+2+1 bytes = HDRSIZE bytes.

//prints one passed msgheader to stdout!;
void print_hdr(Msgheader* n);

//fill a Msgheader from HDRSIZE bytes string; return said header pointer; argument is the buffer location;
void buffer_to_hdr(char *str, Msgheader* hdr);

//frees the allocated memory of a header
void free_hdr(Msgheader *hdr);

//pass in a filled header and an empty buffer to create a transmittable string of length HDRSIZE.
char* serialize_hdr(char* buffer, Msgheader* hdr);

//take a message body and a header as parameters and, stick them together. returns a pointer to combined string. NULL if weirds.
char* serialize_everything(char* buffer, Msgheader* hdr);

//get separate message and hdr to given pointers from a serialize_everything()'d chunk. return less than 0 if fails. NOTE: might not need to copy everything..
int split_datas(char* chunk, char* message, Msgheader* hdr);

//read a message from socket, store message to arg2, header to agr3. Some header fields are malloc'd, need to be freed with free_hdr before freeing header itself.
int read_message(int fd, char * msg_dest, Msgheader *hdr_dest);

//read a message from socket, store message to arg2, header to agr3. Some header fields are malloc'd, need to be freed with free_hdr before freeing header itself. Timeouts after 0.5 seconds and returns 0 if no other errors found.
int server_read(int fd, char * msg_dest, Msgheader *hdr_dest);

//writes a normal chat message to socket. Returns message length after succesful forwarding. Header MUST contain the length of attached message for proper function.
int pass_message(int fd, const char * message, Msgheader* hdr);

#endif
