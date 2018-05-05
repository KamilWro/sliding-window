// Kamil Breczko (280 990)

#ifndef TRANSPORT_SENDER_H
#define TRANSPORT_SENDER_H

#include <netinet/in.h>
#include <string>
#include <iostream>
#include <cstring>

using namespace std;

class Sender {
public:
    void send(int sockfd, struct sockaddr_in server_address, string message);
};

#endif
