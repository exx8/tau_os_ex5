#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>


void check_args(int argc) {
    if (argc != 4) {
        fprintf(stderr, "invalid num of args\n");
    }
    exit(1);
}

void *readEntireFile(char *path, unsigned int *length) {
    int fileDescriptor = open(path, O_RDONLY);
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
}


int main(int argc, char **argv) {
    check_args(argc);
    struct in_addr ip;
    inet_aton(argv[1], &ip);
    unsigned int port = argv[2];
    char *path = argv[3];
    int length;
    readEntireFile(path, length);


    create_socket(&ip, port);


}

