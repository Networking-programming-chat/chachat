//
//  db_test_main.c
//

#include <stdio.h>
#include "db.h"


int main(int argc, const char * argv[]) {
    
    init_db();

    cc_user *user = get_user_by_id(1);
    cc_user *user2 = get_user_by_nick("devon");
    
    //cc_user *user3 = get_users_by_channel_id(0);
    cc_user *user3 = get_users_by_channel_name("Kanava");
    
    //cc_channel *channels = get_channels_of_user_id(0);
    cc_channel *channels = get_channels_of_user_nick("devon");
    
    print_user(user);
    print_user(user2);
        
    print_user_list(user3);
    print_channel_list(channels);
    
    close_db();
    
    printf("done\n");
}