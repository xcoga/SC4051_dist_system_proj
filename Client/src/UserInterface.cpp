#include "UserInterface.hpp"

#include <cstdio>
#include <limits>
#include <numeric>
#include <regex>

#include "ResponseParser.hpp"

/**
 * @brief Constructs a UserInterface object.
 * @param client Reference to the Client object for server communication.
 */
UserInterface::UserInterface(Client &client) : client(client) {}

const std::vector<std::string> UserInterface::MAIN_MENU = {
    "Facility Booking System",
    "1. Query Facility Names",
    "2. Query Facility Availability",
    "3. Book Facility",
    "4. Query Existing Booking",
    "5. Update Existing Booking",
    "6. Delete Existing Booking",
    "7. Monitor Facility Availability",
    "8. Rate Facility",
    "9. Query Facility Rating",
    "10. Echo Message",
    "11. Exit"
};

const std::vector<std::string> UserInterface::DAYS_OF_WEEK_MENU = {
    "Day of Week",
    "1. MONDAY",
    "2. TUESDAY",
    "3. WEDNESDAY",
    "4. THURSDAY",
    "5. FRIDAY",
    "6. SATURDAY",
    "7. SUNDAY"
};

const std::vector<std::string> UserInterface::DAYS_OF_WEEK = {
    "MONDAY",
    "TUESDAY",
    "WEDNESDAY",
    "THURSDAY",
    "FRIDAY",
    "SATURDAY",
    "SUNDAY"
};

/**
 * @brief Displays the main menu and handles user input.
 */
void UserInterface::displayMenu()
{
    int choice;

    while (true)
    {
        std::cout << std::endl;
        std::cout << generateBox(MAIN_MENU);

        choice = UserInterface::promptChoice("Enter choice (1-11): ");

        handleUserChoice(choice);
    }
}

/**
 * @brief Displays connection information for the client and server.
 * 
 * @param socket The socket object used for communication.
 * @param serverAddr The server address structure.
 */
void UserInterface::displayConnectionInfo(Socket &socket, struct sockaddr_in &serverAddr)
{
    struct sockaddr_in clientAddr;
    if (socket.getSocketName((struct sockaddr *)&clientAddr) < 0)
    {
        perror("Error getting socket name");
        exit(1);
    }

    char clientIp[INET_ADDRSTRLEN], serverIp[INET_ADDRSTRLEN];
    std::string clientPort, serverPort;

    inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIp, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(serverAddr.sin_addr), serverIp, INET_ADDRSTRLEN);
    clientPort = std::to_string(ntohs(clientAddr.sin_port));
    serverPort = std::to_string(ntohs(serverAddr.sin_port));

    std::vector<std::string> connectionInfo = {
        "Connection Information",
        "Client socket has been successfully bound.",
        "Client IP Address: " + std::string(clientIp),
        "Client Port: " + clientPort,
        "",
        "Client is connected to the server.",
        "Server IP Address: " + std::string(serverIp),
        "Server Port: " + serverPort
    };

    std::cout << std::endl;
    std::cout << generateBox(connectionInfo);
}

/**
 * @brief Prompts the user for the server IP address and port number.
 * 
 * @param prompt The prompt message to display.
 * @return The server IP address as a string.
 */
