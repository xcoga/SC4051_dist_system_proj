#include "Client.hpp"

#include "Serializer.hpp"
#include "UserInterface.hpp"

Client::Client(const std::string &serverIp, int serverPort) : requestID(0)
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

std::string Client::queryAvailability(std::string facilityName)
{
    std::string messageData = "facility," + facilityName;

    RequestMessage requestMessage(RequestMessage::READ, requestID, messageData);

    return sendWithRetry(requestMessage);
}

std::string Client::bookFacility(
    std::string facilityName,
    std::string dayOfWeek,
    std::string startTime,
    std::string endTime
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

std::string Client::queryBooking(std::string bookingID)
{
    std::string messageData = "booking," + bookingID;

    RequestMessage requestMessage(RequestMessage::READ, requestID, messageData);

    return sendWithRetry(requestMessage);
}

std::string Client::updateBooking(
    std::string oldBookingID,
    std::string newDayOfWeek,
    std::string newStartTime,
    std::string newEndTime
)
{
    // Make queryBooking call first to get the old booking details, especially the facility name
    // Facility name is required for the change booking request
    std::string oldBookingDetails = queryBooking(oldBookingID);
    std::string facilityName = extractFacilityName(oldBookingDetails);

    // Now we can proceed with the change booking request
    std::string startTimeHour, startTimeMinute, endTimeHour, endTimeMinute;
    startTimeHour = newStartTime.substr(0, 2);
    startTimeMinute = newStartTime.substr(2, 2);
    endTimeHour = newEndTime.substr(0, 2);
    endTimeMinute = newEndTime.substr(2, 2);

    std::string messageData = "booking," + oldBookingID + "," + facilityName + "," + newDayOfWeek + "," + startTimeHour + "," + startTimeMinute + "," + endTimeHour + "," + endTimeMinute;

    RequestMessage requestMessage(RequestMessage::UPDATE, requestID, messageData);

    return sendWithRetry(requestMessage);
}

std::string Client::deleteBooking(std::string bookingID)
{
    // Make queryBooking call first to get the old booking details, especially the facility name
    // Facility name is required for the delete booking request
    std::string bookingDetails = queryBooking(bookingID);
    std::string facilityName = extractFacilityName(bookingDetails);

    std::string messageData = bookingID + "," + facilityName;
    
    RequestMessage requestMessage(RequestMessage::DELETE, requestID, messageData);

    return sendWithRetry(requestMessage);
}

void Client::monitorAvailability(
    std::string facilityName,
    int durationSeconds,
    const std::function<void(const std::string &, const bool)> &onUpdate
)
{
    std::string messageData = "register," + facilityName + "," + std::to_string(durationSeconds);

    RequestMessage requestMessage(RequestMessage::MONITOR, requestID, messageData);

    std::string registrationResponse = sendWithRetry(requestMessage);
    std::cout << "HERE" << std::endl;
    onUpdate(registrationResponse, true);       // TODO: onUpdate() calls result in trace trap
    std::cout << "HERE2" << std::endl;

    if (registrationResponse.find("status:SUCCESS") == 0)
    {
        listenForMonitoringUpdates(durationSeconds, [onUpdate](const std::string &update, bool isRegistrationResponse)
        {
            onUpdate(update, false);
        });
    }
}

std::string Client::rateFacility(std::string facilityName, float rating)
{
    std::string messageData = "rating," + facilityName + "," + std::to_string(rating);

    RequestMessage requestMessage(RequestMessage::UPDATE, requestID, messageData);

    return sendWithRetry(requestMessage);
}

std::string Client::queryRating(std::string facilityName)
{
    std::string messageData = "rating," + facilityName;

    RequestMessage requestMessage(RequestMessage::READ, requestID, messageData);

    return sendWithRetry(requestMessage);
}

std::string Client::echoMessage(std::string messageData)
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

std::string Client::receiveResponse()
{
    char recvBuffer[BUFFER_SIZE];
    struct sockaddr_in senderAddr;
    std::string messageData;

    try
    {
        int bytesReceived = socket.receiveDataFrom(recvBuffer, senderAddr);

        // TODO: Check parity

        // Deserialize response
        std::vector<uint8_t> receivedData(recvBuffer, recvBuffer + bytesReceived);
        std::shared_ptr<JavaSerializable> deserializedObj = JavaDeserializer::deserialize(receivedData);

        std::shared_ptr<RequestMessage> responseMessage = std::dynamic_pointer_cast<RequestMessage>(deserializedObj);
        messageData = responseMessage->getData();
    }
    catch(const std::exception& e)
    {
        return "status:ERROR\nmessage:" + std::string(e.what());
    }

    return messageData;
}

std::string Client::sendWithRetry(const RequestMessage &request)
{
    for (int attempt = 0; attempt < MAX_RETRIES; ++attempt)
    {
        sendRequest(request, attempt > 0); // Retry flag is false for first attempt and true for subsequent attempts

        std::string response = receiveResponse();

        if (response.find("status:ERROR") != 0) // If no error, return the response
        {
            return response;
        }

        std::cerr << "Error response received. Retrying... (" << (attempt + 1) << "/" << MAX_RETRIES << ")" << std::endl;
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

void Client::listenForMonitoringUpdates(
    int durationSeconds,
    const std::function<void(const std::string &, const bool)> &onUpdate
)
{
    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::seconds(durationSeconds);

    while (std::chrono::steady_clock::now() < endTime)
    {
        onUpdate(receiveResponse(), false);

        // char recvBuffer[BUFFER_SIZE];
        // struct sockaddr_in senderAddr;

        // try
        // {
        //     int bytesReceived = socket.receiveDataFrom(recvBuffer, senderAddr);

        //     if (bytesReceived > 0)
        //     {
        //         std::string response(recvBuffer, bytesReceived);
        //         onUpdate(response);
        //     }
        // }
        // catch (const std::exception &e)
        // {
        //     std::cerr << "Error receiving monitoring update: " << e.what() << std::endl;
        //     break;
        // }
    }
}
