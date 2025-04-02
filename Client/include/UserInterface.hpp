#ifndef USER_INTERFACE_HPP
#define USER_INTERFACE_HPP

#include <memory>

#include "Client.hpp"
#include "Socket.hpp"

/**
 * @class UserInterface
 * @brief Handles the user interaction for the Facility Booking System.
 * 
 * The UserInterface class provides methods to display menus, prompt user input,
 * and handle various client operations such as querying availability of facilities,
 * booking facilities, updating and deleting bookings, monitoring facility availability,
 * and rating facilities.
 */
class UserInterface
{
private:
    Client &client; ///< Reference to the Client object for server communication.

    /**
     * @brief Handles the user's choice from the main menu.
     * 
     * @param choice The user's main menu choice.
     */
    void handleUserChoice(const int choice);

    /**
     * @brief Handles the user's choice for querying facility names.
     */
    void handleQueryFacilityNames();

    /**
     * @brief Handles the user's choice for querying facility availability.
     */
    void handleQueryAvailability();

    /**
     * @brief Handles the user's choice for booking a facility.
     */
    void handleBookFacility();

    /**
     * @brief Handles the user's choice for querying an existing booking.
     */
    void handleQueryBooking();

    /**
     * @brief Handles the user's choice for updating an existing booking.
     */
    void handleUpdateBooking();

    /**
     * @brief Handles the user's choice for deleting an existing booking.
     */
    void handleDeleteBooking();

    /**
     * @brief Handles the user's choice for monitoring facility availability.
     */
    void handleMonitorAvailability();

    /**
     * @brief Handles the user's choice for rating a facility.
     */
    void handleRateFacility();

    /**
     * @brief Handles the user's choice for querying facility rating.
     */
    void handleQueryRating();

    /**
     * @brief Handles the user's choice for echoing a message.
     */
    void handleEchoMessage();

    /**
     * @brief Handles the user's choice for exiting the application.
     */
    void handleExit();

    /**
     * @brief Prompts the user for choice input and validates it.
     * @param prompt The prompt message to display.
     * @return The user's validated main menu choice.
     */
    static int promptChoice(const std::string prompt);

    /**
     * @brief Prompts the user for a facility name.
     * @param prompt The prompt message to display.
     * @return The facility name entered by the user.
     */
    static std::string promptFacilityName(const std::string prompt);

    /**
     * @brief Prompts the user for a day of the week.
     * @param prompt The prompt message to display.
     * @return The day of the week entered by the user.
     */
    static std::string promptDayOfWeek(const std::string prompt);

    /**
     * @brief Prompts the user for multiple days of the week.
     * @param prompt The prompt message to display.
     * @return The days of the week entered by the user.
     */
    static std::string promptDaysOfWeek(const std::string prompt);

    /**
     * @brief Prompts the user for a time in HHMM format.
     * @param prompt The prompt message to display.
     * @return The time entered by the user in HHMM format.
     */
    static std::string promptTime(const std::string prompt);

    /**
     * @brief Prompts the user for a booking ID.
     * @param prompt The prompt message to display.
     * @return The booking ID entered by the user.
     */
    static std::string promptBookingID(const std::string prompt);

    /**
     * @brief Prompts the user for a time offset in minutes.
     * @param prompt The prompt message to display.
     * @return The time offset entered by the user in minutes.
     */
    static int promptOffset(const std::string prompt);

    /**
     * @brief Prompts the user for a confirmation (yes/no).
     * @param prompt The prompt message to display.
     * @return True if the user confirms, false otherwise.
     */
    static bool promptConfirmation(const std::string prompt);

    /**
     * @brief Prompts the user for a rating value.
     * @param prompt The prompt message to display.
     * @return The rating value entered by the user.
     */
    static float promptRating(const std::string prompt);

    /**
     * @brief Prompts the user for a duration in seconds.
     * @param prompt The prompt message to display.
     * @return The duration entered by the user in seconds.
     */
    static int promptDuration(const std::string prompt);

    /**
     * @brief Generates a box with the specified content.
     * @param content The content to display inside the box.
     * @return The formatted box string.
     */
    static std::string generateBox(const std::vector<std::string> &content);

    /**
     * @brief Checks if the response from the server indicates an error.
     * @param parsedResponse The parsed response from the server.
     * @return True if the response indicates an error, false otherwise.
     */
    static bool isErrorResponse(const std::vector<std::string> &parsedResponse);

public:
    /**
     * @brief Constructs a UserInterface object.
     * @param client Reference to the Client object for server communication.
     */
    UserInterface(Client &client);

    /**
     * @brief Displays the main menu and handles user input.
     */
    void displayMenu();

    /**
     * @brief Displays connection information for the client and server.
     * @param socket The socket object used for communication.
     * @param serverAddr The server address structure.
     */
    static void displayConnectionInfo(Socket &socket, struct sockaddr_in &serverAddr);

    /**
     * @brief Prompts the user for the server IP address and port number.
     * @param prompt The prompt message to display.
     * @return The server IP address as a string.
     */
    static std::string promptServerIP(const std::string prompt);

    /**
     * @brief Prompts the user for the server port number.
     * @param prompt The prompt message to display.
     * @return The server port number as an integer.
     */
    static int promptServerPort(const std::string prompt);
};

#endif // USER_INTERFACE_HPP
