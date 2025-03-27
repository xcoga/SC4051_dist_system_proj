#ifndef RESPONSE_PARSER_HPP
#define RESPONSE_PARSER_HPP

#include <string>

class ResponseParser
{
public:
    static std::vector<std::string> parseQueryFacilityNamesResponse(const std::string &response);
    // static void parseQueryAvailabilityResponse(const std::string &response);
    // static void parseBookFacilityResponse(const std::string &response);
    // static void parseQueryBookingResponse(const std::string &response);
    // static void parseChangeBookingResponse(const std::string &response);
    // static void parseDeleteBookingResponse(const std::string &response);
    // static void parseMonitorAvailabilityResponse(const std::string &response);
    // static void parseRateFacilityResponse(const std::string &response);
    // static void parseQueryRatingResponse(const std::string &response);
    static std::vector<std::string> parseEchoMessageResponse(const std::string &response);

private:
    static bool isErrorResponse(const std::string &response, std::string &errorMessage);
    // static void checkResponseParity(const std::string &response);
};

#endif // RESPONSE_PARSER_HPP