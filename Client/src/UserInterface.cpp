#include "UserInterface.hpp"

#include <regex>

UserInterface::UserInterface(Client &client) : client(client) {}

/* Public methods */
std::string UserInterface::promptServerIP()
{
    std::regex ipRegex(R"(^(\d{1,3}\.){3}\d{1,3}$|^localhost$)");
    std::string serverIp;

    while (true)
    {
        std::cout << "Enter server IP (IPv4 format or 'localhost'): ";
        std::cin >> serverIp;

        if (std::regex_match(serverIp, ipRegex))
        {
            return serverIp;
        }
        else
        {
            std::cout << "Invalid IPv4 address. Please try again." << std::endl;
        }
    }
}

int UserInterface::promptServerPort()
{
    std::regex portRegex(R"(^\d{1,5}$)");
    std::string serverPortStr;
    int serverPort;

    while (true)
    {
        std::cout << "Enter server port: ";
        std::cin >> serverPortStr;

        try
        {
            serverPort = std::stoi(serverPortStr);
            if (serverPort > 0 && serverPort <= 65535)
            {
                break;
            }
            else
            {
                std::cout << "Invalid port number. Please enter a value between 1 and 65535." << std::endl;
            }
        }
        catch (const std::invalid_argument &)
        {
            std::cout << "Invalid port number. Please try again." << std::endl;
        }
        catch (const std::out_of_range &)
        {
            std::cout << "Port number out of range. Please try again." << std::endl;
        }
    }

    return serverPort;
}

void UserInterface::displayMenu()
{
    int choice;
    while (true)
    {
        std::cout << std::endl;
        std::cout << "+----- Facility Booking System -----+" << std::endl;
        std::cout << "| 1. Query Facility Availability    |" << std::endl;
        std::cout << "| 2. Book Facility                  |" << std::endl;
        std::cout << "| 3. Change Existing Booking        |" << std::endl;
        std::cout << "| 4. Monitor Facility Availability  |" << std::endl;
        std::cout << "| 5. TODO: Idempotent operation     |" << std::endl;
        std::cout << "| 6. TODO: Non-idempotent operation |" << std::endl;
        std::cout << "| 7. [DEBUG] Echo Message           |" << std::endl;
        std::cout << "| 8. Exit                           |" << std::endl;
        std::cout << "+-----------------------------------+" << std::endl;

        std::cout << std::endl;
        std::cout << "Enter choice: ";
        std::cin >> choice;

        if (choice == 8)
        {
            std::cout << "Exiting...\n";
            return;
        }

        handleUserChoice(choice);
    }
}

/* Private methods */
void UserInterface::handleUserChoice(int choice)
{
    switch (choice)
    {
    case 1:
        std::cout << "choice 1 selected" << std::endl;
        break;
    case 2:
        std::cout << "choice 2 selected" << std::endl;
        break;
    case 3:
        std::cout << "choice 3 selected" << std::endl;
        break;
    case 4:
        std::cout << "choice 4 selected" << std::endl;
        break;
    case 5:
        std::cout << "choice 5 selected" << std::endl;
        break;
    case 6:
        std::cout << "choice 6 selected" << std::endl;
        break;
    case 7:
        client.sendCustomMessage();
        break;
    default:
        std::cout << "Invalid choice" << std::endl;
    }
}
