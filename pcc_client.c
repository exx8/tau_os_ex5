#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <assert.h>


void check_args(int argc) {
    if (argc != 4) {
        fprintf(stderr, "invalid num of args\n");
    }
    exit(1);
}

void *readEntireFile(int * fileDescriptor,char *path, unsigned int *length) {
    *fileDescriptor = open(path, O_RDONLY);
    *length = lseek(fileDescriptor, 0, SEEK_END);
    return mmap(0, length, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);
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
        assert(nsent >= 0);

        totalsent += nsent;
        notwritten -= nsent;
    }
}

int main(int argc, char **argv) {
    check_args(argc);
    struct in_addr ip;
    inet_aton(argv[1], &ip);
    unsigned int port = argv[2];
    char *path = argv[3];
    int length;
    int fileDescriptor;
    void *data_buf = readEntireFile(&fileDescriptor,path, length);


    int confd = create_socket(&ip, port);
    int lenBuf = htonl(length);
    sendData(&lenBuf, confd, sizeof(lenBuf));
    sendData(data_buf, confd, length);

    // close socket
    close(confd);

}

