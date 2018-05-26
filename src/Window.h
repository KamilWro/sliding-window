// Kamil Breczko (280 990)

#ifndef TRANSPORT_WINDOW_H
#define TRANSPORT_WINDOW_H

#include <iostream>
#include <deque>
#include <utility>

#include "OutputFile.h"

#define WINDOW_SIZE 500

using namespace std;

class Window {
    deque<string> window;

public:
    Window();

    uint32_t size();

    bool isEmpty(uint32_t i);

    void setData(uint32_t id, string data);

    uint32_t writeToFile(OutputFile &outputFile);
};


#endif
