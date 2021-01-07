#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <sys/mman.h>

#define LOWER_LIMIT 32
#define UPPER_LIMIT 126

static unsigned int pcc_total[128];
static shouldIContinue = 1;

void err_handler(int status) {
    if (status < 0) {
        perror(NULL);
        exit(1);
    }
}

int isPrintable(int b) {
    if (b < LOWER_LIMIT)
        return 0;
    if (b > UPPER_LIMIT)
        return 0;
    return 1;
}

void check_args_server(int argc) {
    if (argc != 1) {
        fprintf(stderr, "invalid num of args\n");
    }
    exit(1);
}

int create_socket(struct in_addr *ip, struct sockaddr_in *sin2, unsigned int port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin = {
            .sin_family = AF_INET,
            .sin_port = port,
            .sin_addr = (*ip)
    };
    *sin2 = sin;
    connect(s, &sin, sizeof(sin));
    return s;
}


void sendData(const void *data_buf, int confd, int notwritten) {
    int totalsent = 0;
    // keep looping until nothing left to write
    while (notwritten > 0) {
        // notwritten = how much we have left to write
        // totalsent  = how much we've written so far
        // nsent = how much we've written in last write() call */
        int nsent = write(confd,
                          data_buf + totalsent,
                          notwritten);
        // check if error occured (client closed connection?)
        err_handler(nsent);

        totalsent += nsent;
        notwritten -= nsent;
    }
}

void readData(const void *data_buf, int confd, int notRead) {
    int totalsent = 0;
    // keep looping until nothing left to write
    while (notRead > 0) {
        // notRead = how much we have left to write
        // totalsent  = how much we've written so far
        // nsent = how much we've written in last write() call */
        int nsent = read(confd,
                         data_buf + totalsent,
                         notRead);
        // check if error occured (client closed connection?)

        err_handler(nsent);

        totalsent += nsent;
        notRead -= nsent;
    }
}

int main(int argc, char **argv) {
    check_args_server(argc);
    unsigned int port = htonl(argv[1]);
    struct sockaddr_in sin2;
    int s = create_socket(htonl(INADDR_ANY), &sin2, port);
    err_handler(bind(s, (struct sockaddr *) &sin2, sizeof(sin2)));
    err_handler(listen(s, 10));
    while (shouldIContinue) {
        unsigned int numOfPrintable = 0;
        struct sockaddr peerAddress;
        err_handler(accept(s, &peerAddress, sizeof(peerAddress)));
        unsigned int length;
        readData(&length, s, sizeof(length));
        while (length > 0) {
            char c;
            err_handler(read(s, &c, sizeof(c)));
            if (isPrintable(c)) {
                numOfPrintable++;
                pcc_total[c]++;
            }
        }
        sendData(&numOfPrintable, s, sizeof numOfPrintable);
    }
    for(int k=LOWER_LIMIT;k<=UPPER_LIMIT;k++)
    {
        printf("char '%c' : %u times\n",,);
    }
    close(s);
    return 0;
}