std::string UserInterface::promptServerIP(const std::string prompt)
{
    std::regex ipRegex(R"(^(\d{1,3}\.){3}\d{1,3}$|^localhost$)");
    std::string serverIp;

    while (true)
    {
        std::cout << prompt;
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

/**
 * @brief Prompts the user for the server port number.
 * 
 * @param prompt The prompt message to display.
 * @return The server port number as an integer.
 */
int UserInterface::promptServerPort(const std::string prompt)
{
    std::regex portRegex(R"(^\d{1,5}$)");
    std::string serverPortStr;
    int serverPort;

    while (true)
    {
        std::cout << prompt;
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

/**
 * @brief Handles the user's choice from the main menu.
 * 
 * @param choice The user's main menu choice.
 */
void UserInterface::handleUserChoice(const int choice)
{
    switch (choice)
    {
        case 1:
            handleQueryFacilityNames();
            break;
        case 2:
            handleQueryAvailability();
            break;
        case 3:
            handleBookFacility();
            break;
        case 4:
            handleQueryBooking();
            break;
        case 5:
            handleUpdateBooking();
            break;
        case 6:
            handleDeleteBooking();
            break;
        case 7:
            handleMonitorAvailability();
            break;
        case 8:
            handleRateFacility();
            break;
        case 9:
            handleQueryRating();
            break;
        case 10:
            handleEchoMessage();
            break;
        case 11:
            handleExit();
            break;
        default:
            std::cout << "Invalid choice. Please enter a number between 1 and 11." << std::endl;
    }
}

/**
 * @brief Handles the user's choice for querying facility names.
 */
void UserInterface::handleQueryFacilityNames()
{
    std::cout << std::endl;
    std::cout << "Query Facility Names selected." << std::endl;

    std::string response;
    std::vector<std::string> parsedResponse;

    response = client.queryFacilityNames();
    parsedResponse = ResponseParser::parseQueryFacilityNamesResponse(response);
    std::cout << generateBox(parsedResponse);
}

/**
 * @brief Handles the user's choice for querying facility availability.
 */
void UserInterface::handleQueryAvailability()
{
    std::cout << std::endl;
    std::cout << "Query Facility Availability selected." << std::endl;

    std::string facilityName, daysOfWeek, response;
    std::vector<std::string> parsedResponse;

    // Display list of facility names to choose from
    response = client.queryFacilityNames();
    parsedResponse = ResponseParser::parseQueryFacilityNamesResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }

    // Prompt user to enter facility name to check availability for
    facilityName = promptFacilityName("Enter facility name: ");
    daysOfWeek = promptDaysOfWeek("Enter choice (1-7, comma-separated): ");
    response = client.queryAvailability(facilityName, daysOfWeek);
    parsedResponse = ResponseParser::parseQueryAvailabilityResponse(response, daysOfWeek);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }
}

/**
 * @brief Handles the user's choice for booking a facility.
 */
void UserInterface::handleBookFacility()
{
    std::cout << std::endl;
    std::cout << "Book Facility selected." << std::endl;

    std::string facilityName, dayOfWeek, startTime, endTime;
    std::string response;
    std::vector<std::string> parsedResponse;

    // Display list of facility names to choose from
    response = client.queryFacilityNames();
    parsedResponse = ResponseParser::parseQueryFacilityNamesResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }

    facilityName = promptFacilityName("Enter facility name: ");
    dayOfWeek = promptDayOfWeek("Enter choice (1-7): ");
    startTime = promptTime("Enter start time (HHMM): ");
    endTime = promptTime("Enter end time (HHMM): ");

    response = client.bookFacility(facilityName, dayOfWeek, startTime, endTime);
    parsedResponse = ResponseParser::parseBookFacilityResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }
}

/**
 * @brief Handles the user's choice for querying an existing booking.
 */
void UserInterface::handleQueryBooking()
{
    std::cout << std::endl;
    std::cout << "Query Existing Booking selected." << std::endl;

    std::string bookingID, response;
    std::vector<std::string> parsedResponse;

    bookingID = promptBookingID("Enter booking ID: ");

    response = client.queryBooking(bookingID);
    parsedResponse = ResponseParser::parseQueryBookingResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }
}

/**
 * @brief Handles the user's choice for updating an existing booking.
 */
void UserInterface::handleUpdateBooking()
{
    std::cout << std::endl;
    std::cout << "Update Existing Booking selected." << std::endl;

    int offsetMinutes;
    std::string bookingID, oldBookingDetails, newStartTime, newEndTime, newBookingDetails;
    std::vector<std::string> parsedResponse;
    bool confirmation;

    bookingID = promptBookingID("Enter booking ID: ");

    // Display old booking details
    oldBookingDetails = client.queryBooking(bookingID);
    parsedResponse = ResponseParser::parseQueryBookingResponse(oldBookingDetails);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }

    confirmation = promptConfirmation("Update booking (yes/no)? ");
    if (!confirmation)
    {
        std::cout << "Booking not updated. Returning to main menu..." << std::endl;
        return;
    }

    offsetMinutes = promptOffset("Enter offset in minutes (positive for later, negative for earlier): ");

    newBookingDetails = client.updateBooking(bookingID, offsetMinutes, oldBookingDetails);
    parsedResponse = ResponseParser::parseUpdateBookingResponse(newBookingDetails);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }
}

