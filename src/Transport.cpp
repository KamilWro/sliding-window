// Kamil Breczko (280 990)

#include "Transport.h"

Transport::Transport(uint32_t port, string ipAddr) : port(port), ipAddr(ipAddr) {
    sockfd = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    Inet_pton(AF_INET, ipAddr.c_str(), &serverAddress.sin_addr);
    Connect(sockfd, &serverAddress, sizeof(serverAddress));
}

void Transport::download(string nameFile, uint32_t sizeFile) {
    OutputFile outputFile(move(nameFile));

    uint32_t bytesReceivedCount = 0;
    uint32_t bytesWrittenCount = 0;

    while (bytesWrittenCount < sizeFile) {
        sendPackets(bytesWrittenCount, sizeFile);
        bytesReceivedCount += receivePackets(bytesWrittenCount, bytesReceivedCount, sizeFile);
        bytesWrittenCount += window.writeToFile(outputFile);
    }

    cout << "Received " << bytesWrittenCount << " bytes." << endl;
}


void Transport::sendPackets(uint32_t bytesWrittenCount, uint32_t sizeFile) {
    uint32_t start = bytesWrittenCount;

    for (uint32_t i = 0; i < window.size() && start < sizeFile; i++, start += maxDataLength) {
        if (window.isEmpty(i)) {
            uint32_t sgmtSize = isNotFullSegment(start, sizeFile) ? sizeFile % maxDataLength : maxDataLength;
            string message = "GET " + to_string(start) + " " + to_string(sgmtSize) + "\n";
            SendTo(sockfd, message.c_str(), message.length(), 0, &serverAddress, sizeof(serverAddress));
        }
    }
}

bool Transport::isNotFullSegment(uint32_t start, uint32_t sizeFile) {
    return (sizeFile % maxDataLength) != 0 && (sizeFile - start) < maxDataLength;
}

uint32_t Transport::receivePackets(uint32_t bytesWrittenCount, uint32_t bytesReceivedCount, uint32_t sizeFile) {
    const uint32_t maxBytesCount = bytesWrittenCount + WINDOW_SIZE * maxDataLength;
    uint32_t bytes = 0;
    Receiver receiver;

    while (bytesReceivedCount + bytes < maxBytesCount) {
        fd_set descriptors;
        FD_ZERO (&descriptors);
        FD_SET (sockfd, &descriptors);

        struct timeval tv{};
        tv.tv_sec = 0;
        tv.tv_usec = 200;

        int ready = Select(sockfd + 1, &descriptors, nullptr, nullptr, &tv);

        if (ready == 0)
            break;

        Packet packet{};

        try {
            packet = receiver.receivePacket(sockfd);
        } catch (const exception &e) {
            cerr << "Invalid header was received" << endl;
        }

        if (validatePacket(packet, bytesWrittenCount, sizeFile)) {
            uint32_t id = (packet.start - bytesWrittenCount) / maxDataLength;
            if (window.isEmpty(id)) {
                bytes += packet.length;
                window.setData(id, packet.data);
                printToConsole(bytesReceivedCount + bytes, sizeFile);
            }
        }
    }

    return bytes;
}

bool Transport::validatePacket(Packet packet, uint32_t bytesWrittenCount, uint32_t sizeFile) {
    return packet.ipAddr == ipAddr && packet.port == port && isFittedToWindow(packet.start, bytesWrittenCount) &&
           isCorrectDataLength(packet.start, packet.length, packet.data, sizeFile);
}

bool Transport::isFittedToWindow(uint32_t start, uint32_t bytesWrittenCount) {
    return (start % maxDataLength == 0) && bytesWrittenCount <= start &&
           start < (bytesWrittenCount + WINDOW_SIZE * maxDataLength);
}

bool Transport::isCorrectDataLength(uint32_t start, uint32_t length, string data, uint32_t sizeFile) {
    return data.size() == length && (length == maxDataLength || isNotFullSegment(start, sizeFile));
}

void Transport::printToConsole(uint32_t bytesReceivedCount, uint32_t sizeFile) {
    double percent = (bytesReceivedCount * 100.0) / sizeFile;
    percent = percent > 100 ? 100 : percent;
    cout << "[" << setw(50) << left << string(static_cast<unsigned long>(percent / 2.0), '=') << "]";
    cout << " downloaded: " << percent << "%" << endl << endl;
}

Transport::~Transport() {
    Close(sockfd);
}