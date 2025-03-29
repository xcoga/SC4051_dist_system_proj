#include "UserInterface.hpp"

#include "ResponseParser.hpp"

#include <regex>

/* Constructors */
UserInterface::UserInterface(Client &client) : client(client) {}

/* Public methods */
void UserInterface::displayMenu()
{
    int choice;
    std::vector<std::string> menuContent = {
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
        "10. [DEBUG] Echo Message",
        "11. Exit"
    };

    while (true)
    {
        std::cout << std::endl;
        std::cout << generateBox(menuContent);

        choice = UserInterface::promptChoice("Enter choice (1-11): ");

        if (choice == 11)
        {
            std::cout << "Exiting...\n";
            return;
        }

        handleUserChoice(choice);
    }
}

void UserInterface::displayConnectionInfo(Socket &socket, struct sockaddr_in &serverAddr)
{
    struct sockaddr_in clientAddr;
    if (socket.getSocketName((struct sockaddr *)&clientAddr) < 0)
    {
        perror("Error getting socket name");
        exit(1);
    }

    // Print the bound IP address and port
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
    std::string ipAddress = "Client IP Address: " + std::string(ipStr);
    std::string port = "Client Port: " + std::to_string(ntohs(clientAddr.sin_port));

    char serverIpStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serverAddr.sin_addr), serverIpStr, INET_ADDRSTRLEN);
    std::string serverIpAddress = "Server IP Address: " + std::string(serverIpStr);
    std::string serverPort = "Server Port: " + std::to_string(ntohs(serverAddr.sin_port));

    std::vector<std::string> connectionInfo = {
        "Connection Information",
        "Client socket has been successfully bound.",
        ipAddress,
        port,
        "",
        "Client is connected to the server.",
        serverIpAddress,
        serverPort
    };

    std::cout << std::endl;
    std::cout << generateBox(connectionInfo);
}

std::string UserInterface::promptServerIP(std::string prompt)
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

int UserInterface::promptServerPort(std::string prompt)
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

/* Private methods */
void UserInterface::handleUserChoice(int choice)
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
        std::cout << "Exiting..." << std::endl;
        break;
    default:
        std::cout << "Invalid choice. Please enter a number between 1 and 10." << std::endl;
    }
}

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

void UserInterface::handleQueryAvailability()
{
    std::cout << std::endl;
    std::cout << "Query Facility Availability selected." << std::endl;

    std::string facilityName, response;
    std::vector<std::string> parsedResponse;

    // Display list of facility names to choose from
    response = client.queryFacilityNames();
    parsedResponse = ResponseParser::parseQueryFacilityNamesResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse)) return;

    // Prompt user to enter facility name to check availability for
    facilityName = promptFacilityName("Enter facility name: ");
    response = client.queryAvailability(facilityName);
    parsedResponse = ResponseParser::parseQueryAvailabilityResponse(response, facilityName);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse)) return;
}

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
    if (isErrorResponse(parsedResponse)) return;

    facilityName = promptFacilityName("Enter facility name: ");
    dayOfWeek = promptDayOfWeek("Enter choice (1-7): ");
    startTime = promptTime("Enter start time (HHMM): ");
    endTime = promptTime("Enter end time (HHMM): ");

    response = client.bookFacility(facilityName, dayOfWeek, startTime, endTime);
    parsedResponse = ResponseParser::parseBookFacilityResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse)) return;
}

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
    if (isErrorResponse(parsedResponse)) return;
}

void UserInterface::handleUpdateBooking()
{
    std::cout << std::endl;
    std::cout << "Change Existing Booking selected." << std::endl;

    std::string bookingID, dayOfWeek, newStartTime, newEndTime, response;
    std::vector<std::string> parsedResponse;

    bookingID = promptBookingID("Enter booking ID: ");

    // TODO: Query existing booking details and display them to the user

    dayOfWeek = promptDayOfWeek("Enter new day of week (1-7): ");
    newStartTime = promptTime("Enter new start time (HHMM): ");
    newEndTime = promptTime("Enter new end time (HHMM): ");

    response = client.updateBooking(bookingID, dayOfWeek, newStartTime, newEndTime);
    parsedResponse = ResponseParser::parseUpdateBookingResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse)) return;
}

