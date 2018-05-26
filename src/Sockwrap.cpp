// Kamil Breczko (280 990)

#include "Sockwrap.h"

int Socket(int family, int type, int protocol) {
    int n;
    if ((n = socket(family, type, protocol)) < 0)
        throw runtime_error(string("socket error: ") + strerror(errno) + " \n");
    return n;
}

size_t RecvFrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
    ssize_t n = recvfrom(fd, ptr, nbytes, flags, src_addr, addrlen);
    if (n < 0)
        throw runtime_error(string("recvfrom error: ") + strerror(errno) + " \n");
    return n;
}

void SendTo(int fd, const void *ptr, int nbytes, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) {
    ssize_t bytesSent = sendto(fd, ptr, nbytes, flags, dest_addr, addrlen);
    if (bytesSent < nbytes)
        throw runtime_error(string("send error: ") + strerror(errno) + " \n");
}

void Close(int fd) {
    if (close(fd) < 0)
        throw runtime_error(string("close error: ") + strerror(errno) + " \n");
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
    int n;
    if ((n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
        throw runtime_error(string("select error: ") + strerror(errno) + " \n");
    return n;
}

void Inet_ntop(int af, const void *src, char *dst, socklen_t size) {
    const char *res = inet_ntop(af, src, dst, size);
    if (res == nullptr)
        throw runtime_error(std::string("inet_ntop error: ") + strerror(errno));
}

void Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    int result = setsockopt(sockfd, level, optname, optval, optlen);

    if (result < 0)
        throw invalid_argument(std::string("setsockopt error: ") + strerror(errno));
}

void Inet_pton(int af, const char *src, void *dst) {
    int address = inet_pton(af, src, dst);

    if (address == 0)
        throw std::invalid_argument("error: incorrect IPv4 address");

    if (address == -1)
        throw std::invalid_argument(std::string("inet_pton error:") + strerror(errno));
}
