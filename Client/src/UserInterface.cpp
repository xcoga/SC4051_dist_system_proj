#include "UserInterface.hpp"

UserInterface::UserInterface(Client &client) : client(client) {}

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
        std::cout << "| 7. Exit                           |" << std::endl;
        std::cout << "+-----------------------------------+" << std::endl;

        std::cout << std::endl;
        std::cout << "Enter choice: ";
        std::cin >> choice;

        if (choice == 7)
        {
            std::cout << "Exiting...\n";
            break;
        }

        handleUserChoice(choice);
    }
}

void UserInterface::handleUserChoice(int choice)
{
    switch (choice)
    {
    case 1:
        // std::cout << "choice 1 selected" << std::endl;
        client.sendMessage();
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
    default:
        std::cout << "Invalid choice" << std::endl;
    }
}