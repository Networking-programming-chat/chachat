
#include "serv.h"
#include <stdio.h>
#include <sys/socket.h> //socket.
#include <strings.h>  //bzero
#include <netdb.h> // addrinfo
#include <unistd.h> //close


 /*This function is borrowed from example*/
int serv_listen(const char *host, const char *serv, socklen_t *addrlenp){
        int listenfd, n;
        const int on = 1;
        struct addrinfo hints, *res, *ressave;

        bzero(&hints, sizeof(struct addrinfo));
        hints.ai_flags = AI_PASSIVE;
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0) {
                fprintf(stderr, "tcp_listen error for %s, %s: %s",
                                 host, serv, gai_strerror(n));
		return -1;
	}
        ressave = res;

        do {
                listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
                if (listenfd < 0)
                        continue;               /* error, try next one */

                setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
                if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
                        break;                  /* success */

                close(listenfd);        /* bind error, close and try next one */
        } while ( (res = res->ai_next) != NULL);

        if (res == NULL) {        /* errno from final socket() or bind() */
                fprintf(stderr, "tcp_listen error for %s, %s", host, serv);
		return -1;
	}

        if (listen(listenfd, LISTENQ) < 0) {
		perror("listen");
		return -1;
	}

        if (addrlenp)
                *addrlenp = res->ai_addrlen;    /* return size of protocol address */

        freeaddrinfo(ressave);

        return(listenfd);
}