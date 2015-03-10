#ifndef SOCKET_SERVER_HPP
#define SOCKET_SERVER_HPP

#include <netinet/in.h>

class SocketServer {
    public:
        void bind_port();
        void wait_for_connection();
        void write_data(const char *buffer, size_t size);

    private:
        const int port = 9999;
        int sockfd;
        struct sockaddr_in serv_addr, cli_addr;
};

#endif
