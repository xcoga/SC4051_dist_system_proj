#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>

#include "./include/Serializer.hpp"

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

int main()
{
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
    // serializedData.push_back(0x21);

    cout << "Serialized data size after append: " << serializedData.size() << " bytes" << endl;
    for (size_t i = 0; i < serializedData.size(); i++)
    {
        cout << hex << (int)serializedData[i] << " ";
    }
    cout << endl;

    // Send the serialized data to the server
    if (sendto(s, (const char *)serializedData.data(), (int)serializedData.size(), 0,
               (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cout << "Error sending data: " << WSAGetLastError() << endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    cout << "Sent serialized RequestMessage" << endl;

    // Clean up
    closesocket(s);
    WSACleanup();

    return 0;
}