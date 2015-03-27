//
//  msg_buffers.c
//  chacat
//

#include <stdlib.h>
#include "msg_buffers.h"

#define BUFFER_LINE_AMOUNT 10

typedef struct message_buffer {
    int client_id;
    unsigned int buffer_size;
    unsigned int used_buffer_size;
    unsigned int line_count;
    char *buffer[BUFFER_LINE_AMOUNT];
    
    // TODO: linked list may not be the most effective solution here
    struct message_buffer *next_buffer;
} msg_buffer;

static msg_buffer *first_buffer;
static msg_buffer *last_buffer;

void init_msg_buffers()
{
    first_buffer = NULL;
    last_buffer = NULL;
}

int new_buffer(int client_id)
{
    // Create a new buffer item
    msg_buffer *item = malloc(sizeof(msg_buffer));
    
    item->client_id         = client_id;
    item->buffer_size       = 0;
    item->used_buffer_size  = 0;
    item->line_count        = 0;
    item->next_buffer       = NULL;
    
    if (first_buffer == NULL) {
        first_buffer = item;
        last_buffer = item;
    } else {
        last_buffer->next_buffer = item;
        last_buffer = item;
    }
    
    return 0;
}

int remove_buffer(int client_id)
{
    if (first_buffer == NULL) {
        // Nothing here yet
        return 1;
    }
    
    // Go through all buffers and remove this one
    msg_buffer *item = NULL;
    msg_buffer *iterator = first_buffer;
    
    while (iterator->next_buffer != NULL) {
        if (iterator->next_buffer->client_id == client_id) {
            // found
            item = iterator;
            break;
        }
        iterator = iterator->next_buffer;
    }
    
    if (item == NULL) {
        // Not found
        return 1;
    }
    
    
    
    return 0;
}

char* read_buffer(int client_id);

int write_to_buffer(int client_id, const char* message);
