//
//  db_test_main.c
//

#include <stdio.h>
#include "db.h"

void callback(cc_user *user) {
    
    if (user == NULL) {
        printf("No results!\n");
        return;
    }
    
    print_user(user);
    
    free_cc_user(user);
}

int main(int argc, const char * argv[]) {
    
    init_db();

    get_user_id(1, callback);
    //get_user_nick("devon", callback);
    
    close_db();
    
    printf("done\n");
}