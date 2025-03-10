#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <winsock2.h> // Include winsock2.h for SOCKET_ERROR definition

#include "./utils/Serializer.hpp"
#include "./utils/Parity.hpp"

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class RequestMessage : public JavaSerializable
{
private:
    int requestType;
    int requestID;
    std::string data;

public:
    // Default constructor (required for deserialization)
    RequestMessage() : requestType(0), requestID(0), data("") {}

    // Main constructor
    RequestMessage(int requestType, int requestID, const std::string &data)
        : requestType(requestType), requestID(requestID), data(data) {}

    std::string getJavaClassName() const override
    {
        return "Server.RequestMessage";
    }

    std::vector<std::pair<std::string, std::string>> getFieldMetadata() const override
    {
        return {
            {"requestType", "int"},
            {"requestID", "int"},
            {"data", "java.lang.String"}};
    }

    // This is now empty - use serializeFieldValues instead
    void serializeFields(ByteBuffer &buffer) const override
    {
        // Empty - not used
    }

    // New method that writes values in the right order
    void serializeFieldValues(ByteBuffer &buffer) const override
    {
        // First write ALL primitive values
        buffer.writeInt(requestType);
        buffer.writeInt(requestID);

        // Then write ALL object values
        if (data.empty())
        {
            buffer.writeByte(0); // null marker
        }
        else
        {
            buffer.writeByte(1); // non-null marker
            buffer.writeString(data);
        }
    }

    void deserializeFields(ByteReader &reader) override
    {
        // First read all primitives
        requestType = reader.readInt();
        requestID = reader.readInt();

        // Then read all objects
        uint8_t dataNullMarker = reader.readByte();
        if (dataNullMarker == 1)
        {
            data = reader.readString();
        }
        else
        {
            data = "";
        }
    }

    // Getters
    int getRequestType() const { return requestType; }
    int getRequestID() const { return requestID; }
    std::string getData() const { return data; }

    // Helper method to match Java functionality
    // Operation getOperation() const { return Operation::fromInt(requestType); }

    // Setters
    void setRequestType(int type) { requestType = type; }
    void setRequestID(int id) { requestID = id; }
    void setData(const std::string &d) { data = d; }
};

void makeLocalSocketAddress(struct sockaddr_in *sa)
{
    sa->sin_family = AF_INET;
    sa->sin_port = htons(0);                 // Let the system choose a port
    sa->sin_addr.s_addr = htonl(INADDR_ANY); // On local host
}

void makeRemoteSocketAddress(struct sockaddr_in *sa, char *hostname, int port)
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
            cout << "Unknown host" << endl;
            exit(-1);
        }
        memcpy(&(sa->sin_addr), host->h_addr, host->h_length);
    }
}

void printSA(struct sockaddr_in sa)
{
    char *ipStr = inet_ntoa(sa.sin_addr);
    cout << "sa = " << sa.sin_family << ", " << ipStr << ", " << ntohs(sa.sin_port) << endl;
}

int testSerializationLocally()
{
    // Register the RequestMessage class with the factory
    ObjectFactory::creators["Server.RequestMessage"] = []()
    {
        return std::make_shared<RequestMessage>();
    };

    // Create a test RequestMessage object
    RequestMessage requestMessage(1, 1, "Hello from test!");

    // Serialize the object
    vector<uint8_t> serializedData = JavaSerializer::serialize(&requestMessage);

    cout << "Serialized data size (including parity bit): " << serializedData.size() << " bytes" << endl;

    // Extract message data and parity bit
    vector<uint8_t> messageData(serializedData.begin(), serializedData.end() - 1);
    uint8_t parityBit = serializedData.back();

    cout << "Message data (" << messageData.size() << " bytes): ";
    for (uint8_t byte : messageData)
    {
        cout << hex << (int)byte << " ";
    }
    cout << endl;

    cout << "Parity bit: 0x" << hex << (int)parityBit << dec << endl;

    // Verify the calculated parity bit
    uint8_t calculatedParity = Parity::calculateEvenParityBit(messageData);
    cout << "Calculated parity: 0x" << hex << (int)calculatedParity << dec;
    cout << " (" << (calculatedParity == parityBit ? "MATCHES" : "ERROR: DOES NOT MATCH") << ")" << endl;

    // Deserialize the object
    try
    {
        std::shared_ptr<JavaSerializable> deserializedObj = JavaDeserializer::deserialize(serializedData);
        std::shared_ptr<RequestMessage> responseMessage = std::dynamic_pointer_cast<RequestMessage>(deserializedObj);

        // Display deserialized message
        cout << "Deserialized message:" << endl;
        cout << "Request ID: " << responseMessage->getRequestID() << endl;
        cout << "Message Type: " << responseMessage->getRequestType() << endl;
        cout << "Message: " << responseMessage->getData() << endl;
    }
    catch (const std::exception &e)
    {
        cout << "Error deserializing response: " << e.what() << endl;
    }

    return 0;
}

