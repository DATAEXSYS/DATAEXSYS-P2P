#include "receiver.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define RECV_PORT 1040
#define BUFFER_SIZE 1024

namespace network {
    void start_receiver(std::function<void(const std::string& ip, const std::string& message)> on_message_received, std::atomic<bool>& running) {
        int sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            perror("socket failed");
            return;
        }

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

        int opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in6 addr{};
        addr.sin6_family = AF_INET6;
        addr.sin6_addr = in6addr_any;
        addr.sin6_port = htons(RECV_PORT);

        if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind failed");
            close(sockfd);
            return;
        }

        std::cout << "Receiver listening on port 1040...\n";

        while (running) {
            char buffer[BUFFER_SIZE];
            sockaddr_in6 sender{};
            socklen_t len = sizeof(sender);

            memset(buffer, 0, BUFFER_SIZE);

            int bytes = recvfrom(sockfd,
                                 buffer,
                                 BUFFER_SIZE - 1,
                                 0,
                                 (sockaddr*)&sender,
                                 &len);

            if (bytes > 0) {
                char ip[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &sender.sin6_addr, ip, sizeof(ip));
                on_message_received(std::string(ip), std::string(buffer));
            }
        }

        close(sockfd);
    }
}