#include <iostream>

#include "Client.hpp"
#include "Serializer.hpp"
#include "UserInterface.hpp"

void registerClasses()
{
    ObjectFactory::creators["Server.RequestMessage"] = []()
    {
        return std::make_shared<RequestMessage>();
    };
}

int main()
{
    std::string serverIP;
    int serverPort;

    serverIP = UserInterface::promptServerIP("Enter server IP (IPv4 format or 'localhost'): ");
    serverPort = UserInterface::promptServerPort("Enter server port: ");

    registerClasses();

    try
    {
        Client client(serverIP, serverPort);
        UserInterface ui(client);
        ui.displayMenu();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
