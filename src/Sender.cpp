// Kamil Breczko (280 990)

#include "Sender.h"
#include "Sockwrap.h"

void Sender::send(int sockfd, struct sockaddr_in server_address, string message) {
    size_t message_len = message.size();
    SendTo(sockfd, message.c_str(), message_len, 0, (struct sockaddr *) &server_address, sizeof(server_address));
}