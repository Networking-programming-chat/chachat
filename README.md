#1. Architecture

Server:
- Main thread: accepts incoming TCP
- Client threads: reads from socket, one per child
- Server threads: maintains sockets for server based communications

Client:
- UI thread: draws UI, waits for strings to draw
- Connection thread: reads from socket and passes strings to draw to UI thread, writes to socket if UI thread sends it data

#2. Requirements

- Server doesn't crash often
- Client UI is responsive. The code seems to do what the user tells it to. Also doesn't crash.
- UTF8 strings work
- Messages are session based, session persistence
 


- Crash screnarios:
  Dropped connections
  

#8. Distribution of work
- Jussi: Databases on servers, threading
- Ibrahim: Client treads UI/connections
- Judy: Server thread implementations: LISTEN/ACCEPT + serve client
- Aapo: Protocol, sockets, struct creation



#9. References






---------


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
3. userid can have prefixes, ex. #!&% whatever to denote channels/groupchats. If specified groupchat room does not exist, it 
is created.



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
	    char* clientarray[]; // holds pointers to new connected client ids, gets passed around in the network to all 
servers.
	    char* serverlist[];        //array of other servers iPs in the network
    }S2SNoise;


- It seems s2c protocol messages could be passed along unaltered inside the network
- Perhaps the clientarray can also hold channelnames hosted on a specific server. There is little difference from protocol 
standpoint.


----------------

#Protocol suggestion 2

Client -> server -> client

    uint8_t idlen           1 byte                      // Length of id
    uint8_t reslen          1 byte                      // Length of recipient
    uint16_t meslen         2 bytes                     // Length of message
    char* id                idlen bytes (up to 255)     // id of sender
    char* res               reslen bytes                // recipient
    uint16_t message        meslen bytes                // Message (in ascii or utf-8, I'd say we can just choose one for 
everyone) 

Server -> server

1. Uses client-to-server protocol to relay messages
2. Uses client-to-server protocol to join the server network
3. Uses server-to-server protocol to list users and channels for new servers joining the network and for servers 
4. Uses server-to-server protocol to list new users, removed users, new channels, and removed channels to other servers

Could this be improved to be more effective?

    uint32_t userlistlen     4 bytes                     // Length of user list
    uint32_t channellistlen  4 bytes                     // Length of channel list
    
    userinfo {                                           // User info "struct"
        uint8_t idlen
        uint8_t nicklen
        uint8_t serveridlen
        char* id             idlen bytes
        char* nick           nicklen bytes
        char* serverid       serveridlen bytes           // if serverid is null, the user has disconnected
    }
    
    channelinfo {                                        // Channel info "struct"
        uint8_t namelen
        uint16_t topiclen
        uint32_t usercount                               // if usercount is 0, the channel is removed
        char* name           namelen bytes
        char* topic          topiclen bytes
        
        useridlist {                                     // User id "struct"
            uint8_t idlen
            char* id         idlen bytes
        }
    }


