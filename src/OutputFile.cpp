// Kamil Breczko (280 990)

#include "OutputFile.h"

OutputFile::OutputFile(string name) {
    file.open(name, ios::out | ios::binary);

    if (file.bad() || file.fail()) {
        file.close();
        file.exceptions(ios::failbit | ios::badbit);
    }
}

OutputFile::~OutputFile() {
    file.close();
}

void OutputFile::write(string msg) {
    file << msg;
}
