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
void * readEntireFile(char* fileName)
{
    int fileDescriptor = open(fileName, O_RDONLY);
    int length = lseek(fileDescriptor, 0, SEEK_END);
    return  mmap(0, length, PROT_READ, MAP_PRIVATE, fileDescriptor, 0);
}
int main(int argc, char **argv) {
    check_args(argc);
    struct in_addr ip;
    inet_aton(argv[1], &ip);
    unsigned int port = argv[2];
    char *path = argv[3];


}