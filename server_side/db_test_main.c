//
//  db_test_main.c
//

#include <stdio.h>
#include "db.h"

void callback(cc_user *user) {
    printf("callback\n");
}

int main(int argc, const char * argv[]) {
    
    init_db();

    get_user_id(0, callback);
    
    close_db();
    
    printf("done\n");
}