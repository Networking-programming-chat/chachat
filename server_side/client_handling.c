#include "client_handling.h"

void handle_command(){
	printf("command handled\n");
}

int pass_data(thread_id thread, char* data, Msgheader* hdr){
	return 0;
}

int s2c_mainloop(int fd){
	char hdrbuffer[HDRSIZE], messagebuffer[MAXMSG];
	Msgheader hdr;
	int i;
	
	//lol;
	pthreadid* dest_thread_array[];
	
	while(1){
        read_message(fd, messagebuffer, hdrbuffer);
		buffer_to_hdr(hdrbuffer, hdr);
		if(hdr.firstbyte != 0x30)
			handle_command();
		
		if(hdr.recipient_id){
			dest_thread_array=recipient_parser(hdr.recipient_id); //has like an element to hold thread-ID or somesuch.
		}
		
		//this imaginary array has the way to pass data to another thread;
		i=0;
		while(dest_thread_array[i]){
			pass_data(dest_thread_array[i], messagebuffer, hdr);
			i++;
		}
	
    }
}

