//
//  db_test_main.c
//

#include <stdio.h>
#include "db.h"


int main(int argc, const char * argv[]) {
    
    init_db();
    
    //int s = add_user("Kuukkeli");

    /*cc_user *user = get_user_by_id(1);
    cc_user *user2 = get_user_by_nick("Kuukkeli");
    
    //cc_user *user3 = get_users_by_channel_id(0);
    cc_user *user3 = get_users_by_channel_name("Fiksut");
    
    //cc_channel *channels = get_channels_of_user_id(0);
    cc_channel *channels = get_channels_of_user_nick("devon");
    
    print_user(user);
    print_user(user2);
        
    print_user_list(user3);
    print_channel_list(channels);
    
    free_cc_user(user);
    free_cc_user(user2);
    free_cc_user(user3);
    
    free_cc_channel(channels);*/
    
    //join_channel("devon", "Hoopot");
    //join_channel("devon", "ChatThing");

    cc_user * user_list = get_users_by_channel_name("ChatThing");
    cc_channel * channel_list = get_channels_of_user_nick("devon");
    
    print_user_list(user_list);
    printf("\n");
    print_channel_list(channel_list);

    free_cc_user(user_list);
    free_cc_channel(channel_list);
    
    close_db();
    
    printf("done\n");
}