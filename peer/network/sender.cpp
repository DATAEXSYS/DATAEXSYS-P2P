#include "sender.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define SEND_PORT 1020

namespace network {
    bool send_message(const std::string& dest_ip, const std::string& message) {
        int sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            perror("socket failed");
            return false;
        }

        int opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in6 local{};
        local.sin6_family = AF_INET6;
        local.sin6_addr = in6addr_any;
        local.sin6_port = htons(SEND_PORT);

        if (bind(sockfd, (sockaddr*)&local, sizeof(local)) < 0) {
            perror("bind failed");
            close(sockfd);
            return false;
        }

        sockaddr_in6 dest{};
        dest.sin6_family = AF_INET6;
        dest.sin6_port = htons(1040);

        if (inet_pton(AF_INET6, dest_ip.c_str(), &dest.sin6_addr) != 1) {
            std::cerr << "Invalid IPv6 address\n";
            close(sockfd);
            return false;
        }

        sendto(sockfd,
               message.c_str(),
               message.length(),
               0,
               (sockaddr*)&dest,
               sizeof(dest));

        close(sockfd);
        return true;
    }
}