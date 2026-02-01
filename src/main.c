#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>

#include <poll.h>

#include "argparse/argparse.h"
#include "socket/socket.h"

typedef struct {
    char* port;
    char* address;
    bool  help;
} Args;

int main(int argc, char** argv) {
    int ret;
    Args args;
    memset(&args, 0, sizeof(args));

    CliElement cliElements[] = {
        NEW_CLI_SWITCH(STRING, 'a', "address", &args.address),
        NEW_CLI_SWITCH(ENABLE, 'h', "help", &args.help),
        NEW_CLI_ARG(STRING, &args.port),
    };

     ret = parse(
        argc,
        argv,
        cliElements,
        sizeof(cliElements) / sizeof(cliElements[0])
    );

    if (args.help || args.port == NULL || ret == -1 || argc < 2) {
        printf("Usage: %s <port> [-a/--address <IPv4>]\n", argv[0]);
        printf("\nOptions:\n");
        printf("-a\t--address <IPv4>\tSet a custom interface IPv4\n");
        printf("\nExamples:\n");
        printf("%s 4444\n", argv[0]);
        printf("%s 4444 -a 127.0.0.1\n", argv[0]);
        return -1;
    }

    struct sockaddr_storage clientAddr;
    struct addrinfo *servInfo;

    socklen_t clientAddrSize = sizeof(clientAddr);

    ret = sockInit(
        args.address,
        args.port,
        AF_INET,
        SOCK_STREAM,
        AI_PASSIVE,
        &servInfo
    );

    if (ret != 0) {
        perror("ERROR on sockInit\n");
        return ret;
    }

    int serverFd = createSocketAndBind(servInfo);

    if (serverFd == -1) {
        perror("ERROR on createSocketAndBind\n");
        return serverFd;
    }

    ret = sockSetSockOpt(serverFd, SOL_SOCKET, SO_REUSEADDR, 1);

    if (ret != 0) {
        perror("ERROR on sockSetSockOpt\n");
        return ret;
    }

    ret = sockListen(serverFd, -1);

    if (ret != 0) {
        perror("ERROR on sockListen\n");
        return ret;
    }

    int clientFd = sockAccept(serverFd, &clientAddr, &clientAddrSize);

    if (clientFd == -1) {
        perror("ERROR on listen\n");
        return ret;
    }

    char* ipv4 = toIPv4(&clientAddr);
    printf("NEW CONNECTION FROM %s\n", ipv4);

    struct pollfd fds[2];
    fds[0].fd       = STDIN_FILENO;
    fds[0].events   = POLLIN;

    fds[1].fd       = clientFd;
    fds[1].events   = POLLIN;

    char buf[1024];

    while (1) {
        ret = poll(fds, 2, -1);
        if (ret <= 0) continue;

        if (fds[0].revents & POLLIN) {
            memset(buf, 0, 1024);
            fgets(buf, 1024, stdin);
            send(clientFd, buf, 1024, 0);
        }

        if (fds[1].revents & POLLIN) {
            memset(buf, 0, 1024);
            recv(clientFd, buf, 1024, 0);
            printf("%s", buf);
        }
    }

    close(clientFd);
    close(serverFd);

    freeaddrinfo(servInfo);

    return 0;
}
