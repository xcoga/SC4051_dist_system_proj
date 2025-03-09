#include "Client.hpp"

#include "Serializer.hpp"

Client::Client(const std::string &serverIp, int serverPort)
{
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    makeLocalSocketAddress(&clientAddr);

    if (::bind(s, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) < 0)
    {
        perror("Error binding socket");
        exit(1);
    }

    printBoundSocketInfo();
    
    makeRemoteSocketAddress(&serverAddr, const_cast<char *>(serverIp.c_str()), serverPort);
}

Client::~Client()
{
    close(s);
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
    // Get the assigned port and IP address
    socklen_t addrLen = sizeof(clientAddr);
    if (getsockname(s, (struct sockaddr *)&clientAddr, &addrLen) < 0)
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

    if (sendto(s, (const char *)serializedData.data(), (int)serializedData.size(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Error sending data");
        exit(1);
    }
}

std::string Client::receiveResponse()
{
    uint8_t buffer[BUFFER_SIZE];

    socklen_t serverAddrLen = sizeof(serverAddr);
    int bytesReceived = recvfrom(s, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);

    if (bytesReceived < 0)
    {
        perror("Error receiving data");
        exit(1);
    }

    std::vector<uint8_t> receivedData(buffer, buffer + bytesReceived);
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