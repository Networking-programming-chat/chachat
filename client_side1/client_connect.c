#include "client.h"

int client_connect(const char *servName, const char *servPort){

int n,sockfd;
struct addrinfo hints, *res,*ressave;
bzero(&hints, sizeof(struct addrinfo));
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;

	if ( (n = getaddrinfo(servName, servPort, &hints, &res)) != 0) {
		fprintf(stderr, "client error for %s, %s: %s\n",
		servName, servPort, gai_strerror(n));
	return -1;
	}
	
ressave = res;
	do {
		sockfd = socket(res->ai_family, res->ai_socktype,
		res->ai_protocol);
	if (sockfd < 0)
	continue; /* ignore this one */
	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
	break; /* success */
		close(sockfd); /* ignore this one */
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL) { /* errno set from final connect() */
		fprintf(stderr, "tcp_connect error for %s, %s\n", servName, servPort);
		sockfd = -1;
	} else {
		printf("connect to server success\n");
	}
//print_address(res);
freeaddrinfo(ressave);
return(sockfd);
}
