// Kamil Breczko (280 990)

#include "Transport.h"

Transport::Transport(int port, string ipAddr) : port(port), ipAddr(ipAddr) {
    if (port < 0 && port > 0xFFFF)
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

    segmentCount = (sizeFile / MAX_SEGMENT_SIZE) + (sizeFile % MAX_SEGMENT_SIZE != 0 ? 1 : 0);
    lastACKReceived = 0;
    bytesReceivedCount = 0;

    while (lastACKReceived < segmentCount) {
        sendPackets(sizeFile);
        receivePackets();
        lastACKReceived += writeToFile(outputFile);
        printToConsole(sizeFile);
    }

    cout << "Received " << bytesReceivedCount << " bytes." << endl;
}

void Transport::removeSegments() {
    segments.clear();
    for (uint32_t i = 0; i < senderWindowSize; i++)
        segments.push_back("");
}

void Transport::sendPackets(uint32_t sizeFile) {
    Sender sender;
    uint32_t start = lastACKReceived;
    for (uint32_t i = 0; i < segments.size() && start < segmentCount; i++, start++) {
        if (segments[i] == "") {
            uint32_t segmentSize = (segmentCount - 1) == start ? sizeFile % MAX_SEGMENT_SIZE : MAX_SEGMENT_SIZE;

            string message = "GET " + to_string(start * MAX_SEGMENT_SIZE) + " " + to_string(segmentSize) + "\n";
            cout << message;
            sender.send(sockfd, serverAddress, message);
        }
    }
}

void Transport::receivePackets() {
    uint32_t packetsReceivedCount = 0;
    Receiver receiver;
    while (packetsReceivedCount < senderWindowSize) {
        fd_set descriptors;
        FD_ZERO (&descriptors);
        FD_SET (sockfd, &descriptors);

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

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

            string start = header.substr(header.find(' ') + 1);
            string bytesReceived_s = start.substr(start.find(' ') + 1);

            uint32_t id = stoi(start) / MAX_SEGMENT_SIZE;
            uint32_t bytesReceived = stoi(bytesReceived_s);

            cout << "RECEIVE: " << id << " " << bytesReceived;
            if (lastACKReceived <= id && (id < lastACKReceived + segments.size()) &&
                (bytesReceived == MAX_SEGMENT_SIZE || (segmentCount - 1) == id)) {
                id -= lastACKReceived;
                if (segments[id] == "") {
                    cout << " <- OK";
                    bytesReceivedCount += bytesReceived;
                    segments[id] = data;
                    packetsReceivedCount++;
                }
            }
            cout << endl;
        }
    }
}

uint32_t Transport::writeToFile(OutputFile &outputFile) {
    uint32_t packetsWritedCount = 0;
    while (segments.front() != "") {
        string segment = segments.front();

        outputFile.write(segment);

        segments.pop_front();
        segments.push_back("");
        packetsWritedCount++;
    }

    return packetsWritedCount;
}

void Transport::printToConsole(uint32_t size) {
    uint32_t percent = (bytesReceivedCount * 100) / size;
    percent = percent > 100 ? 100 : percent;
    cout << "[" << setw(50) << left << string(percent / 2, '=') << "]" << "downloaded: " << percent << "%" << endl
         << endl;
}

Transport::~Transport() {
    close(sockfd);
}
