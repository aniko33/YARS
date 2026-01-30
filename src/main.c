#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <poll.h>

#include "argparse/argparse.h"

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

    struct addrinfo hints;
    struct addrinfo *servInfo;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_flags      = AI_PASSIVE;

    ret = getaddrinfo(
        args.address,
        args.port,
        &hints,
        &servInfo
    );

    if (ret != 0) {
        perror("ERROR on getaddrinfo\n");
        return ret;
    }

    int s = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);

    if (s == -1) {
        perror("ERROR on socket\n");
        return s;
    }

    ret = bind(s, servInfo->ai_addr, servInfo->ai_addrlen);

    if (ret != 0) {
        perror("ERROR on bind\n");
        return ret;
    }

    {
        int yes = 1;
        ret = setsockopt(
            s,
            SOL_SOCKET,
            SO_REUSEADDR,
            &yes,
            sizeof(yes)
        );
    }

    if (ret != 0) {
        perror("ERROR on setsockopt\n");
        return ret;
    }

    ret = listen(s, -1); 

    if (ret != 0) {
        perror("ERROR on listen\n");
        return ret;
    }

    struct sockaddr_storage clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int sClient = accept(s, (struct sockaddr*)&clientAddr, &clientAddrSize); 

    if (sClient == -1) {
        perror("ERROR on listen\n");
        return ret;
    }

    char* ipv4 = inet_ntoa(((struct sockaddr_in*)&clientAddr)->sin_addr);
    printf("NEW CONNECTION FROM %s\n", ipv4);

    char buf[1024];
    while (1) {
        memset(buf, 0, 1024);

        fgets(buf, 1024, stdin);
        send(sClient, buf, 1024, 0);

        memset(buf, 0, 1024);

        recv(sClient, buf, 1024, 0);
        printf("%s", buf);
    }

    freeaddrinfo(servInfo);

    return 0;
}
