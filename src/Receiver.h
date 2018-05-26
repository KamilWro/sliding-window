// Kamil Breczko (280 990)

#ifndef TRANSPORT_RECEIVER_H
#define TRANSPORT_RECEIVER_H

#include <netinet/ip.h>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include "Packet.h"

using namespace std;

class Receiver {
    Packet readBuffer(sockaddr_in sender, char *buffer, size_t size);

public:
    Packet receivePacket(int sockfd);
};

#endif
