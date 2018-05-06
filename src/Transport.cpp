// Kamil Breczko (280 990)

#include "Transport.h"

Transport::Transport(int port, string ipAddr) : port(port), ipAddr(ipAddr) {
    if (port < 0 || port > 0xFFFF)
        throw invalid_argument("port must be between 0 and 65535");

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
        throw runtime_error(string("socket error: ") + strerror(errno));

    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    int address = inet_pton(AF_INET, ipAddr.c_str(), &serverAddress.sin_addr);
    if (address == 0)
        throw invalid_argument("error: incorrect IPv4 address");

    if (connect(sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
        throw runtime_error(string("connect error: ") + strerror(errno));
}

void Transport::download(string nameFile, uint32_t sizeFile) {
    OutputFile outputFile(nameFile);
    removeSegments();

    uint32_t bytesReceivedCount = 0;
    uint32_t bytesWrittenCount = 0;

    while (bytesWrittenCount < sizeFile) {
        sendPackets(bytesWrittenCount, sizeFile);
        receivePackets(bytesWrittenCount, bytesReceivedCount, sizeFile);
        bytesWrittenCount += writeToFile(outputFile);
    }

    cout << "Received " << bytesReceivedCount << " bytes." << endl;
}

void Transport::removeSegments() {
    segments.clear();
    for (uint32_t i = 0; i < senderWindowSize; i++)
        segments.push_back("");
}

void Transport::sendPackets(uint32_t bytesWrittenCount, uint32_t sizeFile) {
    Sender sender;
    uint32_t start = bytesWrittenCount;

    for (auto it = segments.begin(); it != segments.end() && start < sizeFile; ++it, start += MAX_SEGMENT_SIZE) {
        if (*it == "") {
            uint32_t sgmtSize = (sizeFile - start) < MAX_SEGMENT_SIZE ? sizeFile % MAX_SEGMENT_SIZE : MAX_SEGMENT_SIZE;

            string message = "GET " + to_string(start) + " " + to_string(sgmtSize) + "\n";
            sender.send(sockfd, serverAddress, message);
        }
    }
}

void Transport::receivePackets(uint32_t bytesWrittenCount, uint32_t &bytesReceivedCount, uint32_t sizeFile) {
    uint32_t packetsReceivedCount = 0;
    Receiver receiver;
    while (packetsReceivedCount < senderWindowSize) {
        fd_set descriptors;
        FD_ZERO (&descriptors);
        FD_SET (sockfd, &descriptors);

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 200;

        int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);

        if (ready < 0)
            throw runtime_error("select error: waiting for the package in the socket has failed");

        if (ready == 0)
            break;

        Packet packet = receiver.receive(sockfd);
        if (packet.ipAddr == ipAddr && packet.port == port) {
            string data = packet.data;
            string header = data.substr(0, data.find('\n'));
            data = data.substr(data.find('\n') + 1);

            string start_s = header.substr(header.find(' ') + 1);
            string sizeData_s = start_s.substr(start_s.find(' ') + 1);

            uint32_t start = stoi(start_s);
            uint32_t sizeData = stoi(sizeData_s);

            if (isFittedToWindow(start, bytesWrittenCount) && isCorrectSizeOfData(start, sizeData, data, sizeFile)) {
                uint32_t id = (start - bytesWrittenCount) / MAX_SEGMENT_SIZE;
                if (segments[id] == "") {
                    bytesReceivedCount += sizeData;
                    packetsReceivedCount++;
                    segments[id] = data;
                    printToConsole(bytesReceivedCount, sizeFile);
                }
            }
        }
    }
}

bool Transport::isFittedToWindow(uint32_t start, uint32_t bytesWrittenCount) {
    return (start % MAX_SEGMENT_SIZE == 0) && bytesWrittenCount <= start &&
           (start < bytesWrittenCount + segments.size() * MAX_SEGMENT_SIZE);
}

bool Transport::isCorrectSizeOfData(uint32_t start, uint32_t sizeData, string data, uint32_t sizeFile) {
    return data.size() == sizeData && (sizeData == MAX_SEGMENT_SIZE ||
                                       ((sizeFile % MAX_SEGMENT_SIZE != 0) && (sizeFile - start) < MAX_SEGMENT_SIZE));
}

uint32_t Transport::writeToFile(OutputFile &outputFile) {
    uint32_t bytes = 0;
    while (segments.front() != "") {
        string segment = segments.front();

        outputFile.write(segment);
        bytes += segment.size();

        segments.pop_front();
        segments.push_back("");
    }

    return bytes;
}

void Transport::printToConsole(uint32_t bytesReceivedCount, uint32_t sizeFile) {
    double percent = (bytesReceivedCount * 100.0) / sizeFile;
    percent = percent > 100 ? 100 : percent;
    cout << "[" << setw(50) << left << string(static_cast<unsigned long>(percent / 2.0), '=') << "]" << " downloaded: "
         << percent << "%" << endl << endl;
}

Transport::~Transport() {
    close(sockfd);
}