// Kamil Breczko (280 990)

#include "Transport.h"

Transport::Transport(uint32_t port, string ipAddr) : port(port), ipAddr(ipAddr) {
    if (port > 0xFFFF)
        throw invalid_argument("port must be between 0 and 65535 \n");

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
        throw runtime_error(string("socket error: ") + strerror(errno) + " \n");

    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    int address = inet_pton(AF_INET, ipAddr.c_str(), &serverAddress.sin_addr);
    if (address == 0)
        throw invalid_argument("error: incorrect IPv4 address \n");

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
    window = deque<string>(MAX_WINDOW_SIZE, "");
}

void Transport::sendPackets(uint32_t bytesWrittenCount, uint32_t sizeFile) {
    Sender sender;
    uint32_t start = bytesWrittenCount;

    for (auto it = window.begin(); it != window.end() && start < sizeFile; ++it, start += MAX_WINDOW_SIZE) {
        if (*it == "") {
            uint32_t sgmtSize = isNotFullSegment(start, sizeFile) ? sizeFile % MAX_WINDOW_SIZE : MAX_WINDOW_SIZE;
            string message = "GET " + to_string(start) + " " + to_string(sgmtSize) + "\n";

            sender.send(sockfd, serverAddress, message);
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

        Packet packet = receiver.receive(sockfd);
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
                uint32_t id = (start - bytesWrittenCount) / MAX_WINDOW_SIZE;
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
    return (start % MAX_WINDOW_SIZE == 0) && bytesWrittenCount <= start &&
           start < (bytesWrittenCount + window.size() * MAX_WINDOW_SIZE);
}

bool Transport::isCorrectSizeOfData(uint32_t start, uint32_t sizeData, string data, uint32_t sizeFile) {
    return data.size() == sizeData && (sizeData == MAX_WINDOW_SIZE || isNotFullSegment(start, sizeFile));
}

bool Transport::isNotFullSegment(uint32_t start, uint32_t sizeFile) {
    return (sizeFile % MAX_WINDOW_SIZE) != 0 && (sizeFile - start) < MAX_WINDOW_SIZE;
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