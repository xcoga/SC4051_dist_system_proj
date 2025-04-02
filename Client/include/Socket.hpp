#ifndef SOCKET_HPP
#define SOCKET_HPP

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#define BUFFER_SIZE 1024 ///< Default buffer size for socket operations.

/**
 * @class Socket
 * @brief A wrapper class for socket operations, supporting both Windows and Unix-based systems.
 * 
 * The Socket class provides methods for creating, binding, sending, and receiving data over a socket.
 * It abstracts platform-specific differences between Windows and Unix-based systems, thereby ensuring
 * portability.
 */
class Socket
{
public:
    /**
     * @brief Constructs a Socket object and initializes platform-specific resources.
     */
    Socket();

    /**
     * @brief Destroys the Socket object and cleans up resources.
     */
    ~Socket();

    /**
     * @brief Creates a socket with the specified domain, type, and protocol.
     * @param domain The communication domain (e.g., AF_INET for IPv4).
     * @param type The socket type (e.g., SOCK_DGRAM for UDP).
     * @param protocol The protocol to use (e.g., IPPROTO_UDP for UDP).
     * @throws std::runtime_error if socket creation fails.
     */
    void create(const int domain, const int type, const int protocol);

    /**
     * @brief Binds the socket to the specified port.
     * @param port The port number to bind the socket to.
     * @throws std::runtime_error if binding fails.
     */
    void bind(const int port);

    /**
     * @brief Sets the receive timeout for the socket.
     * @param seconds The timeout duration in seconds.
     * @throws std::runtime_error if setting the timeout fails.
     */
    void setReceiveTimeout(int seconds);

    /**
     * @brief Sends data to a specified address.
     * @param data The data to send.
     * @param addr The destination address.
     * @throws std::runtime_error if sending fails.
     */
    void sendDataTo(const std::vector<uint8_t> &data, const struct sockaddr_in &addr);

    /**
     * @brief Receives data from a specified address.
     * @param buffer The buffer to store the received data.
     * @param addr The source address.
     * @return The number of bytes received.
     * @throws std::runtime_error if receiving fails.
     */
    int receiveDataFrom(char *buffer, struct sockaddr_in &addr);

    /**
     * @brief Retrieves the local socket name (IP address and port).
     * @param addr The address structure to store the socket name.
     * @return 0 on success, or throws an exception on failure.
     * @throws std::runtime_error if retrieving the socket name fails.
     */
    int getSocketName(struct sockaddr *addr);

    /**
     * @brief Closes the socket and cleans up resources.
     */
    void closeSocket();

private:
    int sockfd; ///< The file descriptor for the socket.

#ifdef _WIN32
    WSADATA wsaData; ///< Winsock data structure for Windows.
#endif
};

#endif // SOCKET_HPP
