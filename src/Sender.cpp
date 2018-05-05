// Kamil Breczko (280 990)

#include "Sender.h"

void Sender::send(int sockfd, struct sockaddr_in server_address, string message) {
    size_t message_len = message.size();
    ssize_t bytes_sent = sendto(sockfd, message.c_str(), message_len, 0, (struct sockaddr *) &server_address,
                                sizeof(server_address));

    if (bytes_sent != (ssize_t) message_len)
        cerr << "sendto error: " << strerror(errno);
}