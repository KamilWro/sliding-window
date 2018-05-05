// Kamil Breczko (280 990)

#include "Receiver.h"

Packet Receiver::readBuffor(struct sockaddr_in sender, char *buffer, ssize_t size) {
    Packet packet;

    const char *dst = inet_ntop(AF_INET, &sender.sin_addr, packet.ipAddr, INET_ADDRSTRLEN);
    if (dst == NULL)
        throw runtime_error(string("inet_ntop error: ") + strerror(errno));

    packet.port = ntohs(sender.sin_port);
    packet.data = string(buffer, size);

    return packet;
}

Packet Receiver::receive(int sockfd) {
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    uint8_t buffer[IP_MAXPACKET + 1];

    ssize_t size = recvfrom(sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr *) &sender, &sender_len);
    if (size < 0)
        throw runtime_error(string("recvfrom error: ") + strerror(errno));

    return readBuffor(sender, reinterpret_cast<char *>(buffer), size);
}