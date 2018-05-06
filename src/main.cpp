// Kamil Breczko (280 990)

#include <iostream>
#include "Transport.h"

using namespace std;

void validate(const char *port_s, const char *sizeFile_s);

int main(int argc, char const *argv[]) {
    if (argc != 4) {
        cerr << "Incorrect input, three arguments were expected \n";
        return -1;
    }

    validate(argv[1], argv[3]);

    try {
        uint32_t port = stoi(argv[1]);
        string fileName = string(argv[2]);
        uint32_t sizeFile = stoi(argv[3]);

        Transport transport(port, "156.17.4.30");
        transport.download(fileName, sizeFile);
    } catch (const exception &e) {
        cerr << e.what();
        return -1;
    }

    return 0;
}

void validate(const char *port_s, const char *sizeFile_s) {
    try {
        int port = stoi(port_s);
        int sizeFile = stoi(sizeFile_s);

        if (port < 0 || port > 0xFFFF)
            throw invalid_argument("Port must be between 0 and 65535 \n");

        if (sizeFile < 0 || sizeFile > 10001000)
            throw invalid_argument("Size file must be between 0 and 10001000 \n");

    } catch (const exception &e) {
        throw invalid_argument("Incorrect input, expected: port fileName fileSize \n");
    }
}
