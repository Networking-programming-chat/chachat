typedef struct header1{
    char* userid;       //server-generated, or user specified. Handle collisions.
    //char* nick;               //optional, can same nicks exist?
    uint16_t msglen;    //truncate messages to fit 16bit 65k chars.
    int type;           //ascii/bin? utf8?
    char* message;      //malloc'd messagebody. below some ideas.
    int somecounter or such
}Message;

messagebody:

1. readfunction in a thread on the server, reads msglen bytes from socket
2. as a string or binary. probably string. 8-bit characters?

-----------------


typedef struct header1{
    char* userid;       //server-generated, or user specified. Handle collisions.
    //char* nick;               //optional, can same nicks exist?
    uint16_t msglen;    //truncate messages to fit 16bit 65k chars.
    int type;           //ascii/bin? utf8?
}Msgheader;

char* messagebody; // send the data last as string?


---------------------------------------------------



typedef struct header1{
    char* userid;       //server-generated, or user specified. Handle collisions.
    //char* nick;               //optional, can same nicks exist?
    uint16_t msglen;    //truncate messages to fit 16bit 65k chars.
    int type;           //ascii/bin? utf8?
}Msgheader;

int* messagebody; // 8bit padded ascii?, dunno how utf8 knows when to read 2 b/char.



-------------------------------

1. might be easier to separate the header struct from the data after all.


