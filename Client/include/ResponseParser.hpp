#ifndef RESPONSE_PARSER_HPP
#define RESPONSE_PARSER_HPP

#include <string>

class ResponseParser
{
public:
    static void parseResponse(const std::string &response);
    // static void parseQueryAvailabilityResponse(const std::string &response);
    // static void parseBookFacilityResponse(const std::string &response);
    // static void parseChangeBookingResponse(const std::string &response);
    // static void parseMonitorAvailabilityResponse(const std::string &response);
    // TODO: Parse idempotent and non-idempotent operation responses
    static std::vector<std::string> parseEchoResponse(const std::string &response);

// private:
    // static void checkResponseParity(const std::string &response);
};

#endif // RESPONSE_PARSER_HPP