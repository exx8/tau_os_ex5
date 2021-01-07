#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <sys/mman.h>

static unsigned int pcc_total[128];

void err_handler(int status) {
    if (status < 0) {
        perror(NULL);
        exit(1);
    }
}

int isPrintable(int b) {
    if (b < 32)
        return 0;
    if (b > 126)
        return 0;
    return 1;
}

void check_args_server(int argc) {
    if (argc != 1) {
        fprintf(stderr, "invalid num of args\n");
    }
    exit(1);
}

int create_socket(struct in_addr *ip, unsigned int port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin = {
            .sin_family = AF_INET,
            .sin_port = port,
            .sin_addr = (*ip)
    };
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
    unsigned int port = argv[1];


}