void UserInterface::handleDeleteBooking()
{
    std::cout << std::endl;
    std::cout << "Delete Existing Booking selected." << std::endl;

    std::string bookingID, response;
    std::vector<std::string> parsedResponse;

    bookingID = promptBookingID("Enter booking ID: ");

    // TODO: Query existing booking details and display them to the user

    // Prompt user for confirmation before deleting booking
    bool confirmation = promptConfirmation("Delete booking (yes/no)? ");
    if (!confirmation)
    {
        std::cout << "Booking not deleted. Returning to main menu..." << std::endl;
        return;
    }

    response = client.deleteBooking(bookingID);
    parsedResponse = ResponseParser::parseDeleteBookingResponse(response);

    std::cout << "Received response from server: " << response << std::endl;
}

void UserInterface::handleMonitorAvailability()
{
    std::cout << std::endl;
    std::cout << "Monitor Facility Availability selected." << std::endl;

    std::string facilityName;
    std::string response;

    facilityName = promptFacilityName("Enter facility name: ");
    response = client.monitorAvailability(facilityName);

    std::cout << "Received response from server: " << response << std::endl;
}

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
    if (isErrorResponse(parsedResponse)) return;

    facilityName = promptFacilityName("Enter facility name: ");
    rating = promptRating("Enter rating (1-5): ");

    response = client.rateFacility(facilityName, rating);
    parsedResponse = ResponseParser::parseRateFacilityResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse)) return;
}

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
    if (isErrorResponse(parsedResponse)) return;

    facilityName = promptFacilityName("Enter facility name: ");
    
    response = client.queryRating(facilityName);
    parsedResponse = ResponseParser::parseQueryRatingResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse)) return;
}

void UserInterface::handleEchoMessage()
{
    std::cout << std::endl;
    std::cout << "Echo Message selected." << std::endl;

    std::string messageData;
    std::string response;
    std::vector<std::string> parsedResponse;

    std::cout << "Enter message to send: ";
    std::cin.ignore();
    std::getline(std::cin, messageData);

    bool confirmation = promptConfirmation("Send message to server (yes/no)? ");
    if (!confirmation)
    {
        std::cout << "Message not sent. Returning to main menu..." << std::endl;
        return;
    }

    response = client.echoMessage(messageData);
    parsedResponse = ResponseParser::parseEchoMessageResponse(response);
    std::cout << generateBox(parsedResponse);
    if (isErrorResponse(parsedResponse)) return;
}

int UserInterface::promptChoice(std::string prompt)
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

std::string UserInterface::promptFacilityName(std::string prompt)
{
    std::string facilityName;

    std::cout << prompt;
    std::cin.ignore();
    std::getline(std::cin, facilityName);

    return facilityName;
}

std::string UserInterface::promptDayOfWeek(std::string prompt)
{
    int choice;
    std::vector<std::string> daysOfWeekMenu = {
        "Day of Week",
        "1. MONDAY",
        "2. TUESDAY",
        "3. WEDNESDAY",
        "4. THURSDAY",
        "5. FRIDAY",
        "6. SATURDAY",
        "7. SUNDAY"
    };
    std::vector<std::string> daysOfWeek = {"MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY"};

    while (true)
    {
        std::cout << generateBox(daysOfWeekMenu);
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
            return daysOfWeek[choice - 1];
        }
    }
}

std::string UserInterface::promptTime(std::string prompt)
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

std::string UserInterface::promptBookingID(std::string prompt)
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

bool UserInterface::promptConfirmation(std::string prompt)
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

float UserInterface::promptRating(std::string prompt)
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

bool UserInterface::isErrorResponse(const std::vector<std::string> &parsedResponse)
{
    if (!parsedResponse.empty() && parsedResponse[0] == "Error")
    {
        std::cout << "Returning to main menu..." << std::endl;
        return true;
    }
    return false;
}
