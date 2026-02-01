#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int sockInit(char *address, char *port, int family, int socktype, int flags, struct addrinfo **servAddr);
int createSocketAndBind(struct addrinfo *servAddr);
int sockSetSockOpt(int sockFd, int level, int optname, int enable);
int sockListen(int sockFd, int backlog);
int sockAccept(int sockFd, struct sockaddr_storage *clientAddr, socklen_t *clientAddrSize);
char* toIPv4(struct sockaddr_storage *sockAddr);

#endif

