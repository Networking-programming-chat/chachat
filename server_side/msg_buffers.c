//
//  msg_buffers.c
//  chacat
//

#include <stdlib.h>
#include <stdio.h>  // printf
#include <pthread.h>
#include <stdarg.h> // va_
#include <errno.h>
#include <string.h>
#include "msg_buffers.h"

#define BUFFER_LINE_AMOUNT 10

// Function to test a condition
static void check_ims (int test, const char * message, ...)
{
    if (test) {
        va_list args;
        
        // Iterate the arguments
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        
        // Print error message to stderr
        fprintf (stderr, ": %s\n", strerror(errno));
        
        // Failure
        exit (1);
    }
}

void m_lock(pthread_mutex_t *m) {
    check_ims(pthread_mutex_lock(m), "mutex lock failed");
}

void m_unlock(pthread_mutex_t *m) {
    sched_yield();
    check_ims(pthread_mutex_unlock(m), "mutex unlock failed");
}

void c_wait(pthread_cond_t *c, pthread_mutex_t *m) {
    check_ims(pthread_cond_wait(c, m), "condition variable wait failed");
}

void c_broadcast(pthread_cond_t *c) {
    check_ims(pthread_cond_broadcast(c), "condition variable broadcast failed");
}

typedef struct message_buffer {
    int client_id;
    unsigned int first_line;
    unsigned int line_count;
    pthread_mutex_t buffer_access;
    pthread_cond_t read_cond;
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
    int i;
    msg_buffer *item = malloc(sizeof(msg_buffer));
    
    item->client_id         = client_id;
    item->first_line        = 0;
    item->line_count        = 0;
    item->next_buffer       = NULL;
    
    check_ims(pthread_mutex_init(&(item->buffer_access), NULL), "mutex init failed");
    check_ims(pthread_cond_init(&(item->read_cond), NULL), "condition variable init failed");

    if (first_buffer == NULL) {
        first_buffer = item;
        last_buffer = item;
    } else {
        last_buffer->next_buffer = item;
        last_buffer = item;
    }
    
    // Clear the buffers
    for (i = 0; i < BUFFER_LINE_AMOUNT; ++i) {
        item->buffer[i] = NULL;
    }
    
    return 0;
}

void free_buffer(msg_buffer *item)
{
    int i;
    
    // TODO: Free all the buffers?
    for (i = 0; i < BUFFER_LINE_AMOUNT; ++i) {
        if (item->buffer[i] != NULL) {
            free(item->buffer[i]);
        }
    }
    
    check_ims(pthread_mutex_destroy(&(item->buffer_access)), "mutex destroy failed");
    check_ims(pthread_cond_destroy(&(item->read_cond)), "condition variable destroy failed");

    free(item);
}

int remove_buffer(int client_id)
{
    msg_buffer *item = NULL;
    msg_buffer *iterator = first_buffer;
    
    if (first_buffer == NULL) {
        // Nothing here yet
        return 1;
    }
    
    // Go through all buffers and remove this one
    while (iterator->next_buffer != NULL) {
        if (iterator->next_buffer->client_id == client_id) {
            // found
            item = iterator->next_buffer;
            break;
        }
        iterator = iterator->next_buffer;
    }
    
    if (item == NULL) {
        // Not found
        return 1;
    }
    
    // Found, remove one from the middle
    iterator->next_buffer = item->next_buffer;
    free_buffer(item);
    
    return 0;
}

void clear_all_msg_buffers()
{
    msg_buffer *item = NULL;
    msg_buffer *iterator = first_buffer;
    
    while (iterator != NULL) {
        item = iterator;
        iterator = iterator->next_buffer;
        
        free_buffer(item);
    }
}

char* read_buffer_internal(int client_id, int block)
{
    msg_buffer *item = first_buffer;
    
    // Consume a message from a buffer
    
    // First, find the item
    while (item != NULL) {
        if (item->client_id == client_id) {
            break;
        }
        item = item->next_buffer;
    }
    
    if (item == NULL) {
        // Client_id does not exist in this list
        return NULL;
    }
    
    // Get mutex to access buffer items
    m_lock(&(item->buffer_access));
    
    // Find the next line to read from this buffer
    
    if (item->line_count <= 0) {
        // No lines to read
        
        if (block == 0) {
            // Do not block, just return
            m_unlock(&(item->buffer_access));
            return NULL;
        }
        
        // Block until there is a signal of a new message
        while (item->line_count <= 0) {
            c_wait(&(item->read_cond), &(item->buffer_access));
        }
    }
    
    // Get the first line in the buffer
    char *line = item->buffer[item->first_line];
    item->buffer[item->first_line] = NULL;
    
    // Update the counters
    item->first_line++;
    item->line_count--;
    
    // This buffer is a ring
    if (item->first_line >= BUFFER_LINE_AMOUNT) {
        item->first_line = 0;
    }
    
    // Release mutex
    m_unlock(&(item->buffer_access));
    
    return line;
}

char* read_buffer(int client_id)
{
    return read_buffer_internal(client_id, 0);
}

char* read_buffer_block(int client_id)
{
    return read_buffer_internal(client_id, 1);
}

int write_to_buffer(int client_id, const char* message)
{
    unsigned int slot;
    size_t msg_len;
    msg_buffer *item = first_buffer;
    
    // Write a message to a buffer
    // TODO: Mutex for thread safety
    
    // First, find the item
    while (item != NULL) {
        if (item->client_id == client_id) {
            break;
        }
        item = item->next_buffer;
    }
    
    if (item == NULL) {
        // Client_id does not exist in this list
        return 1;
    }
   
    // Get mutex to access buffer items
    m_lock(&(item->buffer_access));
    
    // Check if there is room to write a line
    if (item->line_count >= BUFFER_LINE_AMOUNT) {
        // Buffer is full
        m_unlock(&(item->buffer_access));
        return 2;
    }
    
    // Write a line to the next available slot
    slot = (item->first_line + item->line_count) % BUFFER_LINE_AMOUNT;
    msg_len = strlen(message);
    item->buffer[slot] = malloc(sizeof(char) * msg_len + 1);
    if (item->buffer[slot] == NULL) {
        m_unlock(&(item->buffer_access));
        return 3;
    }
    
    strncpy(item->buffer[slot], message, msg_len);
    item->buffer[slot][msg_len] = '\0';
    
    // Add line to bookkeeping
    item->line_count ++;
    
    // Broadcast the addition of a line
    c_broadcast(&(item->read_cond));
    
    // Release mutex
    m_unlock(&(item->buffer_access));
    
    return 0;
}

void list_buffers()
{
    int i = 0;
    msg_buffer *iterator = first_buffer;

    while (iterator != NULL) {
        // Print the buffer data
        printf("Buffer %d:\nClient id: %d\n", i, iterator->client_id);
        ++i;
        iterator = iterator->next_buffer;
    }
}
