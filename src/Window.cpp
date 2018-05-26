// Kamil Breczko (280 990)

#include "Window.h"

Window::Window() {
    window = deque<string>(WINDOW_SIZE, "");
}

uint32_t Window::writeToFile(OutputFile &outputFile) {
    uint32_t bytes = 0;
    while (!window.front().empty()) {
        string segment = window.front();

        outputFile.write(segment);
        bytes += segment.size();

        window.pop_front();
        window.emplace_back("");
    }

    return bytes;
}

uint32_t Window::size() {
    return window.size();
}

bool Window::isEmpty(uint32_t i) {
    return window[i].empty();
}

void Window::setData(uint32_t id, string data) {
    window[id] = move(data);
}
