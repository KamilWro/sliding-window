// Kamil Breczko (280 990)

#include "Receiver.h"
#include "Sockwrap.h"

Packet Receiver::receivePacket(int sockfd) {
    struct sockaddr_in sender{};
    socklen_t sender_len = sizeof(sender);
    char buffer[IP_MAXPACKET + 1];

    size_t size = RecvFrom(sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr *) &sender, &sender_len);
    return readBuffer(sender, buffer, size);
}

Packet Receiver::readBuffer(struct sockaddr_in sender, char *buffer, size_t size) {
    Packet packet;
    Inet_ntop(AF_INET, &sender.sin_addr, packet.ipAddr, INET_ADDRSTRLEN);
    packet.port = ntohs(sender.sin_port);
    packet.data = string(buffer, size);
    return packet;
}