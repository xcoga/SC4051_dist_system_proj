#include "Socket.hpp"

#include <iostream>
#include <cstring>

#ifdef _WIN32
    // Winsock initialization
    bool initializeWinsock()
    {
        return (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);
    }

    void cleanupWinsock()
    {
        WSACleanup();
    }
#endif

Socket::Socket() : sockfd(-1)
{
#ifdef _WIN32
    // Initialize Winsock
    if (!initializeWinsock())
    {
        throw std::runtime_error("Winsock initialization failed!");
    }
#endif
}

Socket::~Socket()
{
    closeSocket();
#ifdef _WIN32
    cleanupWinsock();
#endif
}

void Socket::create(int domain, int type, int protocol)
{
#ifdef _WIN32
    sockfd = socket(domain, type, protocol);
    if (sockfd == INVALID_SOCKET)
    {
        throw std::runtime_error("Socket creation failed!");
    }
#else
    sockfd = socket(domain, type, protocol);
    if (sockfd == -1)
    {
        throw std::runtime_error("Socket creation failed!");
    }
#endif
}

void Socket::bind(int port)
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

#ifdef _WIN32
    if (::bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
#else
    if (::bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
#endif
    {
        throw std::runtime_error("Bind failed!");
    }
}

void Socket::sendDataTo(const std::vector<uint8_t> &data, const struct sockaddr_in &addr)
{
#ifdef _WIN32
    int result = sendto(sockfd, (const char *)data.data(), data.size(), 0, (const sockaddr *)&addr, sizeof(addr));
    if (result == SOCKET_ERROR)
    {
        throw std::runtime_error("Send failed!");
    }
#else
    ssize_t result = sendto(sockfd, data.data(), data.size(), 0, (const struct sockaddr *)&addr, sizeof(addr));
    if (result == -1)
    {
        throw std::runtime_error("Send failed!");
    }
#endif
}

// TODO: Fix receiving messages from server
int Socket::receiveDataFrom(char *buffer, struct sockaddr_in &addr)
{
#ifdef _WIN32
    int addrLen = sizeof(addr);
#else
    socklen_t addrLen = sizeof(addr);
#endif
    int bytesReceived = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&addr, &addrLen);
    if (bytesReceived < 0)
    {
        throw std::runtime_error("Receive failed!");
    }
    return bytesReceived;
}

// std::string Socket::receiveDataFrom(struct sockaddr_in &addr)
// {
//     char buffer[BUFFER_SIZE];
//     memset(buffer, 0, sizeof(buffer));

// #ifdef _WIN32
//     int result = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, (int *)sizeof(addr));
//     if (result == SOCKET_ERROR)
//     {
//         std::cerr << "Receive failed!" << std::endl;
//         return "";
//     }
// #else
//     ssize_t result = recvfrom(sockfd, buffer, sizeof(buffer), 0 , (struct sockaddr *)&addr, (socklen_t *)sizeof(addr));
//     if (result == -1)
//     {
//         std::cerr << "Receive failed!" << std::endl;
//         return "";
//     }
// #endif

//     return std::string(buffer);
// }

int Socket::getSocketName(struct sockaddr *addr)
{
#ifdef _WIN32
    int addrLen = sizeof(*addr);
    return getsockname(sockfd, addr, &addrLen);
#else
    socklen_t addrLen = sizeof(*addr);
    return getsockname(sockfd, addr, &addrLen);
#endif
}

void Socket::closeSocket()
{
#ifdef _WIN32
    closesocket(sockfd);  // Windows specific
#else
    close(sockfd);  // UNIX specific
#endif
}
