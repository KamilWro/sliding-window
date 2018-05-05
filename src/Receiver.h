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
    Packet readBuffor(sockaddr_in sender, char *buffer, ssize_t size);

public:
    Packet receive(int sockfd);
};

#endif
