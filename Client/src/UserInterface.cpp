#include "UserInterface.hpp"

#include <regex>

/* Constants */
const std::string UserInterface::dayOfWeekMenu = R"(+-----------------------------------+
|            Day of Week            |
+-----------------------------------+
| 1. MONDAY                         |
| 2. TUESDAY                        |
| 3. WEDNESDAY                      |
| 4. THURSDAY                       |
| 5. FRIDAY                         |
| 6. SATURDAY                       |
| 7. SUNDAY                         |
+-----------------------------------+
)";

const std::string UserInterface::daysOfWeek[] = {"MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY"};

const std::string UserInterface::mainMenu = R"(
+-----------------------------------+
|      Facility Booking System      |
+-----------------------------------+
| 1. Query Facility Availability    |
| 2. Book Facility                  |
| 3. Query Existing Booking         |
| 4. Change Existing Booking        |
| 5. Monitor Facility Availability  |
| 6. TODO: Idempotent operation     |
| 7. TODO: Non-idempotent operation |
| 8. [DEBUG] Echo Message           |
| 9. Exit                           |
+-----------------------------------+
)";

/* Constructors */
UserInterface::UserInterface(Client &client) : client(client) {}

/* Public methods */
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

void UserInterface::displayMenu()
{
    int choice;
    while (true)
    {
        std::cout << mainMenu;

        choice = UserInterface::promptChoice("Enter choice (1-9): ");

        if (choice == 9)
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
    // Initialize all possible variables that may be used in the switch statement
    std::string facilityName, dayOfWeek, startTime, newStartTime, endTime, newEndTime, bookingID, messageData, response;
    std::cout << std::endl;

    switch (choice)
    {
    case 1:
        std::cout << "Query Facility Availability selected." << std::endl;

        facilityName = promptFacilityName("Enter facility name: ");;
        response = client.queryAvailability(facilityName);

        std::cout << "Received response from server: " << response << std::endl;

        break;
    case 2:
        std::cout << "Book Facility selected." << std::endl;

        facilityName = promptFacilityName("Enter facility name: ");
        dayOfWeek = promptDayOfWeek("Enter choice (1-7): ");
        startTime = promptTime("Enter start time (HHMM): ");
        endTime = promptTime("Enter end time (HHMM): ");

        response = client.bookFacility(facilityName, dayOfWeek, startTime, endTime);
        std::cout << "Received response from server: " << response << std::endl;

        break;
    case 3:
        std::cout << "Query Existing Booking selected." << std::endl;

        bookingID = promptBookingID("Enter booking ID: ");

        std::cout << "Booking ID: " << bookingID << std::endl;

        break;
    case 4:
        std::cout << "Change Existing Booking selected." << std::endl;

        bookingID = promptBookingID("Enter booking ID: ");
        dayOfWeek = promptDayOfWeek("Enter new day of week (1-7): ");
        newStartTime = promptTime("Enter new start time (HHMM): ");
        newEndTime = promptTime("Enter new end time (HHMM): ");

        std::cout << "Booking ID: " << bookingID << std::endl;
        std::cout << "New Day of Week: " << dayOfWeek << std::endl;
        std::cout << "New Start Time: " << newStartTime << std::endl;
        std::cout << "New End Time: " << newEndTime << std::endl;

        break;
    case 5:
        std::cout << "Monitor Facility Availability selected." << std::endl;

        facilityName = promptFacilityName("Enter facility name: ");

        std::cout << "Facility Name: " << facilityName << std::endl;

        break;
    case 6:
        std::cout << "TODO: Idempotent operation selected." << std::endl;
        break;
    case 7:
        std::cout << "TODO: Non-idempotent operation selected." << std::endl;
        break;
    case 8:
        std::cout << "Echo Message selected." << std::endl;

        std::cout << "Enter message to send: ";
        std::cin.ignore();
        std::getline(std::cin, messageData);

        response = client.sendCustomMessage(messageData);
        std::cout << "Received response from server: " << response << std::endl;

        break;
    case 9:
        std::cout << "Exiting..." << std::endl;
        break;
    default:
        std::cout << "Invalid choice." << std::endl;
    }
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
        else if (choice < 1 || choice > 9)
        {
            std::cout << "Invalid choice. Please enter a number between 1 and 9." << std::endl;
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

    while (true)
    {
        std::cout << dayOfWeekMenu;
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

        if (std::regex_match(timeStr, timeRegex))
        {
            return timeStr;
        }
        else
        {
            std::cout << "Invalid time format. Please enter a valid time between 0000 and 2359." << std::endl;
        }
    }
}

std::string UserInterface::promptBookingID(std::string prompt)
{
    // TODO: Finalize regex for booking number (currently 6-digits)
    std::regex bookingIDRegex(R"(^\d{6}$)");
    std::string bookingID;

    while (true)
    {
        std::cout << prompt;
        std::cin >> bookingID;

        if (std::regex_match(bookingID, bookingIDRegex))
        {
            return bookingID;
        }
        else
        {
            std::cout << "Invalid booking ID. Please enter a 6-digit number." << std::endl;
        }
    }
}
