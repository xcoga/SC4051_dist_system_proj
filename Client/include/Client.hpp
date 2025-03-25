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
    int requestID;

public:
    Client(const std::string &serverIp, int serverPort);
    ~Client();

    std::string queryAvailability(std::string facilityName);
    std::string bookFacility(std::string facilityName, std::string dayOfWeek, std::string startTime, std::string endTime);
    std::string queryBooking(std::string bookingID);
    std::string changeBooking(std::string bookingID, std::string newDayOfWeek, std::string newStartTime, std::string newEndTime);
    std::string monitorAvailability(std::string facilityName);
    // TODO: idempotentOperation();
    // TODO: nonIdempotentOperation();
    std::string sendCustomMessage(std::string messageData);

private:
    void makeLocalSocketAddress(struct sockaddr_in *sa);
    void makeRemoteSocketAddress(struct sockaddr_in *sa, char *hostname, int port);
    void printBoundSocketInfo();
    void sendRequest(const RequestMessage &request);
    std::string receiveResponse();
};

#endif // CLIENT_HPP