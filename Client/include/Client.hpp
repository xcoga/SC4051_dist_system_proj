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

#define TIMEOUT_SEC 5
#define MAX_RETRIES 5

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
    std::string queryAvailability(std::string facilityName, std::string daysOfWeek);
    std::string bookFacility(
        std::string facilityName,
        std::string dayOfWeek,
        std::string startTime,
        std::string endTime
    );
    std::string queryBooking(std::string bookingID);
    std::string updateBooking(std::string oldBookingID, int offsetMinutes);
    std::string deleteBooking(std::string bookingID);
    void monitorAvailability(
        std::string facilityName,
        int durationSeconds,
        const std::function<void(const std::string &, const bool)> &onUpdate
    );
    std::string rateFacility(std::string facilityName, float rating);       // Non-idempotent operation
    std::string queryRating(std::string facilityName);                      // Idempotent operation
    std::string echoMessage(std::string messageData);

private:
    void makeLocalSocketAddress(struct sockaddr_in *sa);
    void makeRemoteSocketAddress(struct sockaddr_in *sa, char *hostname, int port);

    void sendRequest(const RequestMessage &request, bool retry = false);
    // std::string receiveResponse();
    std::string receiveResponse(uint32_t expectedRequestID);
    std::string sendWithRetry(const RequestMessage &request);

    std::string extractFacilityName(const std::string &bookingDetails);
    std::string extractDayOfWeek(const std::string &bookingDetails);
    std::string extractStartTime(const std::string &bookingDetails);
    std::string extractEndTime(const std::string &bookingDetails);

    void listenForMonitoringUpdates(
        int durationSeconds,
        const std::function<void(const std::string &, const bool)> &onUpdate
    );
};

#endif // CLIENT_HPP
