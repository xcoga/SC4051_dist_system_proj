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
        int errorCode = WSAGetLastError();
        throw std::runtime_error("Socket creation failed! Error code: " + std::to_string(errorCode));
    }
#else
    sockfd = socket(domain, type, protocol);
    if (sockfd == -1)
    {
        throw std::runtime_error("Socket creation failed! Error: " + std::string(strerror(errno)));
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
    {
        int errorCode = WSAGetLastError();
        throw std::runtime_error("Bind failed! Error code: " + std::to_string(errorCode));
    }
#else
    if (::bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        throw std::runtime_error("Bind failed! Error: " + std::string(strerror(errno)));
    }
#endif
}

void Socket::setReceiveTimeout(int seconds)
{
#ifdef _WIN32
    DWORD timeout = seconds * 1000;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        throw std::runtime_error("Set receive timeout failed! Error code: " + std::to_string(errorCode));
    }
#else
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) == -1)
    {
        throw std::runtime_error("Set receive timeout failed! Error: " + std::string(strerror(errno)));
    }
#endif
}


void Socket::sendDataTo(const std::vector<uint8_t> &data, const struct sockaddr_in &addr)
{
#ifdef _WIN32
    int result = sendto(sockfd, (const char *)data.data(), data.size(), 0, (const sockaddr *)&addr, sizeof(addr));
    if (result == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        throw std::runtime_error("Send failed! Error code: " + std::to_string(errorCode));
    }
#else
    ssize_t result = sendto(sockfd, data.data(), data.size(), 0, (const struct sockaddr *)&addr, sizeof(addr));
    if (result == -1)
    {
        throw std::runtime_error("Send failed! Error: " + std::string(strerror(errno)));
    }
#endif
}

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
#ifdef _WIN32
        int errorCode = WSAGetLastError();
        if (errorCode == WSAETIMEDOUT)
        {
            throw std::runtime_error("Receive failed! Error: Timeout occurred");
        }
        else
        {
            throw std::runtime_error("Receive failed! Error code: " + std::to_string(errorCode));
        }
#else
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            throw std::runtime_error("Receive failed! Error: Timeout occurred");
        }
        else
        {
            throw std::runtime_error("Receive failed! Error code: " + std::to_string(errno));
        }
#endif
    }
    return bytesReceived;
}

int Socket::getSocketName(struct sockaddr *addr)
{
#ifdef _WIN32
    int addrLen = sizeof(*addr);
    int result = getsockname(sockfd, addr, &addrLen);
    if (result == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        throw std::runtime_error("Get socket name failed! Error code: " + std::to_string(errorCode));
    }
#else
    socklen_t addrLen = sizeof(*addr);
    int result = getsockname(sockfd, addr, &addrLen);
    if (result == -1)
    {
        throw std::runtime_error("Get socket name failed! Error: " + std::string(strerror(errno)));
    }
#endif
    return result;
}

void Socket::closeSocket()
{
#ifdef _WIN32
    closesocket(sockfd);
#else
    close(sockfd);
#endif
}
