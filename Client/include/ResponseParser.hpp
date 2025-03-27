#ifndef RESPONSE_PARSER_HPP
#define RESPONSE_PARSER_HPP

#include <string>

class ResponseParser
{
public:
    static std::vector<std::string> parseQueryFacilityNamesResponse(const std::string &response);
    static std::vector<std::string> parseQueryAvailabilityResponse(const std::string &response, const std::string &facilityName);
    static std::vector<std::string> parseBookFacilityResponse(const std::string &response);
    // static std::vector<std::string> parseQueryBookingResponse(const std::string &response);
    static std::vector<std::string> parseChangeBookingResponse(const std::string &response);
    // static std::vector<std::string> parseDeleteBookingResponse(const std::string &response);
    // static std::vector<std::string> parseMonitorAvailabilityResponse(const std::string &response);
    static std::vector<std::string> parseRateFacilityResponse(const std::string &response);
    // static std::vector<std::string> parseQueryRatingResponse(const std::string &response);
    static std::vector<std::string> parseEchoMessageResponse(const std::string &response);

private:
    static bool isErrorResponse(std::istringstream &responseStream);
};

#endif // RESPONSE_PARSER_HPP