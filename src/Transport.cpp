// Kamil Breczko (280 990)

#include "Transport.h"
#include "Sockwrap.h"

Transport::Transport(uint32_t port, string ipAddr) : port(port), ipAddr(ipAddr) {
    if (port > 0xFFFF)
        throw invalid_argument("port must be between 0 and 65535 \n");

    sockfd = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    Inet_pton(AF_INET, ipAddr.c_str(), &serverAddress.sin_addr);

    if (connect(sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
        throw runtime_error(string("connect error: ") + strerror(errno) + " \n");
}

void Transport::download(string nameFile, uint32_t sizeFile) {
    OutputFile outputFile(nameFile);
    removeSegments();

    uint32_t bytesReceivedCount = 0;
    uint32_t bytesWrittenCount = 0;

    while (bytesWrittenCount < sizeFile) {
        sendPackets(bytesWrittenCount, sizeFile);
        bytesReceivedCount += receivePackets(bytesWrittenCount, bytesReceivedCount, sizeFile);
        bytesWrittenCount += writeToFile(outputFile);
    }

    cout << "Received " << bytesWrittenCount << " bytes." << endl;
}

void Transport::removeSegments() {
    window.clear();
    window = deque<string>(maxWindowSize, "");
}

void Transport::sendPackets(uint32_t bytesWrittenCount, uint32_t sizeFile) {
    uint32_t start = bytesWrittenCount;

    for (auto it = window.begin(); it != window.end() && start < sizeFile; ++it, start += maxWindowSize) {
        if (*it == "") {
            uint32_t sgmtSize = isNotFullSegment(start, sizeFile) ? sizeFile % maxWindowSize : maxWindowSize;
            string message = "GET " + to_string(start) + " " + to_string(sgmtSize) + "\n";
            SendTo(sockfd, message.c_str(), message.length(), 0, (struct sockaddr *) &serverAddress,
                   sizeof(serverAddress));
        }
    }
}

uint32_t Transport::receivePackets(uint32_t bytesWrittenCount, uint32_t bytesReceivedCount, uint32_t sizeFile) {
    uint32_t packets = 0;
    uint32_t bytes = 0;
    Receiver receiver;

    while (packets < senderWindowSize) {
        fd_set descriptors;
        FD_ZERO (&descriptors);
        FD_SET (sockfd, &descriptors);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 200;

        int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);

        if (ready < 0)
            throw runtime_error("select error: waiting for the package in the socket has failed \n");

        if (ready == 0)
            break;

        Packet packet = receiver.receivePacket(sockfd);
        if (packet.ipAddr == ipAddr && packet.port == port) {
            string data = packet.data;
            data = data.substr(data.find('\n') + 1);
            uint32_t start = 0, sizeData = 0;

            try {
                start = extractStart(packet.data);
                sizeData = extractSizeData(packet.data);
            } catch (const exception &e) {
                cerr << "Invalid header was received" << endl;
                continue;
            }

            if (isFittedToWindow(start, bytesWrittenCount) && isCorrectSizeOfData(start, sizeData, data, sizeFile)) {
                uint32_t id = (start - bytesWrittenCount) / maxWindowSize;
                if (window[id] == "") {
                    bytes += sizeData;
                    packets++;
                    window[id] = data;
                    printToConsole(bytesReceivedCount + bytes, sizeFile);
                }
            }
        }
    }

    return bytes;
}

uint32_t Transport::extractStart(string data) {
    data = data.substr(0, data.find('\n'));
    data = data.substr(data.find(' ') + 1);
    return stoi(data);
}

uint32_t Transport::extractSizeData(string data) {
    data = data.substr(0, data.find('\n'));
    data = data.substr(data.find(' ') + 1);
    data = data.substr(data.find(' ') + 1);
    return stoi(data);
}

bool Transport::isFittedToWindow(uint32_t start, uint32_t bytesWrittenCount) {
    return (start % maxWindowSize == 0) && bytesWrittenCount <= start &&
           start < (bytesWrittenCount + window.size() * maxWindowSize);
}

bool Transport::isCorrectSizeOfData(uint32_t start, uint32_t sizeData, string data, uint32_t sizeFile) {
    return data.size() == sizeData && (sizeData == maxWindowSize || isNotFullSegment(start, sizeFile));
}

bool Transport::isNotFullSegment(uint32_t start, uint32_t sizeFile) {
    return (sizeFile % maxWindowSize) != 0 && (sizeFile - start) < maxWindowSize;
}

uint32_t Transport::writeToFile(OutputFile &outputFile) {
    uint32_t bytes = 0;
    while (window.front() != "") {
        string segment = window.front();

        outputFile.write(segment);
        bytes += segment.size();

        window.pop_front();
        window.push_back("");
    }

    return bytes;
}

void Transport::printToConsole(uint32_t bytesReceivedCount, uint32_t sizeFile) {
    double percent = (bytesReceivedCount * 100.0) / sizeFile;
    percent = percent > 100 ? 100 : percent;
    cout << "[" << setw(50) << left << string(static_cast<unsigned long>(percent / 2.0), '=') << "]";
    cout << " downloaded: " << percent << "%" << endl << endl;
}

Transport::~Transport() {
    close(sockfd);
}