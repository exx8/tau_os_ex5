#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void check_args_server(int argc) {
    if (argc != 1) {
        fprintf(stderr, "invalid num of args\n");
    }
    exit(1);
}

int main(int argc, char **argv) {
    check_args_server(argc);
    unsigned int port = argv[1];


}