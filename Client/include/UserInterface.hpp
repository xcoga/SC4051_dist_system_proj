#ifndef USER_INTERFACE_HPP
#define USER_INTERFACE_HPP

#include "Client.hpp"

#include <memory>

class UserInterface
{
private:
    Client &client;
    void handleUserChoice(int choice);

public:
    UserInterface(Client &client);
    void displayMenu();
    static std::string promptServerIP();
    static int promptServerPort();
};

#endif // USER_INTERFACE_HPP