int main()
{
    // Register the RequestMessage class with the factory
    ObjectFactory::creators["Server.RequestMessage"] = []()
    {
        return std::make_shared<RequestMessage>();
    };

    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        cout << "WSAStartup failed: " << result << endl;
        return 1;
    }

    SOCKET s;
    int clientPort, serverPort;
    char serverHostname[100];
    struct sockaddr_in clientAddr, serverAddr;

    cout << "Enter server hostname or IP address: ";
    cin >> serverHostname;
    cout << "Enter server port: ";
    cin >> serverPort;

    // Create socket
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET)
    {
        cout << "Error creating socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Set client address
    makeLocalSocketAddress(&clientAddr);
    if (bind(s, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR)
    {
        cout << "Error binding socket: " << WSAGetLastError() << endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    // Get the assigned port
    int addrLen = sizeof(clientAddr);
    getsockname(s, (struct sockaddr *)&clientAddr, &addrLen);
    printSA(clientAddr);

    // Set server address
    makeRemoteSocketAddress(&serverAddr, serverHostname, serverPort);
    printSA(serverAddr);

    // Format message into required structure and serialize before sending
    RequestMessage requestMessage(1, 1, "Hello from client!");
    vector<uint8_t> serializedData = JavaSerializer::serialize(&requestMessage);

    // The serialize method now automatically adds the parity bit at the end
    // Display the serialized data with parity bit
    cout << "Serialized data size (including parity bit): " << serializedData.size() << " bytes" << endl;

    // Extract and show the data and parity bit separately for debugging
    vector<uint8_t> messageData(serializedData.begin(), serializedData.end() - 1);
    uint8_t parityBit = serializedData.back();

    cout << "Message data (" << messageData.size() << " bytes): ";
    for (size_t i = 0; i < messageData.size(); i++)
    {
        cout << hex << (int)messageData[i] << " ";
    }
    cout << endl;

    cout << "Parity bit: 0x" << hex << (int)parityBit << dec << endl;

    // Verify that our parity calculation is correct
    uint8_t calculatedParity = Parity::calculateEvenParityBit(messageData);
    cout << "Calculated parity: 0x" << hex << (int)calculatedParity << dec;
    cout << " (" << (calculatedParity == parityBit ? "MATCHES" : "ERROR: DOES NOT MATCH") << ")" << endl;

    // Send the serialized data (with parity bit) to the server
    if (sendto(s, (const char *)serializedData.data(), (int)serializedData.size(), 0,
               (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cout << "Error sending data: " << WSAGetLastError() << endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    cout << "Sent serialized RequestMessage with parity bit" << endl;

    // Set up for receiving response
    cout << "Waiting for server response..." << endl;

    // Buffer to receive data
    const int BUFFER_SIZE = 2048;
    char recvBuffer[BUFFER_SIZE];

    // Set up for address of sender
    struct sockaddr_in senderAddr;
    int senderAddrLen = sizeof(senderAddr);

    // Receive data
    int bytesReceived = recvfrom(s, recvBuffer, BUFFER_SIZE, 0,
                                 (struct sockaddr *)&senderAddr, &senderAddrLen);

    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "Error receiving data: " << WSAGetLastError() << endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    cout << "Received " << bytesReceived << " bytes from ";
    printSA(senderAddr);

    // Extract and verify parity from the received data for logging purposes
    if (bytesReceived > 0)
    {
        vector<uint8_t> receivedData(recvBuffer, recvBuffer + bytesReceived);
        vector<uint8_t> receivedMessageData(receivedData.begin(), receivedData.end() - 1);
        uint8_t receivedParityBit = receivedData.back();

        cout << "Received message data size: " << receivedMessageData.size() << " bytes" << endl;
        cout << "Received parity bit: 0x" << hex << (int)receivedParityBit << dec << endl;

        uint8_t calculatedReceivedParity = Parity::calculateEvenParityBit(receivedMessageData);
        cout << "Calculated parity for received data: 0x" << hex << (int)calculatedReceivedParity << dec;
        cout << " (" << (calculatedReceivedParity == receivedParityBit ? "VALID" : "INVALID - possible data corruption") << ")" << endl;
    }

    // Deserialize the response - now handles parity check internally
    try
    {
        vector<uint8_t> receivedData(recvBuffer, recvBuffer + bytesReceived);

        // The deserialize method now automatically extracts and verifies the parity bit
        std::shared_ptr<JavaSerializable> deserializedObj = JavaDeserializer::deserialize(receivedData);

        // Cast to RequestMessage if that's what we're expecting
        std::shared_ptr<RequestMessage> responseMessage = std::dynamic_pointer_cast<RequestMessage>(deserializedObj);

        // Display deserialized message
        cout << "Deserialized message:" << endl;
        cout << "Request ID: " << responseMessage->getRequestID() << endl;
        cout << "Message Type: " << responseMessage->getRequestType() << endl;
        cout << "Message: " << responseMessage->getData() << endl;
    }
    catch (const std::exception &e)
    {
        cout << "Error deserializing response: " << e.what() << endl;
    }

    // Clean up
    closesocket(s);
    WSACleanup();

    return 0;
}