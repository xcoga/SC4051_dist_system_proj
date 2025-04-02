#include "Client.hpp"

#include <chrono>

#include "Serializer.hpp"
#include "UserInterface.hpp"

Client::Client(const std::string &serverIp, const int serverPort) : requestID(0)
{
    try
    {
        socket.create(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error creating socket: " << e.what() << std::endl;
        exit(1);
    }

    makeLocalSocketAddress(&clientAddr);

    try
    {
        socket.bind(0);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error binding socket: " << e.what() << std::endl;
        exit(1);
    }

    makeRemoteSocketAddress(&serverAddr, const_cast<char *>(serverIp.c_str()), serverPort);

    socket.setReceiveTimeout(TIMEOUT_SEC);

    UserInterface::displayConnectionInfo(socket, serverAddr);
}

Client::~Client()
{
    socket.closeSocket();
}

std::string Client::queryFacilityNames()
{
    std::string messageData = "facility,ALL";

    RequestMessage requestMessage(RequestMessage::READ, requestID, messageData);

    return sendWithRetry(requestMessage);
}

std::string Client::queryAvailability(const std::string facilityName, const std::string daysOfWeek)
{
    std::string messageData = "facility," + facilityName + "," + daysOfWeek;

    RequestMessage requestMessage(RequestMessage::READ, requestID, messageData);

    return sendWithRetry(requestMessage);
}

std::string Client::bookFacility(
    const std::string facilityName,
    const std::string dayOfWeek,
    const std::string startTime,
    const std::string endTime
)
{
    std::string startTimeHour, startTimeMinute, endTimeHour, endTimeMinute;
    startTimeHour = startTime.substr(0, 2);
    startTimeMinute = startTime.substr(2, 2);
    endTimeHour = endTime.substr(0, 2);
    endTimeMinute = endTime.substr(2, 2);

    std::string messageData = facilityName + "," + dayOfWeek + "," + startTimeHour + "," + startTimeMinute + "," + endTimeHour + "," + endTimeMinute;

    RequestMessage requestMessage(RequestMessage::WRITE, requestID, messageData);    

    return sendWithRetry(requestMessage);
}

std::string Client::queryBooking(const std::string bookingID)
{
    std::string messageData = "booking," + bookingID;

    RequestMessage requestMessage(RequestMessage::READ, requestID, messageData);

    return sendWithRetry(requestMessage);
}

std::string Client::updateBooking(const std::string oldBookingID, const int offsetMinutes)
{
    // Make queryBooking call first to get the old booking details
    std::string oldBookingDetails = queryBooking(oldBookingID);
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

    RequestMessage requestMessage(RequestMessage::UPDATE, requestID, messageData);

    return sendWithRetry(requestMessage);
}

std::string Client::deleteBooking(const std::string bookingID)
{
    // Make queryBooking call first to get the old booking details, especially the facility name
    // Facility name is required for the delete booking request
    std::string bookingDetails = queryBooking(bookingID);
    std::string facilityName = extractFacilityName(bookingDetails);

    std::string messageData = bookingID + "," + facilityName;
    
    RequestMessage requestMessage(RequestMessage::DELETE_REQUEST, requestID, messageData);

    return sendWithRetry(requestMessage);
}

void Client::monitorAvailability(
    const std::string facilityName,
    const int durationSeconds,
    const std::function<void(const std::string &, const bool)> &onUpdate
)
{
    std::string messageData = "register," + facilityName + "," + std::to_string(durationSeconds);

    RequestMessage requestMessage(RequestMessage::MONITOR, requestID, messageData);

    std::string registrationResponse = sendWithRetry(requestMessage);
    onUpdate(registrationResponse, true);

    if (registrationResponse.find("status:SUCCESS") == 0)
    {
        listenForMonitoringUpdates(durationSeconds, [onUpdate](const std::string &update, bool isRegistrationResponse)
        {
            onUpdate(update, false);
        });
    }
}

std::string Client::rateFacility(const std::string facilityName, const float rating)
{
    std::string messageData = "rating," + facilityName + "," + std::to_string(rating);

    RequestMessage requestMessage(RequestMessage::UPDATE, requestID, messageData);

    return sendWithRetry(requestMessage);
}

std::string Client::queryRating(const std::string facilityName)
{
    std::string messageData = "rating," + facilityName;

    RequestMessage requestMessage(RequestMessage::READ, requestID, messageData);

    return sendWithRetry(requestMessage);
}

std::string Client::echoMessage(const std::string messageData)
{
    RequestMessage requestMessage(RequestMessage::ECHO, requestID, messageData);

    return sendWithRetry(requestMessage);
}

void Client::makeLocalSocketAddress(struct sockaddr_in *sa)
{
    sa->sin_family = AF_INET;
    sa->sin_port = htons(8000);                 // Let the system choose a port
    sa->sin_addr.s_addr = htonl(INADDR_ANY);    // On local host
}

void Client::makeRemoteSocketAddress(struct sockaddr_in *sa, const char *hostname, const int port)
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

void Client::sendRequest(const RequestMessage &request, const bool retry)
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

std::string Client::receiveResponse(const uint32_t expectedRequestID)
{
    char recvBuffer[BUFFER_SIZE];
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

std::string Client::sendWithRetry(const RequestMessage &request)
{
    for (int attempt = 0; attempt < MAX_RETRIES; ++attempt)
    {
        sendRequest(request, attempt > 0); // Retry flag is false for first attempt and true for subsequent attempts

        // std::string response = receiveResponse();
        std::string response = receiveResponse(request.getRequestID());


        if (!response.empty())
        {
            return response;
        }

        std::cerr << "No response received. Retrying... (" << (attempt + 1) << "/" << MAX_RETRIES << ")" << std::endl;
    }

    return "status:ERROR\nmessage:Request failed after " + std::to_string(MAX_RETRIES) + " attempts.";
}

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

void Client::listenForMonitoringUpdates(
    const int durationSeconds,
    const std::function<void(const std::string &, const bool)> &onUpdate
)
{
    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::seconds(durationSeconds);

    socket.setReceiveTimeout(durationSeconds); // Set timeout to the duration of monitoring

    while (std::chrono::steady_clock::now() < endTime)
    {
        char recvBuffer[BUFFER_SIZE];
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

    socket.setReceiveTimeout(TIMEOUT_SEC);
}
