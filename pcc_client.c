#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

void err_handler(int status) {
    if (status < 0) {
        perror(NULL);
        exit(1);
    }
}

void check_args(int argc) {
    if (argc != 4) {
        fprintf(stderr, "invalid num of args\n");
        exit(1);

    }
}

void *readEntireFile(int *fileDescriptor, char *path, unsigned int *length) {
    *fileDescriptor = open(path, O_RDONLY);
    err_handler(*fileDescriptor);
    __off_t length2 = lseek(*fileDescriptor, 0, SEEK_END);
    *length = length2;
    void *returnValue = mmap(0, *length, PROT_READ, MAP_PRIVATE, *fileDescriptor, 0);
    if (length2>0 && returnValue == (void *) (-1))
        err_handler(-1);
    return returnValue;
}

int create_socket(struct in_addr *ip, unsigned int port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = (port); // Note: htons for endiannes
    sin.sin_addr.s_addr = ip->s_addr;
    err_handler(connect(s, (struct sockaddr *) &sin, sizeof(sin)));
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

int main(int argc, char **argv) {
    check_args(argc);
    struct in_addr ip;
    inet_aton(argv[1], &ip);
    unsigned int port = htons(atoi(argv[2]));
    char *path = argv[3];
    unsigned int length;
    int fileDescriptor;
    void *data_buf = readEntireFile(&fileDescriptor, path, &length);


    int confd = create_socket(&ip, port);
    int lenBuf = htonl(length);//#todo the problem is probably here, should it be swapped? overflow detected
    sendData(&lenBuf, confd, sizeof(lenBuf));
    sendData(data_buf, confd, length);
    unsigned int readableNum;
    readData(&readableNum, confd, sizeof(readableNum));
    readableNum=ntohl(readableNum);
    printf("# of printable characters: %u\n", readableNum);
    // close socket
    close(confd);
    close(fileDescriptor);
    return 0;
}

