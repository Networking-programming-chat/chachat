#Client-server connection flow

1. client->DNS server, domain name resolution to get the ip address of the server.
2. client->server, client connects to the server, server allocates an unique ID for the client and add client information to the database on server side.

3. server->client, the server sends REPLY back to the client. For example, "CONNECT SUCCESSFUL" or "CONNECT FAIL"
server->server, synchronize the database between servers.
 
Client sends message:
 
client join a channel: sends /join #channel message to the server. The server update the database and update its channel information. And the servers synchronize the database between server.  client sends P2P message: through the server it connects. The server parses the message and forward the message based on its "routing table" to the destination server. And each server has a unique identifier. The destination server checks if the destination client connects to it and whether it is in the same channel. If yes, sends the message to the destination client. 

client sends "broadcast" message to all people in the channel: the server check the client's channel, and finds all other clients who in the same channel. Forwards the message to their servers.