/**
 * @brief Handles the user's choice for deleting an existing booking.
 */
void UserInterface::handleDeleteBooking()
{
    std::cout << std::endl;
    std::cout << "Delete Existing Booking selected." << std::endl;

    std::string bookingID, oldBookingDetails, response;
    std::vector<std::string> parsedResponse;
    bool confirmation;

    bookingID = promptBookingID("Enter booking ID: ");

    // Display booking details
    oldBookingDetails = client.queryBooking(bookingID);
    parsedResponse = ResponseParser::parseQueryBookingResponse(oldBookingDetails);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }

    confirmation = promptConfirmation("Delete booking (yes/no)? ");
    if (!confirmation)
    {
        std::cout << "Booking not deleted. Returning to main menu..." << std::endl;
        return;
    }

    response = client.deleteBooking(bookingID, oldBookingDetails);
    parsedResponse = ResponseParser::parseDeleteBookingResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }
}

/**
 * @brief Handles the user's choice for monitoring facility availability.
 */
void UserInterface::handleMonitorAvailability()
{
    std::cout << std::endl;
    std::cout << "Monitor Facility Availability selected." << std::endl;

    std::string facilityName, response;
    int durationSeconds;
    std::vector<std::string> parsedResponse;

    // Display list of facility names to choose from
    response = client.queryFacilityNames();
    parsedResponse = ResponseParser::parseQueryFacilityNamesResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }

    facilityName = promptFacilityName("Enter facility name: ");
    durationSeconds = promptDuration("Enter duration in seconds: ");

    client.monitorAvailability(facilityName, durationSeconds, [this](const std::string &response, const bool isRegistrationResponse) {
        std::vector<std::string> parsedResponse;
        if (isRegistrationResponse)
        {
            parsedResponse = ResponseParser::parseMonitorAvailabilityResponse(response);
            std::cout << generateBox(parsedResponse);
        }
        else
        {
            parsedResponse = ResponseParser::parseQueryAvailabilityResponse(response);
            std::cout << generateBox(parsedResponse);
        }
    });

    // Update client that monitoring period has ended
    parsedResponse = {
        "End of Monitoring",
        "Monitoring period has ended. No further updates will be received."
    };
    std::cout << generateBox(parsedResponse);
    std::cout << "Returning to main menu..." << std::endl;
}

/**
 * @brief Handles the user's choice for rating a facility.
 */
void UserInterface::handleRateFacility()
{
    std::cout << std::endl;
    std::cout << "Rate Facility selected." << std::endl;

    std::string facilityName, response;
    float rating;
    std::vector<std::string> parsedResponse;

    // Display list of facility names to choose from
    response = client.queryFacilityNames();
    parsedResponse = ResponseParser::parseQueryFacilityNamesResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }

    facilityName = promptFacilityName("Enter facility name: ");
    rating = promptRating("Enter rating (1-5): ");

    response = client.rateFacility(facilityName, rating);
    parsedResponse = ResponseParser::parseRateFacilityResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }
}

/**
 * @brief Handles the user's choice for querying facility rating.
 */
void UserInterface::handleQueryRating()
{
    std::cout << std::endl;
    std::cout << "Query Facility Rating selected." << std::endl;

    std::string facilityName, response;
    std::vector<std::string> parsedResponse;

    // Display list of facility names to choose from
    response = client.queryFacilityNames();
    parsedResponse = ResponseParser::parseQueryFacilityNamesResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }

    facilityName = promptFacilityName("Enter facility name: ");
    
    response = client.queryRating(facilityName);
    parsedResponse = ResponseParser::parseQueryRatingResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }
}

/**
 * @brief Handles the user's choice for echoing a message.
 */
