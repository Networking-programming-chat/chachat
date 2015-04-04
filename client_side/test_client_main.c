//
//  test_client_main.c
//  Chachat
//

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include <termios.h>
#include <unistd.h>

#include <pthread.h>
#include <signal.h>

#include <errno.h>
#include <stdarg.h> // va_ args
#include <stdlib.h> // exit()

#define MAXLINE 80
#define CONNECTION_AMOUNT 10
#define BUFSIZE 1024
#define THREAD_COUNT 128

typedef struct {
    pthread_t thread_id;
    int socketfd;
} thread_s;

struct termios orig_termios;

/*void sig_handler(int signo) {
 printf("-- signal --\n");
 }*/

// Function to test a condition
static void check (int test, const char * message, ...)
{
    if (test) {
        va_list args;
        
        // Iterate the arguments
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        
        // Print error message to stderr
        fprintf (stderr, ": %s\n", strerror(errno));
        
        // Failure
        exit (1);
    }
}

// Function to test a condition
static void checkneg (int test, const char * message, ...)
{
    if (test < 0) {
        va_list args;
        
        // Iterate the arguments
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        
        // Print error message to stderr
        fprintf (stderr, ": %s\n", strerror(errno));
        
        // Failure
        exit (1);
    }
}

void cleanup_handler()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void cbc_mode()
{
    struct termios cbc_termios;
    
    // Remember original terminal settings
    tcgetattr(0, &orig_termios);
    memcpy(&cbc_termios, &orig_termios, sizeof(cbc_termios));
    
    // register cleanup handler
    atexit(cleanup_handler);
    
    // Set character by character terminal mode
    //cfmakeraw(&cbc_termios);
    cbc_termios.c_lflag &= ~(ECHO|ICANON);
    cbc_termios.c_cc[VTIME] = 0;
    cbc_termios.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &cbc_termios);

}

int check_keyboard()
{
    struct timeval tv = { 0L, 200L * 1000000L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int get_character()
{
    int r;
    unsigned char c;
    if ((r = (int)read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

void *reader_thread(void *arg) {
    thread_s *id;
    int sockfd;
    ssize_t n;
    char buffer[1024];
    
    id = (thread_s*) arg;
    sockfd = id->socketfd;
    
    printf("reader socket: %d\n", id->socketfd);
    
    // Read socket in a loop
    for (;;) {
        n = read(sockfd, buffer, 1023);
        if (n < 0) {
            perror("read error");
            break;
        }
        
        if (n > 0) {
            // Write to stdout
            printf("%s", buffer);
        }
    }
    
    printf("Reader thread finished\n");
    
    pthread_exit(arg);
}

int handletask(int mainsockfd)
{
    // This method handles the task after connection is established
    char recvline[MAXLINE + 1], nick[128];
    ssize_t n;
    thread_s *thread;
    void *thread_result;
    
    thread = malloc(sizeof(thread_s));
    
    // Ask for nick
    for (;;)
    {
        printf("Enter nick:\n");
        fgets(nick, 127, stdin);
        
        n = strlen(nick);
        
        n = write(mainsockfd, nick, n);
        checkneg((int)n, "write error");
        
        // Read data
        n = read(mainsockfd, recvline, MAXLINE);
        checkneg((int)n, "read error");
        recvline[n] = '\0'; // Terminate
        
        // print
        printf("%s", recvline);
        
        // Check the message
        if (strncmp("OK", recvline, 2) == 0) {
            printf("Received OK\n");
            break;
        }
        else {
            printf("Fail, try again\n");
            continue;
        }
    }
    
    // TODO: message receive and send
    
    // Spawn listener thread to receive messages
    thread->socketfd = mainsockfd;
    n = pthread_create(&(thread->thread_id), NULL, reader_thread, (void*)thread);
    check((int)n, "thread creation failed");
    
    // Send messages
    for (;;) {
        fgets(nick, 127, stdin);
        
        n = strlen(nick);
        //printf("message length: %zd\n", n);
        
        if (n == 1) {
            // Only newline
            continue;
        }
        
        // Set the newline to 0
        nick[n-1] = '\0';
        
        if (strncmp(nick, "quit", 4) == 0) {
            break;
        }
        
        n = write(mainsockfd, nick, n);
        checkneg((int)n, "write error");
    }
    
    printf("Done\n");
    
    n = pthread_join(thread->thread_id, &thread_result);
    check((int)n, "thread join failed");
    
    
    return 0;
}

int main(int argc, char **argv)
{
    int status, mainsockfd;
    struct addrinfo info, *res, *firstres;
    
    // Get arrd info
    memset(&info, 0, sizeof(struct addrinfo));
    info.ai_family = AF_UNSPEC;
    info.ai_socktype = SOCK_STREAM;
    
    // Check arguments
    if (argc != 3) {
        printf("Usage: %s [server] [port]\n", argv[0]);
        return 0;
    }
    
    status = getaddrinfo(argv[1], argv[2], &info, &res);
    checkneg(status, "getaddrinfo error");
    
    // Remember first for freeaddrinfo
    firstres = res;
    
    do {
        mainsockfd = socket(res->ai_family, res->ai_socktype,
                            res->ai_protocol);
        if (mainsockfd < 0)
            continue;
        
        printf("Got %s address\n", res->ai_family == AF_INET ? "IPv4" : "IPv6");
        
        // Connect to IP
        status = connect(mainsockfd, res->ai_addr, res->ai_addrlen);
        checkneg(status, "connect error");
        
        // Handle the task
        handletask(mainsockfd);
        
        // Close when done
        status = close(mainsockfd);
        checkneg(status, "socket close error");
        
        break;
        
    } while ( (res = res->ai_next) != NULL);
    
    freeaddrinfo(firstres);
    
    // TODO: Join all threads?
    
    return 0;
}

