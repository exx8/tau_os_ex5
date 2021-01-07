#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void check_args(int argc) {
    if (argc != 4) {
        fprintf(stderr, "invalid num of args\n");
    }
    exit(1);
}

int main(int argc, char **argv) {
    check_args(argc);
    struct in_addr ip;
    inet_aton(argv[1], &ip);

}