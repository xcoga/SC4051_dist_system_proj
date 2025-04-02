#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <vector>
#include <string>

#include "RequestMessage.hpp"
#include "Socket.hpp"

#define TIMEOUT_SEC 5
#define MAX_RETRIES 5

/**
 * @class Client
 * @brief Handles communication with the server for facility booking operations.
 * 
 * The Client class provides methods to query, book, update, and delete facilities,
 * as well as monitor availability and rate facilities.
 */
class Client
{
private:
    Socket socket; ///< Socket for communication with the server.
    struct sockaddr_in clientAddr, serverAddr; ///< Local and remote socket addresses.
    std::vector<uint8_t> buffer; ///< Buffer for storing received data.
    int requestID; ///< Unique ID for each request sent to the server.

public:
    /**
     * @brief Constructs a Client object and initializes the connection to the server.
     * @param serverIp The IP address of the server.
     * @param serverPort The port number of the server.
     */
    Client(const std::string &serverIp, int serverPort);

    /**
     * @brief C;pses the socket and cleans up resources.
     */
    ~Client();

    /**
     * @brief Queries the names of all available facilities.
     * @return A string containing the list of facility names or error message.
     */
    std::string queryFacilityNames();

    /**
     * @brief Queries the availability of a facility for specific days.
     * @param facilityName The name of the facility to query availability for.
     * @param daysOfWeek A comma-separated list of days (e.g., "MONDAY,TUESDAY,WEDNESDAY").
     * @return A string containing the availability information or error message.
     */
    std::string queryAvailability(std::string facilityName, std::string daysOfWeek);

    /**
     * @brief Books a facility for a specific day and time range.
     * @param facilityName The name of the facility.
     * @param dayOfWeek The day of the week (e.g., "MONDAY").
     * @param startTime The start time in HHMM format (e.g., "0900").
     * @param endTime The end time in HHMM format (e.g., "1100").
     * @return A string containing the booking confirmation or error message.
     */
    std::string bookFacility(
        const std::string facilityName,
        const std::string dayOfWeek,
        const std::string startTime,
        const std::string endTime
    );

    /**
     * @brief Queries the details of an existing booking.
     * @param bookingID The ID of the booking to query.
     * @return A string containing the booking details or an error message.
     */
    std::string queryBooking(std::string bookingID);

    /**
     * @brief Updates an existing booking by applying a time offset.
     * @param oldBookingID The ID of the booking to update.
     * @param offsetMinutes The time offset in minutes (positive for later, negative for earlier).
     * @param oldBookingDetails The details of the old booking to be updated.
     * @return A string containing the updated booking details or an error message.
     */
    std::string updateBooking(
        std::string oldBookingID,
        int offsetMinutes,
        std::string oldBookingDetails
    );

    /**
     * @brief Deletes an existing booking.
     * @param bookingID The ID of the booking to delete.
     * @param bookingDetails The details of the booking to delete.
     * @return A string containing the deletion confirmation or an error message.
     */
    std::string deleteBooking(std::string bookingID, std::string bookingDetails);

    /**
     * @brief Monitors the availability of a facility for a specified duration.
     * @param facilityName The name of the facility to monitor.
     * @param durationSeconds The duration to monitor in seconds.
     * @param onUpdate Callback function to handle updates during monitoring.
     */
    void monitorAvailability(
        const std::string facilityName,
        const int durationSeconds,
        const std::function<void(const std::string &, const bool)> &onUpdate
    );

    /**
     * @brief Rates a facility.
     * @param facilityName The name of the facility to rate.
     * @param rating The rating value (e.g., 4.5).
     * @return A string containing the rating confirmation or an error message.
     */
    std::string rateFacility(std::string facilityName, float rating);       // Non-idempotent operation

    /**
     * @brief Queries the rating of a facility.
     * @param facilityName The name of the facility to query.
     * @return A string containing the rating information or error message.
     */
    std::string queryRating(std::string facilityName);                      // Idempotent operation

    /**
     * @brief Sends an echo message to the server and receives the response.
     * @param messageData The message data to send.
     * @return A string containing the echoed message from the server or an error message.
     */
    std::string echoMessage(std::string messageData);

private:
    /**
     * @brief Creates a local socket address.
     * @param sa Pointer to the sockaddr_in structure to initialize.
     */
    void makeLocalSocketAddress(struct sockaddr_in *sa);

    /**
     * @brief Creates a remote socket address from a hostname and port.
     * @param sa Pointer to the sockaddr_in structure to initialize.
     * @param hostname The hostname or IP address of the server.
     * @param port The port number of the server.
     */
    void makeRemoteSocketAddress(struct sockaddr_in *sa, char *hostname, int port);

    /**
     * @brief Sends a request message to the server.
     * @param request The request message to send.
     * @param retry Whether this is a retry attempt.
     */
    void sendRequest(const RequestMessage &request, bool retry = false);

    /**
     * @brief Receives a response from the server and verifies the request ID.
     * @param expectedRequestID The expected request ID for the response.
     * @return A string containing the response message from the server or an error message.
     */
    std::string receiveResponse(uint32_t expectedRequestID);

    /**
     * @brief Sends a request to the server with retries.
     * @param request The request message to send.
     * @return A string containing the response message from the server or an error message.
     */
    std::string sendWithRetry(const RequestMessage &request);

    /**
     * @brief Extracts the facility name from the booking details string.
     * @param bookingDetails The booking details string.
     * @return The extracted facility name.
     */
    std::string extractFacilityName(const std::string &bookingDetails);

    /**
     * @brief Extracts the day of the week from the booking details string.
     * @param bookingDetails The booking details string.
     * @return The extracted day of the week.
     */
    std::string extractDayOfWeek(const std::string &bookingDetails);

    /**
     * @brief Extracts the start time from the booking details string.
     * @param bookingDetails The booking details string.
     * @return The extracted start time in HHMM format.
     */
    std::string extractStartTime(const std::string &bookingDetails);

    /**
     * @brief Extracts the end time from the booking details string.
     * @param bookingDetails The booking details string.
     * @return The extracted end time in HHMM format.
     */
    std::string extractEndTime(const std::string &bookingDetails);

    /**
     * @brief Listens for monitoring updates from the server.
     * @param durationSeconds The duration to monitor in seconds.
     * @param onUpdate Callback function to handle updates during monitoring.
     */
    void listenForMonitoringUpdates(
        const int durationSeconds,
        const std::function<void(const std::string &, const bool)> &onUpdate
    );
};

#endif // CLIENT_HPP
