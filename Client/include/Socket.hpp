#ifndef SOCKET_HPP
#define SOCKET_HPP

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
#endif

#include <iostream>
#include <vector>
#include <string>

#define BUFFER_SIZE 1024

class Socket
{
public:
    Socket();
    ~Socket();

    void create(const int domain, const int type, const int protocol);
    void bind(const int port);
    void sendDataTo(const std::vector<uint8_t> &data, const struct sockaddr_in &addr);
    int receiveDataFrom(char *buffer, struct sockaddr_in &addr);
    int getSocketName(struct sockaddr *addr);
    void closeSocket();

private:
    int sockfd;

#ifdef _WIN32
    WSADATA wsaData;
#endif
};

#endif // SOCKET_HPP