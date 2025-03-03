#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int SERVER_PORT = 8000;

void makeLocalSocketAddress(struct sockaddr_in *sa)
{
    sa->sin_family = AF_INET;
    sa->sin_port = htons(SERVER_PORT);        // Let the system choose a port
    sa->sin_addr.s_addr = htonl(INADDR_ANY);        // On local host
}

int main()
{
    int s;
    struct sockaddr_in serverAddr;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cout << "Error creating socket" << std::endl;
        return 1;
    }

    // Set own address
    makeLocalSocketAddress(&serverAddr);
    if (bind(s, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cout << "Error binding socket" << std::endl;
        return 1;
    }

    std::cout << "Server started on port " << SERVER_PORT << std::endl;

    while (true)
    {
        char buffer[100];
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int n;

        if ((n = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &clientAddrLen)) < 0) {
            std::cout << "Error receiving data" << std::endl;
            return 1;
        }

        buffer[n] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    }
}