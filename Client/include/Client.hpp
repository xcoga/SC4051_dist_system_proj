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

    std::string queryFacilityNames();
    std::string queryAvailability(std::string facilityName);
    std::string bookFacility(
        std::string facilityName,
        std::string dayOfWeek,
        std::string startTime,
        std::string endTime
    );
    std::string queryBooking(std::string bookingID);
    std::string changeBooking(
        std::string oldBookingID,
        std::string newDayOfWeek,
        std::string newStartTime,
        std::string newEndTime
    );
    std::string deleteBooking(std::string bookingID);
    std::string monitorAvailability(std::string facilityName);
    std::string rateFacility(std::string facilityName, float rating);       // Non-idempotent operation
    std::string queryRating(std::string facilityName);                      // Idempotent operation
    std::string echoMessage(std::string messageData);

private:
    void makeLocalSocketAddress(struct sockaddr_in *sa);
    void makeRemoteSocketAddress(struct sockaddr_in *sa, char *hostname, int port);
    void sendRequest(const RequestMessage &request);
    std::string receiveResponse();
};

#endif // CLIENT_HPP