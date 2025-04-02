#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>
#include <vector>

/**
 * @namespace Constants
 * @brief Defines constants used throughout the project.
 * 
 * This file contains global constants for configuration, menu string vectors, status strings and other fixed values used in the project.
 * These constants are centralized for better maintainability and readability.
 */
namespace Constants
{
    // General constants
    const int TIMEOUT_SEC = 5;
    const int MAX_RETRIES = 5;
    const int BUFFER_SIZE = 1024;
    const std::vector<std::string> DAYS_OF_WEEK = {
        "MONDAY",
        "TUESDAY",
        "WEDNESDAY",
        "THURSDAY",
        "FRIDAY",
        "SATURDAY",
        "SUNDAY"
    };

    // Menu string vectors
    const std::vector<std::string> MAIN_MENU = {
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
    const std::vector<std::string> DAYS_OF_WEEK_MENU = {
        "Day of Week",
        "1. MONDAY",
        "2. TUESDAY",
        "3. WEDNESDAY",
        "4. THURSDAY",
        "5. FRIDAY",
        "6. SATURDAY",
        "7. SUNDAY"
    };

    // Status strings
    const std::string STATUS_SUCCESS = "status:SUCCESS";
    const std::string STATUS_ERROR = "status:ERROR";
}

#endif // CONSTANTS_HPP
