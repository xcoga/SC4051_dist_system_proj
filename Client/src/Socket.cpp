#include "Socket.hpp"

#include <iostream>
#include <cstring>

#include "Constants.hpp"

#ifdef _WIN32
    /**
     * @brief Initializes Winsock for Windows systems.
     * 
     * @return True if initialization is successful, false otherwise.
     */
    bool initializeWinsock()
    {
        WSADATA wsaData;
        return (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);
    }

    /**
     * @brief Cleans up Winsock resources for Windows systems.
     */
    void cleanupWinsock()
    {
        WSACleanup();
    }
#endif

/**
 * @brief Constructs a Socket object and initializes platform-specific resources.
 */
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

/**
 * @brief Destroys the Socket object and cleans up resources.
 */
Socket::~Socket()
{
    closeSocket();
#ifdef _WIN32
    cleanupWinsock();
#endif
}

/**
 * @brief Creates a socket with the specified domain, type, and protocol.
 * 
 * This method initializes the socket using the specified parameters. It throws an exception if the socket creation fails.
 * 
 * @param domain The communication domain (e.g., AF_INET for IPv4).
 * @param type The socket type (e.g., SOCK_DGRAM for UDP).
 * @param protocol The protocol to use (e.g., IPPROTO_UDP for UDP).
 * 
 * @throws std::runtime_error if socket creation fails.
 */
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

/**
 * @brief Binds the socket to the specified port.
 * 
 * This method binds the socket to the specified port. It throws an exception if the binding fails.
 * 
 * @param port The port number to bind the socket to.
 * 
 * @throws std::runtime_error if binding fails.
 */
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

/**
 * @brief Sets the receive timeout for the socket.
 * 
 * This method sets the receive timeout for the socket. It throws an exception if setting the timeout fails.
 * 
 * @param seconds The timeout duration in seconds.
 * 
 * @throws std::runtime_error if setting the timeout fails.
 * 
 * @note On Windows, the timeout is set in milliseconds. Hence, the input in seconds is first converted to milliseconds before setting the timeout.
 */
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

/**
 * @brief Sends data to a specified address.
 * 
 * This method sends data to the specified address. It throws an exception if sending fails.
 * 
 * @param data The data to send.
 * @param addr The destination address.
 * 
 * @throws std::runtime_error if sending fails.
 */
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

/**
 * @brief Receives data from a specified address.
 * 
 * This method receives data from the specified address. It throws an exception if receiving fails.
 * 
 * @param buffer The buffer to store the received data.
 * @param addr The source address.
 * 
 * @return The number of bytes received.
 * 
 * @throws std::runtime_error if receiving fails.
 */
int Socket::receiveDataFrom(char *buffer, struct sockaddr_in &addr)
{
#ifdef _WIN32
    int addrLen = sizeof(addr);
#else
    socklen_t addrLen = sizeof(addr);
#endif
    int bytesReceived = recvfrom(sockfd, buffer, Constants::BUFFER_SIZE, 0, (struct sockaddr *)&addr, &addrLen);
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

/**
 * @brief Retrieves the local socket name (IP address and port).
 * 
 * This method retrieves the local socket name and stores it in the provided address structure. It throws an exception if retrieving the socket name fails.
 * 
 * @param addr The address structure to store the socket name.
 * 
 * @return 0 on success, or throws an exception on failure.
 */
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

/**
 * @brief Closes the socket and cleans up resources.
 */
void Socket::closeSocket()
{
#ifdef _WIN32
    closesocket(sockfd);
#else
    close(sockfd);
#endif
}
