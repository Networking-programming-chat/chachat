//
//  msg_buffers.h
//  chachat
//

#ifndef __Chachat__msg_buffers__
#define __Chachat__msg_buffers__

// This should be called once as the program starts
void init_msg_buffers();

// This will remove all the buffers and free the memory
void clear_all_msg_buffers();

// Creates a new message buffer
// A client connection thread is supposed to call this function when connected
int new_buffer(int client_id);

int remove_buffer(int client_id);

// Reads a line from a message buffer
// If the buffer is empty returns NULL
// Returned message is malloced, should be freed after consumed
char* read_buffer(int client_id);

// Like previous, but will block until there is a new message to read
char* read_buffer_block(int client_id);

// When sending messages to other clients, threads use this method to write to client buffer
// That client's connection thread will read the message from that buffer
// Message will be copied to a new malloced memory address in the heap
int write_to_buffer(int client_id, const char* message, int n);

// List the buffers to stdout for debug purposes
void list_buffers();

#endif /* defined(__Chachat__msg_buffers__) */
