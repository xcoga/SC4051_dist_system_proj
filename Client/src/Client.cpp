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

    RequestMessage requestMessage(RequestMessage::READ, requestID++, messageData);
    sendRequest(requestMessage);

    return receiveResponse();
}

std::string Client::queryAvailability(std::string facilityName)
{
    std::string messageData = "facility," + facilityName;

    RequestMessage requestMessage(RequestMessage::READ, requestID++, messageData);
    sendRequest(requestMessage);

    return receiveResponse();
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

    RequestMessage requestMessage(RequestMessage::WRITE, requestID++, messageData);
    sendRequest(requestMessage);

    return receiveResponse();
}

std::string Client::queryBooking(std::string bookingID)
{
    std::string messageData = "booking," + bookingID;

    RequestMessage requestMessage(RequestMessage::READ, requestID++, messageData);
    sendRequest(requestMessage);

    return receiveResponse();
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

    RequestMessage requestMessage(RequestMessage::UPDATE, requestID++, messageData);
    sendRequest(requestMessage);

    return receiveResponse();
}

std::string Client::deleteBooking(std::string bookingID)
{
    // Make queryBooking call first to get the old booking details, especially the facility name
    // Facility name is required for the delete booking request
    std::string bookingDetails = queryBooking(bookingID);
    std::string facilityName = extractFacilityName(bookingDetails);

    std::string messageData = bookingID + "," + facilityName;
    
    RequestMessage requestMessage(RequestMessage::DELETE, requestID++, messageData);
    sendRequest(requestMessage);

    return receiveResponse();
}

std::string Client::monitorAvailability(std::string facilityName)
{
    // TODO: Implement monitor availability functionality
    // Client to keep track of duration then block subsequent requests throughout the duration
    return "";
}

std::string Client::rateFacility(std::string facilityName, float rating)
{
    std::string messageData = "rating," + facilityName + "," + std::to_string(rating);

    RequestMessage requestMessage(RequestMessage::UPDATE, requestID++, messageData);
    sendRequest(requestMessage);

    return receiveResponse();
}

std::string Client::queryRating(std::string facilityName)
{
    std::string messageData = "rating," + facilityName;

    RequestMessage requestMessage(RequestMessage::READ, requestID++, messageData);
    sendRequest(requestMessage);

    return receiveResponse();
}

// TODO: Need to simulate repeated request operations, so requestID should not increment

std::string Client::echoMessage(std::string messageData)
{
    RequestMessage requestMessage(RequestMessage::ECHO, requestID++, messageData);
    sendRequest(requestMessage);

    return receiveResponse();
}

void Client::makeLocalSocketAddress(struct sockaddr_in *sa)
{
    sa->sin_family = AF_INET;
    sa->sin_port = htons(0);                 // Let the system choose a port
    sa->sin_addr.s_addr = htonl(INADDR_ANY); // On local host
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

void Client::sendRequest(const RequestMessage &request)
{
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
        // std::cerr << e.what() << '\n';
    }

    return messageData;
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
