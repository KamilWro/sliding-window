// Kamil Breczko (280 990)

#include <iostream>
#include "Transport.h"

using namespace std;

int main(int argc, char const *argv[]) {
    if (argc != 4) {
        cerr << "Incorrect input, three arguments were expected";
        return -1;
    }

    try {
        int port = stoi(argv[1]);
        string fileName = string(argv[2]);
        int size = stoi(argv[3]);

        Transport transport(port, "156.17.4.30");
        transport.download(fileName, size);
    } catch (const exception &e) {
        cerr << e.what();
        return -1;
    }

    return 0;
}