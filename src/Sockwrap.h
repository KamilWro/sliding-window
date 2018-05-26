// Kamil Breczko (280 990)

#ifndef SOCKWRAP_H
#define SOCKWRAP_H

#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <stdexcept>
#include <netinet/ip_icmp.h>

using namespace std;

int Socket(int family, int type, int protocol);

size_t RecvFrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr *src_addr, socklen_t *addrlen);

void SendTo(int fd, const void *ptr, int nbytes, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

void Close(int fd);

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

void Inet_ntop(int af, const void *src, char *dst, socklen_t size);

void Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

void Inet_pton(int af, const char *src, void *dst);

#endif