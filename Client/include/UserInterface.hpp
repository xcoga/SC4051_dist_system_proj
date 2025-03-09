#ifndef USER_INTERFACE_HPP
#define USER_INTERFACE_HPP

#include "Client.hpp"

class UserInterface
{
private:
    Client &client;
    void handleUserChoice(int choice);

public:
    UserInterface(Client &client);

    void displayMenu();
};

#endif // USER_INTERFACE_HPP