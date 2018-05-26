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
    string message = string(buffer, size);
    packet.port = ntohs(sender.sin_port);
    packet.start = extractStart(message);
    packet.length = extractLength(message);
    packet.data = extractData(message);
    return packet;
}

uint32_t Receiver::extractStart(string data) {
    data = data.substr(0, data.find('\n'));
    data = data.substr(data.find(' ') + 1);
    return stoi(data);
}

uint32_t Receiver::extractLength(string data) {
    data = data.substr(0, data.find('\n'));
    data = data.substr(data.find(' ') + 1);
    data = data.substr(data.find(' ') + 1);
    return stoi(data);
}

string Receiver::extractData(string data) {
    auto pos = data.find('\n');
    if (pos == string::npos)
        throw runtime_error("Invalid data received.");

    return data.substr(pos + 1);
}
