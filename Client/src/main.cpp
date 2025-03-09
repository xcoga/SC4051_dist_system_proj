#include <iostream>

#include "Client.hpp"
#include "UserInterface.hpp"
#include "Serializer.hpp"

void registerClasses()
{
    ObjectFactory::registerClass<RequestMessage>("Server.RequestMessage");
}

int main()
{
    std::string serverIp;
    int serverPort;

    std::cout << "Enter server IP: ";
    std::cin >> serverIp;

    std::cout << "Enter server port: ";
    std::cin >> serverPort;

    try
    {
        Client client(serverIp, serverPort);
        UserInterface ui(client);
        ui.displayMenu();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}