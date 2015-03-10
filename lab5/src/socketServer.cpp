#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>

#include "socketServer.hpp"

void SocketServer::bind_port() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        throw new std::runtime_error("Error opening socket");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(
            sockfd,
            reinterpret_cast<struct sockaddr*>(&serv_addr),
            sizeof(serv_addr)) < 0) {
        throw new std::runtime_error("Error on binding");
    }

    std::cout << "Bound to port " << port << " successfully." << std::endl;
}

void SocketServer::wait_for_connection() {
    std::cout << "Listening for connections..." << std::endl;
    listen(sockfd, 5);
    socklen_t clilen = sizeof(cli_addr);

    sockfd = accept(
            sockfd,
            reinterpret_cast<struct sockaddr*>(&cli_addr),
            &clilen);

    if (sockfd < 0) {
        throw new std::runtime_error("Error on accept");
    }

    std::cout << "Connection established!" << std::endl;
}

void SocketServer::write_data(const char *buffer, size_t size) {
    // We are going to use a rudimentary protocol here. First, we write the
    // length of the buffer to the socket (padded to 3 digits).
    size_t numZeros = 0;
    if (size > 999) {
        throw new std::runtime_error("Packet too big!");
    } else if (size > 99) {
        numZeros = 0;
    } else if (size > 9) {
        numZeros = 1;
    } else {
        numZeros = 0;
    }

    std::string packetLength = std::string(numZeros, '0').append(std::to_string(size));
    int n = write(sockfd, packetLength.c_str(), packetLength.size());
    if (n < 0) {
        throw new std::runtime_error("Error writing to socket");
    }

    n = write(sockfd, buffer, size);
    if (n < 0) {
        throw new std::runtime_error("Error writing to socket");
    }
}