void UserInterface::handleEchoMessage()
{
    std::cout << std::endl;
    std::cout << "Echo Message selected." << std::endl;

    std::string messageData;
    std::string response;
    std::vector<std::string> parsedResponse;
    bool confirmation;

    std::cout << "Enter message to send: ";
    std::cin.ignore();
    std::getline(std::cin, messageData);

    confirmation = promptConfirmation("Send message to server (yes/no)? ");
    if (!confirmation)
    {
        std::cout << "Message not sent. Returning to main menu..." << std::endl;
        return;
    }

    response = client.echoMessage(messageData);
    parsedResponse = ResponseParser::parseEchoMessageResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse))
    {
        return;
    }
}

/**
 * @brief Handles the user's choice for exiting the application.
 */
void UserInterface::handleExit()
{
    bool confirmation = promptConfirmation("Exit Facility Booking System (yes/no)? ");
    if (confirmation)
    {
        std::cout << "Exiting..." << std::endl;
        exit(0);
    }
    else
    {
        std::cout << "Returning to main menu..." << std::endl;
    }
}

/**
 * @brief Prompts the user for choice input and validates it.
 * 
 * This function displays a prompt message to the user and waits for input.
 * It validates the input to ensure it is a number between 1 and 11.
 * If the input is invalid, it clears the error state and prompts again.
 * 
 * @param prompt The prompt message to display.
 * 
 * @return The user's validated main menu choice.
 */
int UserInterface::promptChoice(const std::string prompt)
{
    int choice;

    while (true)
    {
        std::cout << prompt;
        std::cin >> choice;

        if (std::cin.fail())
        {
            std::cin.clear(); // Clear the error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cout << "Invalid input. Please enter a number." << std::endl;
        }
        else if (choice < 1 || choice > 11)
        {
            std::cout << "Invalid choice. Please enter a number between 1 and 11." << std::endl;
        }
        else
        {
            return choice;
        }
    }
}

/**
 * @brief Prompts the user for a facility name.
 * 
 * @param prompt The prompt message to display.
 * 
 * @return The facility name entered by the user.
 */
std::string UserInterface::promptFacilityName(const std::string prompt)
{
    std::string facilityName;

    std::cout << prompt;
    std::cin.ignore();
    std::getline(std::cin, facilityName);

    return facilityName;
}

/**
 * @brief Prompts the user for a day of the week.
 * 
 * This function displays a menu of days of the week and prompts the user to select one.
 * It validates the input to ensure it is a number between 1 and 7.
 * If the input is invalid, it clears the error state and prompts again.
 * If the input is valid, it returns the corresponding day name.
 * 
 * @param prompt The prompt message to display.
 * 
 * @return The day of the week entered by the user.
 */
std::string UserInterface::promptDayOfWeek(const std::string prompt)
{
    int choice;

    while (true)
    {
        std::cout << generateBox(DAYS_OF_WEEK_MENU);
        std::cout << prompt;
        std::cin >> choice;

        if (std::cin.fail() || choice < 1 || choice > 7)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cout << "Invalid input. Please enter a number between 1 and 7." << std::endl;
        }
        else
        {
            return DAYS_OF_WEEK[choice - 1];
        }
    }
}

/**
 * @brief Prompts the user for multiple days of the week.
 * 
 * This function displays a menu of days of the week and prompts the user to select multiple days.
 * It validates the input to ensure each selected day is a number between 1 and 7.
 * If the input is invalid, it clears the error state and prompts again.
 * If the input is valid, it returns a comma-separated string of the corresponding day names for the user's input.
 * 
 * @param prompt The prompt message to display.
 * 
 * @return The days of the week entered by the user.
 */
