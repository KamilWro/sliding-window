// Kamil Breczko (280 990)

#ifndef TRANSPORT_PACKET_H
#define TRANSPORT_PACKET_H

#include <iostream>
#include <netinet/in.h>

using namespace std;

typedef struct packet {
    char ipAddr[INET_ADDRSTRLEN];
    uint16_t port;
    string data;
} Packet;

#endif
