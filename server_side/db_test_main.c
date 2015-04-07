//
//  db_test_main.c
//

#include <stdio.h>
#include "db.h"
#include "terminal_color.h"


int main(int argc, const char * argv[]) {
    
    printf(COLOR_GRN "Database unit tests\n" COLOR_NRM);
    
    init_db();
    
    add_user("Kuukkeli");

    cc_user *user = get_user_by_id(1);
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
    
    free_cc_channel(channels);
    
    printf("\n");
    printf("creating chanlist\n");
    add_channel("nfl");
    cc_channel * channel_list = get_all_channels(); // get_channels_of_user_nick("devon");
    print_channel_list(channel_list);
	printf("done--\n");
    printf("\n");
    
        
    join_channel("devon", "nfl");
    join_channel("devon", "chat");
    part_channel("devon", "chat");
    
    join_channel("hadrn", "nfl");
    join_channel("Troop", "nfl");
    join_channel("Madden", "nfl");
    
    // Add test users
    add_user("devon");
    add_user("hadrn");
    add_user("Troop");
    add_user("Poor");
    add_user("Madden");
    add_user("har_har");
    
    // Add test channels
    add_channel("test");
    add_channel("chat");
    
    //remove_user("devon");
    
    //remove_channel("nfl");
    
    set_channel_topic("nfl", "Discussion about nfl");
    change_nick("Troop", "Madden2");
	
	printf("\n");
    printf("users on nfl\n");
    cc_user * user_list = get_users_by_channel_name("nfl"); // get_users_by_channel_name("nfl");
	print_user_list(user_list);
	printf("---------end\n");
    printf("\n");
    
  
    channel_list = get_all_channels(); // get_channels_of_user_nick("devon");
    
    print_channel_list(channel_list);

    free_cc_user(user_list);
    free_cc_channel(channel_list);
    
    close_db();
    
    printf("done\n");
}