std::string UserInterface::promptDaysOfWeek(const std::string prompt)
{
    while (true)
    {
        std::cout << generateBox(DAYS_OF_WEEK_MENU);
        std::cout << prompt;

        std::string input;
        std::cin >> input;

        // Split the input by commas
        std::vector<std::string> tokens;
        std::stringstream ss(input);
        std::string token;
        while (std::getline(ss, token, ','))
        {
            tokens.push_back(token);
        }

        // Validate each token
        std::vector<std::string> selectedDays;
        bool isValid = true;
        for (const auto &choiceStr : tokens)
        {
            try
            {
                int choice = std::stoi(choiceStr);
                if (choice < 1 || choice > 7)
                {
                    std::cout << "Invalid input: " << choiceStr << ". Please enter numbers between 1 and 7." << std::endl;
                    isValid = false;
                    break;
                }
                selectedDays.push_back(DAYS_OF_WEEK[choice - 1]);
            }
            catch (const std::invalid_argument &)
            {
                std::cout << "Invalid input: " << choiceStr << ". Please enter valid numbers separated by commas." << std::endl;
                isValid = false;
                break;
            }
        }

        if (isValid)
        {
            // Join the selected days into a comma-separated string
            std::string result = std::accumulate(
                std::next(selectedDays.begin()), selectedDays.end(), selectedDays[0],
                [](std::string a, const std::string &b) { return a + "," + b; });

            return result;
        }
    }
}

/**
 * @brief Prompts the user for a time in HHMM format.
 * 
 * This function displays a prompt message to the user and waits for input.
 * It validates the input to ensure it is in the correct HHMM format (24-hour clock).
 * If the input is invalid, it clears the error state and prompts again.
 * 
 * @param prompt The prompt message to display.
 * 
 * @return The time entered by the user in HHMM format.
 */
std::string UserInterface::promptTime(const std::string prompt)
{
    std::regex timeRegex(R"(^([01]\d|2[0-3])[0-5]\d$)");
    std::string timeStr;

    while (true)
    {
        std::cout << prompt;
        std::cin >> timeStr;

        if (!std::regex_match(timeStr, timeRegex))
        {
            std::cout << "Invalid time format. Please enter a valid time between 0000 and 2359." << std::endl;
        }
        else
        {
            return timeStr;
        }
    }
}

/**
 * @brief Prompts the user for a booking ID.
 * 
 * This function displays a prompt message to the user and waits for input.
 * It validates the input to ensure it is in the correct UUID v4 format.
 * If the input is invalid, it clears the error state and prompts again.
 * 
 * @param prompt The prompt message to display.
 * 
 * @return The booking ID entered by the user.
 */
std::string UserInterface::promptBookingID(const std::string prompt)
{
    // Booking ID is in UUID v4 format
    std::regex bookingIDRegex(R"(^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$)", std::regex::icase);
    std::string bookingID;

    while (true)
    {
        std::cout << prompt;
        std::cin >> bookingID;

        if (!std::regex_match(bookingID, bookingIDRegex))
        {
            std::cout << "Invalid booking ID. Please try again." << std::endl;
        }
        else
        {
            return bookingID;
        }
    }
}

/**
 * @brief Prompts the user for an offset in minutes.
 * 
 * This function displays a prompt message to the user and waits for input.
 * It validates the input to ensure it is a non-zero integer.
 * If the input is invalid, it clears the error state and prompts again.
 * 
 * @param prompt The prompt message to display.
 * 
 * @return The offset in minutes entered by the user.
 */
int UserInterface::promptOffset(const std::string prompt)
{
    int offset;

    while (true)
    {
        std::cout << prompt;
        std::cin >> offset;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cout << "Invalid input. Please enter an integer value for the offset in minutes." << std::endl;
        }
        else if (offset == 0)
        {
            std::cout << "Offset cannot be zero. Please enter a non-zero integer value." << std::endl;
        }
        else
        {
            return offset;
        }
    }
}

/**
 * @brief Prompts the user for confirmation.
 * 
 * This function displays a prompt message to the user and waits for input.
 * It validates the input to ensure it is either 'yes' or 'no' or 'y' or 'n'.
 * If the input is invalid, it clears the error state and prompts again.
 * 
 * @param prompt The prompt message to display.
 * 
 * @return True if the user confirms, false otherwise.
 */
