#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include "errno.h"

#define LOWER_LIMIT 32
#define UPPER_LIMIT 126
static unsigned int pcc_total[128];
static int volatile shouldIContinue = 1;
static int sickConnection = 0;

void err_handler(int status) {
    int err = errno;
    if (status < 0) {
        perror(NULL);

        switch (err) {
            case ETIMEDOUT:
            case ECONNRESET:
            case EPIPE:
            case ENOTCONN:
                sickConnection = 1;
                break;
            default:
                exit(1);

        }

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
    if (argc != 2) {
        fprintf(stderr, "invalid num of args\n");
        exit(1);

    }
}

int create_socket(struct sockaddr_in *sin2, unsigned int port) {
    int s = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    int value = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    connect(s, (const struct sockaddr *) sin2, sizeof(sin2));
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

void readData(void *data_buf, int confd, int notRead) {
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

void cntrlc() {
    shouldIContinue = 0;
    printf("halts/n");
}

int main(int argc, char **argv) {

    check_args_server(argc);
    signal(SIGINT, cntrlc);

    unsigned int port = htons(atoi(argv[1]));
    struct sockaddr_in sin2;
    sin2.sin_family = AF_INET;
    // INADDR_ANY = any local machine address
    sin2.sin_addr.s_addr = htons(INADDR_ANY);
    sin2.sin_port = (port);
    int s = create_socket(&sin2, port);

    err_handler(bind(s, (struct sockaddr *) &sin2, sizeof(sin2)));
    err_handler(listen(s, 10));
    unsigned int numOfPrintable = 0;
    struct sockaddr_in peerAddress;
    socklen_t len = (socklen_t) sizeof(peerAddress);
    while (shouldIContinue) {
        sickConnection = 0;// no problems so far with connection.
        const int status = accept(s, (struct sockaddr *) &peerAddress, &len);
        if (status == -1 && errno == EAGAIN)
            continue;
        err_handler(status);
        unsigned int length;
        readData(&length, s, sizeof(length));
        char string2process[length];
        readData(&string2process, s, sizeof(char) * length);
        char * currentString=&string2process;
        while (!sickConnection && length > 0) {
            char c=currentString;
            if (isPrintable(c)) {
                numOfPrintable++;
                pcc_total[c]++;
            }
            length--;
            currentString++;
        }
        sendData(&numOfPrintable, s, sizeof numOfPrintable);
    }
    for (int k = LOWER_LIMIT; k <= UPPER_LIMIT; k++) {
        printf("char '%c' : %u times\n", k, pcc_total[k]);
    }
    close(s);
    return 0;
}