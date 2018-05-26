// Kamil Breczko (280 990)

#ifndef TRANSPORT_TRANSPORT_H
#define TRANSPORT_TRANSPORT_H

#include <arpa/inet.h>
#include <iomanip>
#include <unistd.h>
#include <iostream>
#include <cstring>

#include "Receiver.h"
#include "Sockwrap.h"
#include "Window.h"

using namespace std;

class Transport {
    const uint32_t maxDataLength = 1000;
    int sockfd;
    uint32_t port;
    string ipAddr;
    struct sockaddr_in serverAddress;

    Window window;

    void sendPackets(uint32_t bytesWrittenCount, uint32_t sizeFile);

    bool isNotFullSegment(uint32_t start, uint32_t sizeFile);

    uint32_t receivePackets(uint32_t bytesWrittenCount, uint32_t bytesReceivedCount, uint32_t sizeFile);

    bool validatePacket(Packet packet, uint32_t bytesWrittenCount, uint32_t sizeFile);

    bool isFittedToWindow(uint32_t start, uint32_t bytesWrittenCount);

    bool isCorrectDataLength(uint32_t start, uint32_t length, string data, uint32_t sizeFile);

    void printToConsole(uint32_t bytesReceivedCount, uint32_t sizeFile);

public:

    Transport(uint32_t port, string ipAddr);

    void download(string nameFile, uint32_t sizeFile);

    ~Transport();
};

#endif
