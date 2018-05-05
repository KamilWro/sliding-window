// Kamil Breczko (280 990)

#ifndef TRANSPORT_OUTPUTFILE_H
#define TRANSPORT_OUTPUTFILE_H

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class OutputFile {
    ofstream file;
public:
    OutputFile(string name);

    ~OutputFile();

    void write(string msg);
};

#endif
