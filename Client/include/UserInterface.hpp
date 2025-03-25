#ifndef USER_INTERFACE_HPP
#define USER_INTERFACE_HPP

#include "Client.hpp"

#include <memory>

class UserInterface
{
private:
    Client &client;
    static const std::string dayOfWeekMenu;
    static const std::string daysOfWeek[];
    static const std::string mainMenu;

    void handleUserChoice(int choice);
    static int promptChoice(std::string prompt);
    static std::string promptFacilityName(std::string prompt);
    static std::string promptDayOfWeek(std::string prompt);
    static std::string promptTime(std::string prompt);
    static std::string promptBookingID(std::string prompt);

public:
    UserInterface(Client &client);
    void displayMenu();
    static std::string promptServerIP(std::string prompt);
    static int promptServerPort(std::string prompt);
};

#endif // USER_INTERFACE_HPP