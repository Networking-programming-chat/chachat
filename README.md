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

#3. Instructions

- testserver/client builds with "make" in project root. this was used for protocol testing.
- make testserver/make testclient to build separately


#4. Communication protocol

- Universal header for commands/messages 
- Bulk of transmitted data - 
- Also handles binary for db transmissions
- normal messages pass as is, through servers
 
1. send header

Application needs 2 allocated headers to store outgoing and incoming data per connection.

	typedef struct{
	char firstbyte;				//0x00 == normal message, 0x01 == channel, 0x02 == command!, 0x03 == something else
	uint16_t msglen;			//truncate messages to fit 16bit ~65k chars.
	char* recipient_id;			//nick specified, "nulled 20byte array"
	char* sender_id;			//nick specified, "nulled 20byte array"
	}Msgheader;

2. send data separately

3. functions for applications:

//read a message from socket, store message to arg2, header to agr3
int read_message(int fd, char * msg_dest, Msgheader *hdr_dest);

//writes a normal message to socket. Returns read() return after last read after succesful forwarding. Header MUST contain the length of attached message for proper function with binary.
int pass_message(int fd, const char * message, Msgheader* hdr);

//frees the allocated memory of a header
void free_hdr(Msgheader *hdr);

	  
#6. Quality assurance
##Protocol
- with testserver/testclient one can give arbitrary data to protocol application functions and see how it behaves
- tested against null segfaults
- tested for correct behavior in basic use cases

##Server & Client

Client connect to server, one thread:
 
1. client->server, establish connection.

2, If the connection success (check the the return value of connect()), the client prints the rules to the client’s UI screen. 

3. client->server, client sends its nickname to the server not more than 20 characters long.

4. server check the database whether the nick is in use. Yes, server sends the client a msg to choose another nick instead,
and after 3 trials, return to connection window. No, server allocates a unique ID to the clients and add client information to the database on server side.

//Not in this phase 4. server->server, synchronize the database between servers.

5. Every time the client parse the UI input. Private message’s format ”/chat @destination messagebody“. Other Command like quit format ”/quit”, “/join channel name”.

   Then the client fills the header Mesghheader. If it is normal message, set the “firstbyte” to ‘0’; If it is command, set the “firstbyte” to ‘2’. And the recipent_id is 20 bytes of 0. The command content (remove the slash) is included in messagebody. And sends it to the server.


6. The server read the header. If it is quit command, close the connection and delete the client’s information from the database.

 
Client sends message:

client chat privately: send /chat @nick (existing nickname) 
The server checks if nick exists already in all servers' dbase , if yes, forward message to nick (a client on the other end),
if no, send a msg e.g. @nick is not a user yet, return to current window.

client join a channel: sends /join #channel message to the server. 
The server update the database and update its channel information. 
And the servers synchronize the database between server.

client sends /quit
The server close the connection and delete the user’s information in database. And send synchronise information to other servers.
  
Client sends P2P message: through the server it connects with. 
The server parses the message and forward the message based on its "routing table" to the destination server. 
And each server has a unique identifier. The destination server checks if the destination client connects to it and whether it is in the same channel. 
If yes, sends the message to the destination client. 

client sends "broadcast" message to all people in the channel: the server check the client's channel, and finds all other clients who in the same channel. 
Forwards the message to their servers.

##Database

Unit tests to test the database usage functions

- To compile, run `make dbtest` in server_side
- To run, run the `dbtest` binary

## Inter-thread communication

Unit tests to test the inter-thread communication module

- To compile, run `make threadtest` in server_side
- To run, run the `threadtest` binary

#8. Distribution of work
- Jussi: Databases on servers, threading
- Ibrahim: Client treads UI/connections
- Judy: Server thread implementations: LISTEN/ACCEPT + serve client
- Aapo: Protocol, sockets, struct creation



#9. References






---------




