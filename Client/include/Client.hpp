#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <vector>
#include <string>

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
    Client(const std::string &serverIp, const int serverPort);
    ~Client();

    std::string queryFacilityNames();
    std::string queryAvailability(const std::string facilityName, const std::string daysOfWeek);
    std::string bookFacility(
        const std::string facilityName,
        const std::string dayOfWeek,
        const std::string startTime,
        const std::string endTime
    );
    std::string queryBooking(const std::string bookingID);
    std::string updateBooking(const std::string oldBookingID, const int offsetMinutes);
    std::string deleteBooking(const std::string bookingID);
    void monitorAvailability(
        const std::string facilityName,
        const int durationSeconds,
        const std::function<void(const std::string &, const bool)> &onUpdate
    );
    std::string rateFacility(const std::string facilityName, const float rating);       // Non-idempotent operation
    std::string queryRating(const std::string facilityName);                      // Idempotent operation
    std::string echoMessage(const std::string messageData);

private:
    void makeLocalSocketAddress(struct sockaddr_in *sa);
    void makeRemoteSocketAddress(struct sockaddr_in *sa, const char *hostname, const int port);

    void sendRequest(const RequestMessage &request, const bool retry = false);
    // std::string receiveResponse();
    std::string receiveResponse(const uint32_t expectedRequestID);
    std::string sendWithRetry(const RequestMessage &request);

    std::string extractFacilityName(const std::string &bookingDetails);
    std::string extractDayOfWeek(const std::string &bookingDetails);
    std::string extractStartTime(const std::string &bookingDetails);
    std::string extractEndTime(const std::string &bookingDetails);

    void listenForMonitoringUpdates(
        const int durationSeconds,
        const std::function<void(const std::string &, const bool)> &onUpdate
    );
};

#endif // CLIENT_HPP
