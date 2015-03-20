//
//  db_test_main.c
//

#include <stdio.h>
#include "db.h"


int main(int argc, const char * argv[]) {
    
    init_db();

    cc_user *user = get_user_id(1);
    cc_user *user2 = get_user_nick("devon");
    
    print_user(user);
    print_user(user2);
    
    close_db();
    
    printf("done\n");
}