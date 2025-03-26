#include "ResponseParser.hpp"

#include "Serializer.hpp"

#include <iostream>

std::vector<std::string> ResponseParser::parseEchoResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;

    parsedResponse = {
        "Response from Server",
        "Message: " + response
    };

    return parsedResponse;
}