// Kamil Breczko (280 990)

#ifndef TRANSPORT_TRANSPORT_H
#define TRANSPORT_TRANSPORT_H

#include <arpa/inet.h>
#include <iomanip>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <deque>
#include "OutputFile.h"
#include "Sender.h"
#include "Receiver.h"

using namespace std;

class Transport {
    const uint32_t MAX_SEGMENT_SIZE = 1000;
    int sockfd;
    int port;
    string ipAddr;
    struct sockaddr_in serverAddress;

    deque<string> segments;
    const uint32_t senderWindowSize = 500;

    void removeSegments();

    void sendPackets(uint32_t bytesWrittenCount, uint32_t sizeFile);

    void receivePackets(uint32_t bytesWrittenCount, uint32_t &bytesReceivedCount, uint32_t sizeFile);

    bool isFittedToWindow(uint32_t start, uint32_t bytesWrittenCount);

    bool isCorrectSizeOfData(uint32_t start, uint32_t sizeData, string data, uint32_t sizeFile);

    void printToConsole(uint32_t bytesReceivedCount, uint32_t sizeFile);

    uint32_t writeToFile(OutputFile &outputFile);

public:

    Transport(int port, string ipAddr);

    void download(string nameFile, uint32_t sizeFile);

    virtual ~Transport();
};

#endif