bool UserInterface::promptConfirmation(const std::string prompt)
{
    std::regex yesNoRegex(R"(^\s*(yes|y|no|n)\s*$)", std::regex::icase);
    std::string response;

    while (true)
    {
        std::cout << prompt;
        std::cin >> response;

        if (!std::regex_match(response, yesNoRegex))
        {
            std::cout << "Invalid input. Please enter 'yes' or 'no'." << std::endl;
        }
        else
        {
            // Returns true for 'yes'/'y', false for 'no'/'n'
            return (response[0] == 'y' || response[0] == 'Y');
        }
    }
}

/**
 * @brief Prompts the user for a rating between 1 and 5.
 * 
 * This function displays a prompt message to the user and waits for input.
 * It validates the input to ensure it is a float between 1 and 5.
 * If the input is invalid, it clears the error state and prompts again.
 * 
 * @param prompt The prompt message to display.
 * 
 * @return The rating entered by the user.
 */
float UserInterface::promptRating(const std::string prompt)
{
    float rating;

    while (true)
    {
        std::cout << prompt;
        std::cin >> rating;

        if (std::cin.fail() || rating < 1 || rating > 5)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cout << "Invalid input. Please enter a number between 1 and 5." << std::endl;
        }
        else
        {
            return rating;
        }
    }
}

/**
 * @brief Prompts the user for a duration in seconds.
 * 
 * This function displays a prompt message to the user and waits for input.
 * It validates the input to ensure it is a positive integer.
 * If the input is invalid, it clears the error state and prompts again.
 * 
 * @param prompt The prompt message to display.
 * 
 * @return The duration in seconds entered by the user.
 */
int UserInterface::promptDuration(const std::string prompt)
{
    std::string input;
    int duration;

    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, input);

        // Check if input is all digits
        if (!input.empty() && std::all_of(input.begin(), input.end(), ::isdigit))
        {
            try
            {
                duration = std::stoi(input);
                if (duration > 0)
                {
                    return duration;
                }
                else
                {
                    std::cout << "Please enter a positive number." << std::endl;
                }
            }
            catch (const std::out_of_range &e)
            {
                std::cout << "Number too large. Please enter a smaller positive number." << std::endl;
            }
        }

        std::cout << "Invalid input. Please enter a positive number." << std::endl;
    }
}

/**
 * @brief Generates a box with centered text for the user interface.
 * 
 * This function takes a vector of strings as input and generates a box with the header centered and the content left-aligned.
 * The first line is treated as the header, and the rest are treated as content lines.
 * 
 * @param content The vector of strings to be displayed in the box.
 * 
 * @return A string representing the generated box with centered text.
 */
std::string UserInterface::generateBox(const std::vector<std::string> &content)
{
    if (content.empty())
        return "";

    std::string box;
    size_t maxLength = 0;

    // Find the maximum length of the content lines
    for (const auto &line : content)
    {
        maxLength = std::max(maxLength, line.length());
    }

    // Create the top border
    box += "+-" + std::string(maxLength, '-') + "-+\n";

    // Center-aligned header
    size_t leftPadding = (maxLength - content[0].length()) / 2;
    size_t rightPadding = maxLength - content[0].length() - leftPadding;
    box += "| " + std::string(leftPadding, ' ') + content[0] + std::string(rightPadding, ' ') + " |\n";

    // Add a separator line under the header
    box += "+-" + std::string(maxLength, '-') + "-+\n";

    // Add the rest of the content
    for (size_t i = 1; i < content.size(); i++)
    {
        box += "| " + content[i] + std::string(maxLength - content[i].length(), ' ') + " |\n";
    }

    // Create the bottom border
    box += "+-" + std::string(maxLength, '-') + "-+\n";

    return box;
}

/**
 * @brief Checks if the response indicates an error.
 * 
 * Every parsed response vector that corresponds to an error message will have "Error" as its first element.
 * Hence, this function checks if the first element of the parsed response vector is "Error" to determine whether the response is an error.
 * 
 * @param parsedResponse The parsed response vector to check.
 * 
 * @return True if the response indicates an error, false otherwise.
 */
bool UserInterface::isErrorResponse(const std::vector<std::string> &parsedResponse)
{
    if (!parsedResponse.empty() && parsedResponse[0] == "Error")
    {
        std::cout << "Returning to main menu..." << std::endl;
        return true;
    }
    return false;
}
