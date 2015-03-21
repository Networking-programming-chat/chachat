#include "client.h"
#include "../networking/netproto.h"



int main(void) {
char *hostAddr="localhost";
char *hostPort="3333";
int con;

client(hostAddr, hostPort);

return 0;
}

   
