#include <iostream>

#include "Client.hpp"
#include "UserInterface.hpp"
#include "Serializer.hpp"

void registerClasses()
{
    ObjectFactory::creators["Server.RequestMessage"] = []()
    {
        return std::make_shared<RequestMessage>();
    };
}

int main()
{
    std::string serverIP = UserInterface::promptServerIP();
    int serverPort = UserInterface::promptServerPort();

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