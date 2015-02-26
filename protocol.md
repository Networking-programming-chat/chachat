#c2s protos
typedef struct header1{
    char* userid;       //server-generated, or user specified. Handle collisions.
    uint16_t msglen;    //truncate messages to fit 16bit 65k chars.
    int type;           //ascii/bin? utf8?
    char* message;      //malloc'd messagebody. below some ideas.
    int somecounter or such
}Message;

messagebody:

1. readfunction in a thread on the server, reads msglen bytes from socket
2. as a string or binary. probably string. 8-bit characters?
3. userid can have prefixes, ex. #!&% whatever to denote channels/groupchats. If specified groupchat room does not exist, it is created.



-----------------

    typedef struct header1{
        char* userid;       //server-generated, or user specified. Handle collisions.
        uint16_t msglen;    //truncate messages to fit 16bit 65k chars.
        int type;           //ascii/bin? utf8?
    }Msgheader;
    
    char* messagebody; // send the data last as string?


---------------------------------------------------

    typedef struct header1{
        char* userid;       //server-generated, or user specified. Handle collisions.
        uint16_t msglen;    //truncate messages to fit 16bit 65k chars.
        int type;           //ascii/bin? utf8?
    }Msgheader;
    
    int* messagebody; // 8bit padded ascii?, dunno how utf8 knows when to read 2 b/char.


-------------------------------

1. might be easier to separate the header struct from the data after all.

#s2c example

    typedef struct header_s2c{
	    char* senderid;	//server-generated, or user specified. Handle collisions.
	    char* recipient;	//adressee, channel/user
	    uint16_t msglen;    //truncate messages to fit 16bit 65k chars.
	    int type;           //ascii/bin? utf8?
	    int somecounter or such
    }s2cMessage;


#s2s

    typedef struct header_s2s{
        uint16_t arraylen;    //truncate messages to fit 16bit 65k chars.
        uint16_t listlen;    //truncate messages to fit 16bit 65k chars.
    }s2sMessage;
            
    typedef struct data1{
	    char* clientarray[]; // holds pointers to new connected client ids, gets passed around in the network to all servers.
	    char* serverlist[];        //array of other servers iPs in the network
    }S2SNoise;


- It seems s2c protocol messages could be passed along unaltered inside the network
- Perhaps the clientarray can also hold channelnames hosted on a specific server. There is little difference from protocol standpoint.

