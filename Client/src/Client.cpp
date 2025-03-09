#include "Client.hpp"

#include "Serializer.hpp"

Client::Client(const std::string &serverIp, int serverPort)
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

    printBoundSocketInfo();
    
    makeRemoteSocketAddress(&serverAddr, const_cast<char *>(serverIp.c_str()), serverPort);
}

Client::~Client()
{
    socket.closeSocket();
}

void Client::sendMessage()
{
    RequestMessage requestMessage(1, 0, "Hello from client");
    sendRequest(requestMessage);

    std::string response = receiveResponse();
}

void Client::queryAvailability()
{
    // TODO: Implement query availability functionality
}

void Client::bookFacility()
{
    // TODO: Implement book facility functionality
}

void Client::changeBooking()
{
    // TODO: Implement change booking functionality
}

void Client::monitorAvailability()
{
    // TODO: Implement monitor availability functionality
}

// TODO: Implement idempotent and non-idempotent operations

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

void Client::printBoundSocketInfo()
{
    if (socket.getSocketName((struct sockaddr *)&clientAddr) < 0)
    {
        perror("Error getting socket name");
        exit(1);
    }

    // Print the bound IP address and port
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
    std::cout << "Client socket bound to IP: " << ipStr << ", Port: " << ntohs(clientAddr.sin_port) << std::endl;
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

    int bytesReceived = 0;

    try
    {
        buffer.resize(BUFFER_SIZE);
        bytesReceived = socket.receiveDataFrom(buffer, serverAddr);
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error receiving data: " << e.what() << std::endl;
        exit(1);
    }

    std::vector<uint8_t> receivedData(buffer.begin(), buffer.begin() + bytesReceived);
    auto deserializedObject = JavaDeserializer::deserialize(receivedData);

    // Process the deserialized object (for example, print out some fields)
    if (deserializedObject)
    {
        std::cout << "Received Object of Class: " 
                  << deserializedObject->getJavaClassName() << std::endl;

        // Cast to your specific class (e.g., RequestMessage)
        RequestMessage *response = dynamic_cast<RequestMessage *>(deserializedObject.get());
        if (response)
        {
            std::cout << "Request ID: " << response->getRequestID() << std::endl;
            std::cout << "Request Data: " << response->getData() << std::endl;
        }
        else
        {
            std::cout << "Failed to cast to RequestMessage" << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to deserialize the received data" << std::endl;
    }

    return "";
}