//
//  thread_test_main.c
//  chachat
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdarg.h> // va_
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include <string.h> //strlen
#include "msg_buffers.h"

#define SPAM_THREAD_COUNT 5

void print_line(char *line)
{
    printf("%s\n", line);
    free(line);
}

// Function to test a condition
static void check (int test, const char * message, ...)
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

void simple_test()
{
    // Test buffers
    init_msg_buffers();
    
    new_buffer(2);
    new_buffer(5);
    new_buffer(11);
    new_buffer(9);
    
    list_buffers();
    
    printf("\n Remove one buffer\n");
    
    remove_buffer(5);
    
    list_buffers();
    
    write_to_buffer(11, "This is a message",strlen("This is a message")+1);
    write_to_buffer(11, "This is a second message",strlen("This is a second message")+1);
    write_to_buffer(11, "This is a third message",strlen("This is a third message")+1);
    write_to_buffer(11, "This is a fourth message",strlen("This is a fourth message")+1);
    
    // Consume the lines
    print_line(read_buffer(11));
    print_line(read_buffer(11));
    print_line(read_buffer(11));
    print_line(read_buffer(11));
    print_line(read_buffer(11));
    print_line(read_buffer(11));
    print_line(read_buffer(11));
    
    
    clear_all_msg_buffers();
}

void *reader_thread(void *arg) {
    int buffer_id, i;
    char *msg;
    
    buffer_id = (int)arg;
    
    printf("Reader thread for buffer %d\n", buffer_id);
    
    // Busy loop for a while here
    for (i = 0; i < 2000000; ++i) {
        msg = read_buffer(buffer_id);
        if (msg != NULL) {
            print_line(msg);
        }
    }
    
    printf("Reader thread finished\n");
    
    pthread_exit(arg);
}

void *spam_thread(void *arg) {
    int buffer_id, i, status;
    char msg[128];
    
    buffer_id = (int)arg;
    
    printf("Spam thread for buffer %d\n", buffer_id);
    
    // Spam the buffer
    for (i = 0; i < 1000; ++i) {
        sprintf(msg, "Spam message #%d", i);
        status = write_to_buffer(buffer_id, msg,strlen(msg)+1);
        if (status > 0) {
            printf("Spam message fail: %d item: %d\n", status, i);
        }
    }

    printf("Spam thread finished\n");
    
    pthread_exit(arg);
}

void thread_test()
{
    int status, i, buffer_id;
    pthread_t *thread_id;
    
    buffer_id = 11;
    
    init_msg_buffers();
    
    new_buffer(2);
    new_buffer(5);
    new_buffer(buffer_id);
    new_buffer(9);
    
    list_buffers();
    
    thread_id = malloc(sizeof(pthread_t) * (SPAM_THREAD_COUNT + 1));
    status = thread_id == NULL ? 1 : 0;
    check(status, "malloc failed");

    // One listening thread will read the messages in a busy loop, others will spam the buffer
    
    status = pthread_create(&thread_id[0], NULL, reader_thread, (void*)(long)buffer_id);
    check(status, "thread creation failed");
    
    for (i = 1; i <= SPAM_THREAD_COUNT; ++i) {
        status = pthread_create(&thread_id[i], NULL, spam_thread, (void*)(long)buffer_id);
        check(status, "thread creation failed");
    }
    
    // Join all threads
    for (i = 0; i <= SPAM_THREAD_COUNT; ++i) {
        void *thread_result;
        
        status = pthread_join(thread_id[i],&thread_result);
        check(status, "thread join failed");
        
        printf("Joined thread %d\n", i + 1);
    }
    
    clear_all_msg_buffers();
}

int main(int argc, const char * argv[]) {
    
    //simple_test();
    
    thread_test();
    
    return 0;

}
