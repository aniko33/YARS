#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int ret;

int sockInit(char *address, char *port, int family, int socktype, int flags, struct addrinfo **servAddr) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family      = family;
    hints.ai_socktype    = socktype;
    hints.ai_flags       = flags;

    ret = getaddrinfo(
        address,
        port,
        &hints,
        servAddr
    );

    return ret;
}

int createSocketAndBind(struct addrinfo *servAddr) {
    int s = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);

    if (s == -1) {
        perror("ERROR on socket\n");
        return s;
    }

    ret = bind(s, servAddr->ai_addr, servAddr->ai_addrlen);

    if (ret != 0) {
        perror("ERROR on bind\n");
        return ret;
    }

    return s;
}

int sockSetSockOpt(int sockFd, int level, int optname, int enable) {
    ret = setsockopt(
        sockFd,
        level,
        optname,
        &enable,
        sizeof(enable)
    );

    if (ret != 0) {
        perror("ERROR on setsockopt\n");
    }

    return ret;
}

int sockListen(int sockFd, int backlog) {
    return listen(sockFd, backlog);
}

int sockAccept(int sockFd, struct sockaddr_storage *clientAddr, socklen_t *clientAddrSize) {
    return accept(sockFd, (struct sockaddr*)clientAddr, clientAddrSize);
}

char* toIPv4(struct sockaddr_storage *sockAddr) {
    return inet_ntoa(((struct sockaddr_in*)sockAddr)->sin_addr);
}
