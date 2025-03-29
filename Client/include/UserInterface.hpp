#ifndef USER_INTERFACE_HPP
#define USER_INTERFACE_HPP

#include "Client.hpp"
#include "Socket.hpp"

#include <memory>

class UserInterface
{
private:
    Client &client;

    void handleUserChoice(int choice);
    void handleQueryFacilityNames();
    void handleQueryAvailability();
    void handleBookFacility();
    void handleQueryBooking();
    void handleUpdateBooking();
    void handleDeleteBooking();
    void handleMonitorAvailability();
    void handleRateFacility();
    void handleQueryRating();
    void handleEchoMessage();

    static int promptChoice(std::string prompt);
    static std::string promptFacilityName(std::string prompt);
    static std::string promptDayOfWeek(std::string prompt);
    static std::string promptTime(std::string prompt);
    static std::string promptBookingID(std::string prompt);
    static bool promptConfirmation(std::string prompt);
    static float promptRating(std::string prompt);

    static std::string generateBox(const std::vector<std::string> &content);
    static bool isErrorResponse(const std::vector<std::string> &parsedResponse);

public:
    UserInterface(Client &client);
    void displayMenu();
    static void displayConnectionInfo(Socket &socket, struct sockaddr_in &serverAddr);
    static std::string promptServerIP(std::string prompt);
    static int promptServerPort(std::string prompt);
};

#endif // USER_INTERFACE_HPP