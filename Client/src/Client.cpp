#include "Client.hpp"

#include "Constants.hpp"
#include "Serializer.hpp"
#include "UserInterface.hpp"
#include <chrono>
/**
 * @brief Constructs a Client object and initializes the connection to the server.
 *
 * This constructor creates a UDP socket, binds it to a local address, and sets up
 * the remote server address. It also sets a timeout for receiving data.
 *
 * @param serverIp The IP address of the server.
 * @param serverPort The port number of the server.
 */
Client::Client(const std::string &serverIp, int serverPort) : requestID(0)
{
    try
    {
        socket.create(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Create a UDP socket
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error creating socket: " << e.what() << std::endl;
        exit(1);
    }

    makeLocalSocketAddress(&clientAddr); // Set up the local address

    try
    {
        socket.bind(0); // Bind to any available port
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error binding socket: " << e.what() << std::endl;
        exit(1);
    }

    makeRemoteSocketAddress(&serverAddr, const_cast<char *>(serverIp.c_str()), serverPort); // Set up the server address

    socket.setReceiveTimeout(Constants::TIMEOUT_SEC); // Set a timeout for receiving data

    UserInterface::displayConnectionInfo(socket, serverAddr); // Display connection info to the user
}

/**
 * @brief Destructor for the Client class.
 *
 * This destructor closes the socket when the client object is destroyed.
 */
Client::~Client()
{
    socket.closeSocket(); // Close the socket when the client is destroyed
}

/**
 * @brief Queries the names of all available facilities.
 *
 * This method sends a request to the server to retrieve the names of all facilities.
 *
 * @return A string containing the list of facility names or an error message.
 */
std::string Client::queryFacilityNames()
{
    std::string messageData = "facility,ALL"; // Request all facility name

    RequestMessage requestMessage(RequestMessage::READ, requestID, messageData); // READ operation

    return sendWithRetry(requestMessage); // Send the request with retry (in case of timeout)
}

/**
 * @brief Queries the availability of a facility for specific days.
 * 
 * This method sends a request to the server to check the availability of a facility for the specified days.
 * 
 * @param facilityName The name of the facility to query availability for.
 * @param daysOfWeek A comma-separated list of days (e.g., "MONDAY,TUESDAY,WEDNESDAY").
 * 
 * @return A string containing the availability information or error message.
 */
std::string Client::queryAvailability(std::string facilityName, std::string daysOfWeek)
{
    std::string messageData = "facility," + facilityName + "," + daysOfWeek; // Request availability for the specified facility and days

    RequestMessage requestMessage(RequestMessage::READ, requestID, messageData); // READ operation

    return sendWithRetry(requestMessage); // Send the request with retry (in case of timeout)
}

/**
 * @brief Books a facility for a specific day and time range.
 * 
 * This method sends a request to the server to book a facility for the specified day and time range.
 * 
 * @param facilityName The name of the facility.
 * @param dayOfWeek The day of the week (e.g., "MONDAY").
 * @param startTime The start time in HHMM format (e.g., "0900").
 * @param endTime The end time in HHMM format (e.g., "1100").
 * 
 * @return A string containing the booking confirmation or error message.
 */
std::string Client::bookFacility(
    std::string facilityName,
    std::string dayOfWeek,
    std::string startTime,
    std::string endTime
)
{
    std::string startTimeHour, startTimeMinute, endTimeHour, endTimeMinute;
    // Extract hours and minutes from the start and end times
    startTimeHour = startTime.substr(0, 2);
    startTimeMinute = startTime.substr(2, 2);
    endTimeHour = endTime.substr(0, 2);
    endTimeMinute = endTime.substr(2, 2);

    std::string messageData = facilityName + "," + dayOfWeek + "," + startTimeHour + "," + startTimeMinute + "," + endTimeHour + "," + endTimeMinute; // Booking request for the specified facility, day, and times

    RequestMessage requestMessage(RequestMessage::WRITE, requestID, messageData); // WRITE operation

    return sendWithRetry(requestMessage); // Send the request with retry (in case of timeout)
}

/**
 * @brief Queries the details of an existing booking.
 * 
 * This method sends a request to the server to retrieve the details of an existing booking.
 * 
 * @param bookingID The ID of the booking to query.
 * 
 * @return A string containing the booking details or an error message.
 */
std::string Client::queryBooking(std::string bookingID)
{
    std::string messageData = "booking," + bookingID; // Request booking details for the specified ID

    RequestMessage requestMessage(RequestMessage::READ, requestID, messageData); // READ operation

    return sendWithRetry(requestMessage); // Send the request with retry (in case of timeout)
}

/**
 * @brief Updates an existing booking by applying a time offset.
 * 
 * This method sends a request to the server to update an existing booking by applying a time offset.
 * The new start and end times are first calculated based on the old booking details and the offset.
 * Then, the update request containing the old booking ID, facility name, day of the week, and new times are sent to the server.
 * 
 * @param oldBookingID The ID of the booking to update.
 * @param offsetMinutes The time offset in minutes (positive for later, negative for earlier).
 * @param oldBookingDetails The details of the old booking to be updated.
 * 
 * @return A string containing the updated booking details or an error message.
 */
std::string Client::updateBooking(
    std::string oldBookingID,
    int offsetMinutes,
    std::string oldBookingDetails)
{
    // Extract facility name, day of week, start time, and end time from the old booking details
    std::string facilityName = extractFacilityName(oldBookingDetails);
    std::string oldDayOfWeek = extractDayOfWeek(oldBookingDetails);
    std::string oldStartTime = extractStartTime(oldBookingDetails);
    std::string oldEndTime = extractEndTime(oldBookingDetails);

    // Parse old start and end times into hours and minutes
    int oldStartHour = std::stoi(oldStartTime.substr(0, 2));
    int oldStartMinute = std::stoi(oldStartTime.substr(2, 2));
    int oldEndHour = std::stoi(oldEndTime.substr(0, 2));
    int oldEndMinute = std::stoi(oldEndTime.substr(2, 2));

    // Apply offset
    int totalStartMinutes = oldStartHour * 60 + oldStartMinute + offsetMinutes;
    int totalEndMinutes = oldEndHour * 60 + oldEndMinute + offsetMinutes;

    // Calculate new start and end times
    int newStartHour = totalStartMinutes / 60;
    int newStartMinute = totalStartMinutes % 60;
    int newEndHour = totalEndMinutes / 60;
    int newEndMinute = totalEndMinutes % 60;

    // Now we can proceed with the change booking request
    std::string messageData = (
        "booking," +
        oldBookingID + "," +
        facilityName + "," +
        oldDayOfWeek + "," +
        std::to_string(newStartHour) + "," +
        std::to_string(newStartMinute) + "," +
        std::to_string(newEndHour) + "," +
        std::to_string(newEndMinute)
    );

    RequestMessage requestMessage(RequestMessage::UPDATE, requestID, messageData); // UPDATE operation

    return sendWithRetry(requestMessage); // Send the request with retry (in case of timeout)
}

/**
 * @brief Deletes an existing booking.
 * 
 * This method sends a request to the server to delete an existing booking.
 * 
 * @param bookingID The ID of the booking to delete.
 * @param bookingDetails The details of the booking to delete.
 * 
 * @return A string containing the deletion confirmation or an error message.
 */
std::string Client::deleteBooking(std::string bookingID, std::string bookingDetails)
{
    // Extract the facility name from the existing booking as it is required for the delete booking request
    std::string facilityName = extractFacilityName(bookingDetails);

    std::string messageData = bookingID + "," + facilityName; // Request to delete the booking with the specified ID and facility name

    RequestMessage requestMessage(RequestMessage::DELETE_REQUEST, requestID, messageData); // DELETE (enum named as DELETE_REQUEST) operation

    return sendWithRetry(requestMessage);
}

/**
 * @brief Monitors the availability of a facility for a specified duration.
 * 
 * This method sends a request to the server to register the client's interest in monitoring the availability of a facility.
 * The request message includes the facility to monitor and the duration in seconds.
 * The server will respond with availability updates, if any, during the monitoring period.
 * During the monitoring period, the client is blocked from executing other operations.
 * 
 * @param facilityName The name of the facility to monitor.
 * @param durationSeconds The duration to monitor in seconds.
 * @param onUpdate Callback function to handle updates during monitoring.
 * 
 * @note The boolean in the callback function indicates whether the update message is a registration response. This is required for the callback function to handle the two different types of responses (registration and updates) correctly.
 */
void Client::monitorAvailability(
    std::string facilityName,
    int durationSeconds,
    const std::function<void(const std::string &, const bool)> &onUpdate
)
{
    std::string messageData = "register," + facilityName + "," + std::to_string(durationSeconds); // Request to register for monitoring the specified facility for the specified duration

    RequestMessage requestMessage(RequestMessage::MONITOR, requestID, messageData); // MONITOR operation

    std::string registrationResponse = sendWithRetry(requestMessage);
    onUpdate(registrationResponse, true); // Call the callback function with the registration response

    // Check if the registration was successful
    if (registrationResponse.find(Constants::STATUS_SUCCESS) == 0)
    {
        // If successful, listen for updates for the specified duration
        // Client is blocked from making other requests during this time
        listenForMonitoringUpdates(durationSeconds, [onUpdate](const std::string &update, bool isRegistrationResponse)
        {
            onUpdate(update, false);
        });
    }
}

/**
 * @brief Rates a facility.
 * 
 * This method sends a request to the server to rate a facility.
 * 
 * @param facilityName The name of the facility to rate.
 * @param rating The rating value (e.g., 4.5).
 * 
 * @return A string containing the rating confirmation or error message.
 */
std::string Client::rateFacility(std::string facilityName, float rating)
{
    std::string messageData = "rating," + facilityName + "," + std::to_string(rating); // Request to rate the facility with the specified name and rating

    RequestMessage requestMessage(RequestMessage::UPDATE, requestID, messageData); // UPDATE operation

    return sendWithRetry(requestMessage); // Send the request with retry (in case of timeout)
}

/**
 * @brief Queries the rating of a facility.
 * 
 * This method sends a request to the server to retrieve the average rating of a facility.
 * 
 * @param facilityName The name of the facility to query.
 * 
 * @return A string containing the rating information or error message.
 */
std::string Client::queryRating(std::string facilityName)
{
    std::string messageData = "rating," + facilityName;

    RequestMessage requestMessage(RequestMessage::READ, requestID, messageData);

    return sendWithRetry(requestMessage);
}

/**
 * @brief Sends an echo message to the server and receives the response.
 * 
 * This method sends a message to the server and waits for the echoed response.
 * 
 * @param messageData The message data to send.
 * 
 * @return A string containing the echoed message from the server or error message.
 */
std::string Client::echoMessage(std::string messageData)
{
    RequestMessage requestMessage(RequestMessage::ECHO, requestID, messageData);

    return sendWithRetry(requestMessage);
}

/**
 * @brief Creates a local socket address.
 * 
 * This method initializes the local socket address structure with the specified parameters.
 * 
 * @param sa Pointer to the sockaddr_in structure to initialize.
 */
void Client::makeLocalSocketAddress(struct sockaddr_in *sa)
{
    sa->sin_family = AF_INET;
    sa->sin_port = htons(8000);                 // Set client port to 8000
    sa->sin_addr.s_addr = htonl(INADDR_ANY);    // On local host
}

/**
 * @brief Creates a remote socket address from a hostname and port.
 * 
 * This method initializes the remote socket address structure with the specified hostname and port.
 * It first tries to convert the hostname to an IP address. If that fails, it resolves the hostname using gethostbyname.
 * This is necessary for the client to be able to communicate with the server.
 * 
 * @param sa Pointer to the sockaddr_in structure to initialize.
 * @param hostname The hostname or IP address of the server.
 * @param port The port number of the server.
 */
void Client::makeRemoteSocketAddress(struct sockaddr_in *sa, char *hostname, int port)
{
    sa->sin_family = AF_INET;
    sa->sin_port = htons(port);

    // Try to convert IP address directly
    sa->sin_addr.s_addr = inet_addr(hostname);

    // If not an IP address, try to resolve hostname
    if (sa->sin_addr.s_addr == INADDR_NONE)
    {
        struct hostent *host;
        if ((host = gethostbyname(hostname)) == NULL)
        {
            std::cerr << "Unknown host" << std::endl;
            exit(-1);
        }
        memcpy(&(sa->sin_addr), host->h_addr, host->h_length);
    }
}

/**
 * @brief Sends a request message to the server.
 * 
 * This method serializes the request message and sends it to the server.
 * It increments the request ID for each new request.
 * 
 * @param request The request message to send.
 * @param retry Whether this is a retry attempt.
 * 
 * @note The retry flag is used to determine whether to increment the request ID or not.
 */
void Client::sendRequest(const RequestMessage &request, bool retry)
{
    if (!retry)
    {
        requestID++;
    }

    std::vector<uint8_t> serializedData = JavaSerializer::serialize(&request);

    try
    {
        socket.sendDataTo(serializedData, serverAddr);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error sending data: " << e.what() << std::endl;
        exit(1);
    }
}

/**
 * @brief Receives a response from the server and verifies the request ID.
 * 
 * This method receives a response from the server, deserializes it, and checks if the request ID matches the expected one.
 * If it doesn't match, it returns an empty string to trigger a retry.
 * 
 * @param expectedRequestID The expected request ID for the response.
 * 
 * @return A string containing the response message from the server or error message.
 */
std::string Client::receiveResponse(uint32_t expectedRequestID)
{
    char recvBuffer[Constants::BUFFER_SIZE];
    struct sockaddr_in senderAddr;
    std::string messageData;

    try
    {
        int bytesReceived = socket.receiveDataFrom(recvBuffer, senderAddr);

        // Deserialize response
        std::vector<uint8_t> receivedData(recvBuffer, recvBuffer + bytesReceived);
        std::shared_ptr<JavaSerializable> deserializedObj = JavaDeserializer::deserialize(receivedData);

        std::shared_ptr<RequestMessage> responseMessage = std::dynamic_pointer_cast<RequestMessage>(deserializedObj);
        
        // Verify the response matches our request ID
        if (responseMessage->getRequestID() != expectedRequestID) {
            std::cerr << "Received response for request ID " 
                      << responseMessage->getRequestID() 
                      << " but expected " << expectedRequestID << std::endl;
            return ""; // Return empty string to trigger retry
        }
        
        messageData = responseMessage->getData();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return messageData;
}

/**
 * @brief Sends a request to the server with retries.
 * 
 * This method attempts to send a request to the server multiple times in case of timeouts or no response.
 * It uses a maximum number of retries defined by MAX_RETRIES in Constants.hpp.
 * 
 * @param request The request message to send.
 * 
 * @return A string containing the response message from the server or error message.
 */
std::string Client::sendWithRetry(const RequestMessage &request)
{
    for (int attempt = 0; attempt < Constants::MAX_RETRIES; ++attempt)
    {
        sendRequest(request, attempt > 0); // Retry flag is false for first attempt and true for subsequent attempts

        // std::string response = receiveResponse();
        std::string response = receiveResponse(request.getRequestID());

        if (!response.empty())
        {
            return response;
        }

        std::cerr << "No response received. Retrying... (" << (attempt + 1) << "/" << Constants::MAX_RETRIES << ")" << std::endl;
    }

    return Constants::STATUS_ERROR + "\nmessage:Request failed after " + std::to_string(Constants::MAX_RETRIES) + " attempts.";
}

/**
 * @brief Extracts the facility name from the booking details string.
 * 
 * This method searches for the facility name in the booking details string.
 * It looks for the prefix "facility:" and extracts the name that follows it.
 * 
 * @param bookingDetails The booking details string.
 * 
 * @return The extracted facility name.
 */
std::string Client::extractFacilityName(const std::string &bookingDetails)
{
    const std::string facilityPrefix = "facility:";
    size_t pos = bookingDetails.find(facilityPrefix);

    if (pos != std::string::npos)
    {
        size_t start = pos + facilityPrefix.length();
        size_t end = bookingDetails.find('\n', start);
        return bookingDetails.substr(start, end - start);
    }

    return "";
}

/**
 * @brief Extracts the day of the week from the booking details string.
 * 
 * This method searches for the day of the week in the booking details string.
 * It looks for the prefix "day:" and extracts the name that follows it.
 * 
 * @param bookingDetails The booking details string.
 * 
 * @return The extracted day of the week.
 */
std::string Client::extractDayOfWeek(const std::string &bookingDetails)
{
    const std::string dayPrefix = "day:";
    size_t pos = bookingDetails.find(dayPrefix);

    if (pos != std::string::npos)
    {
        size_t start = pos + dayPrefix.length();
        size_t end = bookingDetails.find('\n', start);
        return bookingDetails.substr(start, end - start);
    }

    return "";
}

/**
 * @brief Extracts the start time from the booking details string.
 * 
 * This method searches for the start time in the booking details string.
 * It looks for the prefix "startTime:" and extracts the time that follows it.
 * 
 * @param bookingDetails The booking details string.
 * 
 * @return The extracted start time in HHMM format.
 */
std::string Client::extractStartTime(const std::string &bookingDetails)
{
    const std::string startTimePrefix = "startTime:";
    size_t pos = bookingDetails.find(startTimePrefix);

    if (pos != std::string::npos)
    {
        size_t start = pos + startTimePrefix.length();
        size_t end = bookingDetails.find('\n', start);
        return bookingDetails.substr(start, end - start);
    }

    return "";
}

/**
 * @brief Extracts the end time from the booking details string.
 * 
 * This method searches for the end time in the booking details string.
 * It looks for the prefix "endTime:" and extracts the time that follows it.
 * 
 * @param bookingDetails The booking details string.
 * 
 * @return The extracted end time in HHMM format.
 */
std::string Client::extractEndTime(const std::string &bookingDetails)
{
    const std::string endTimePrefix = "endTime:";
    size_t pos = bookingDetails.find(endTimePrefix);

    if (pos != std::string::npos)
    {
        size_t start = pos + endTimePrefix.length();
        size_t end = bookingDetails.find('\n', start);
        return bookingDetails.substr(start, end - start);
    }

    return "";
}

/**
 * @brief Listens for monitoring updates from the server.
 * 
 * This method listens for updates from the server during the monitoring period.
 * It uses a timeout to break out of the loop after the specified duration.
 * 
 * @param durationSeconds The duration to monitor in seconds.
 * @param onUpdate Callback function to handle updates during monitoring.
 */
void Client::listenForMonitoringUpdates(
    int durationSeconds,
    const std::function<void(const std::string &, const bool)> &onUpdate
)
{
    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::seconds(durationSeconds);

    socket.setReceiveTimeout(durationSeconds); // Set timeout to the duration of monitoring

    while (std::chrono::steady_clock::now() < endTime)
    {
        char recvBuffer[Constants::BUFFER_SIZE];
        struct sockaddr_in senderAddr;

        try
        {
            int bytesReceived = socket.receiveDataFrom(recvBuffer, senderAddr);

            if (bytesReceived > 0)
            {
                std::string response(recvBuffer, bytesReceived);
                onUpdate(response, false);
            }
        }
        catch (const std::exception &e)
        {
            // Socket timeout aligns with the duration of monitoring, hence we can safely break out of loop
            break;
        }
    }

    socket.setReceiveTimeout(Constants::TIMEOUT_SEC);
}
