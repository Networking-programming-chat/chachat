//
//  test_client_main.c
//  Chachat all copyright reserved
//
#include "client.h"

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
    char buffer[MAXMSG+1];
    Msgheader header;
    
    char buf[20];
    
    id = (thread_s*) arg;
    sockfd = id->socketfd;
    
    //printf("reader socket: %d\n", id->socketfd);
    // Read socket in a loop
    for (;;) {
       // n = read(sockfd, buffer, 1023);
        memset(buffer,0,sizeof(buffer));
        
        if ((n = read_message(sockfd,buffer,&header)) > 0){  
            //memset(buf,0,sizeof(buf));
            get_timestamp(buf);
            printf(" x%sx %s<%s:> ",buf,COLOR_GRN,header.sender_id);
            puts(buffer);
        }else{
            //perror("read error");
            break;
        }
    }
    
    //printf("Reader thread finished\n");
    
    pthread_exit(arg);
}

int handletask(int mainsockfd)
{
    // This method handles the task after connection is established
    //char recvline[MAXLINE + 1];
    char nick[128];
    ssize_t n;
    thread_s *thread;
    void *thread_result;
    
    thread = malloc(sizeof(thread_s));
    
    printChatRule();
    
    //set the nick
    if(client_nick(mainsockfd,nick)<0){
        perror("There is problem writing nickname to the server\n");
        return -1;
    }
    
    // TODO: message receive and send
    
    // Spawn listener thread to deal with message from the server
    thread->socketfd = mainsockfd;
    n = pthread_create(&(thread->thread_id), NULL, reader_thread, (void*)thread);
    check((int)n, "thread creation failed");
    
    //handles sending both chat and channel messages.
    send_message(mainsockfd,nick);
    
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
