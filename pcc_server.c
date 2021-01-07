#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static unsigned int pcc_total[128];

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

int main(int argc, char **argv) {
    check_args_server(argc);
    unsigned int port = argv[1];


}