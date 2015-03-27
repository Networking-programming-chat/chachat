//
//  thread_test_main.c
//  chachat
//

#include <stdio.h>
#include <stdlib.h>
#include "msg_buffers.h"

void print_line(char *line) {
    printf("%s\n", line);
    free(line);
}

int main(int argc, const char * argv[]) {
    
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
    
    write_to_buffer(11, "This is a message");
    write_to_buffer(11, "This is a second message");
    write_to_buffer(11, "This is a third message");
    write_to_buffer(11, "This is a fourth message");
    
    // Consume the lines
    print_line(read_buffer(11));
    print_line(read_buffer(11));
    print_line(read_buffer(11));
    print_line(read_buffer(11));
    print_line(read_buffer(11));
    print_line(read_buffer(11));
    print_line(read_buffer(11));

    
    clear_all_msg_buffers();
    
    return 0;

}
