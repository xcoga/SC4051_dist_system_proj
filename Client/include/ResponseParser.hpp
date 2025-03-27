#ifndef RESPONSE_PARSER_HPP
#define RESPONSE_PARSER_HPP

#include <string>

class ResponseParser
{
public:
    static std::vector<std::string> parseQueryFacilityNamesResponse(const std::string &response);
    static std::vector<std::string> parseQueryAvailabilityResponse(const std::string &response, const std::string &facilityName);
    static std::vector<std::string> parseBookFacilityResponse(
        const std::string &response,
        const std::string &facilityName,
        const std::string &dayOfWeek,
        const std::string &startTime,
        const std::string &endTime
    );
    // static void parseQueryBookingResponse(const std::string &response);
    // static void parseChangeBookingResponse(const std::string &response);
    // static void parseDeleteBookingResponse(const std::string &response);
    // static void parseMonitorAvailabilityResponse(const std::string &response);
    // static void parseRateFacilityResponse(const std::string &response);
    // static void parseQueryRatingResponse(const std::string &response);
    static std::vector<std::string> parseEchoMessageResponse(const std::string &response);

private:
    static bool isErrorResponse(std::istringstream &responseStream);
    // static void checkResponseParity(const std::string &response);
};

#endif // RESPONSE_PARSER_HPP