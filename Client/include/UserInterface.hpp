#ifndef USER_INTERFACE_HPP
#define USER_INTERFACE_HPP

#include <memory>

#include "Client.hpp"
#include "Socket.hpp"

class UserInterface
{
private:
    Client &client;

    static const std::vector<std::string> MAIN_MENU;
    static const std::vector<std::string> DAYS_OF_WEEK_MENU;
    static const std::vector<std::string> DAYS_OF_WEEK;

    void handleUserChoice(const int choice);
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
    void handleExit();

    static int promptChoice(const std::string prompt);
    static std::string promptFacilityName(const std::string prompt);
    static std::string promptDayOfWeek(const std::string prompt);
    static std::string promptDaysOfWeek(const std::string prompt);
    static std::string promptTime(const std::string prompt);
    static std::string promptBookingID(const std::string prompt);
    static int promptOffset(const std::string prompt);
    static bool promptConfirmation(const std::string prompt);
    static float promptRating(const std::string prompt);
    static int promptDuration(const std::string prompt);

    static std::string generateBox(const std::vector<std::string> &content);
    static bool isErrorResponse(const std::vector<std::string> &parsedResponse);

public:
    UserInterface(Client &client);

    void displayMenu();
    static void displayConnectionInfo(Socket &socket, struct sockaddr_in &serverAddr);

    static std::string promptServerIP(const std::string prompt);
    static int promptServerPort(const std::string prompt);
};

#endif // USER_INTERFACE_HPP
