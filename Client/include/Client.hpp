#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

#include "RequestMessage.hpp"
#include "Socket.hpp"

class Client
{
private:
    Socket socket;
    struct sockaddr_in clientAddr, serverAddr;
    std::vector<uint8_t> buffer;

public:
    Client(const std::string &serverIp, int serverPort);
    ~Client();

    void queryAvailability();
    void bookFacility();
    void changeBooking();
    void monitorAvailability();
    // TODO: idempotentOperation();
    // TODO: nonIdempotentOperation();

    void sendMessage();

private:
    void makeLocalSocketAddress(struct sockaddr_in *sa);
    void makeRemoteSocketAddress(struct sockaddr_in *sa, char *hostname, int port);
    void printBoundSocketInfo();

    void sendRequest(const RequestMessage &request);
    std::string receiveResponse();
};

#endif // CLIENT_HPP