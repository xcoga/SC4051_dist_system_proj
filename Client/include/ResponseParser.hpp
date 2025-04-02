#ifndef RESPONSE_PARSER_HPP
#define RESPONSE_PARSER_HPP

#include <sstream>
#include <string>
#include <vector>

/**
 * @class ResponseParser
 * @brief Provides methods to parse server responses into structured data.
 * 
 * The ResponseParser class contains static methods to parse various types of server
 * responses, such as facility names, availabilities, booking details, and ratings.
 * It converts raw server responses into a vector of strings for easier processing and
 * display.
 */
class ResponseParser
{
public:
    /**
     * @brief Parses the response for querying facility names.
     * @param response The raw server response.
     * @return A vector of strings containing the parsed facility names or an error message.
     */
    static std::vector<std::string> parseQueryFacilityNamesResponse(const std::string &response);

    /**
     * @brief Parses the response for querying facility availability.
     * @param response The raw server response.
     * @param daysRequested A comma-separated list of days to filter availability (optional).
     * @return A vector of strings containing the parsed availability information or an error message.
     */
    static std::vector<std::string> parseQueryAvailabilityResponse(
        const std::string &response,
        const std::string &daysRequested = ""
    );

    /**
     * @brief Parses the response for booking a facility.
     * @param response The raw server response.
     * @return A vector of strings containing the parsed booking details or an error message.
     */
    static std::vector<std::string> parseBookFacilityResponse(const std::string &response);

    /**
     * @brief Parses the response for querying booking details.
     * @param response The raw server response.
     * @return A vector of strings containing the parsed booking details or an error message.
     */
    static std::vector<std::string> parseQueryBookingResponse(const std::string &response);

    /**
     * @brief Parses the response for updating a booking.
     * @param response The raw server response.
     * @return A vector of strings containing the parsed updated booking details or an error message.
     */
    static std::vector<std::string> parseUpdateBookingResponse(const std::string &response);

    /**
     * @brief Parses the response for deleting a booking.
     * @param response The raw server response.
     * @return A vector of strings containing the parsed deletion confirmation or an error message.
     */
    static std::vector<std::string> parseDeleteBookingResponse(const std::string &response);

    /**
     * @brief Parses the response for monitoring facility availability.
     * @param response The raw server response.
     * @return A vector of strings containing the parsed monitoring registration details or an error message.
     */
    static std::vector<std::string> parseMonitorAvailabilityResponse(const std::string &response);

    /**
     * @brief Parses the response for adding a rating to a facility.
     * @param response The raw server response.
     * @return A vector of strings containing the parsed rating confirmation or an error message.
     */
    static std::vector<std::string> parseRateFacilityResponse(const std::string &response);

    /**
     * @brief Parses the response for querying a rating of a facility.
     * @param response The raw server response.
     * @return A vector of strings containing the parsed rating details or an error message.
     */
    static std::vector<std::string> parseQueryRatingResponse(const std::string &response);

    /**
     * @brief Parses the response for echoing a message.
     * @param response The raw server response.
     * @return A vector of strings containing the parsed echoed message or an error message.
     */
    static std::vector<std::string> parseEchoMessageResponse(const std::string &response);

private:
    /**
     * @brief Checks if the response indicates an error.
     * @param responseStream The input stream containing the server response.
     * @return True if the response indicates an error, false otherwise.
     */
    static bool isErrorResponse(std::istringstream &responseStream);
};

#endif // RESPONSE_PARSER_HPP
