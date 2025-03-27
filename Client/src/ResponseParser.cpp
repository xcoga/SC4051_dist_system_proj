#include "ResponseParser.hpp"

#include "Serializer.hpp"

#include <iostream>
#include <sstream>

/* Public methods */
std::vector<std::string> ResponseParser::parseQueryFacilityNamesResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;
    std::string errorMessage;

    if (isErrorResponse(response, errorMessage))
    {
        parsedResponse.push_back("Error");
        parsedResponse.push_back(errorMessage);
    }
    else
    {
        parsedResponse.push_back("Facility Names");

        std::istringstream responseStream(response);
        std::string line;

        if (std::getline(responseStream, line) && line == "status: SUCCESS")
        {
            while (std::getline(responseStream, line, ','))
            {
                if (!line.empty())
                {
                    parsedResponse.push_back(line);
                }
            }
        }
    }

    return parsedResponse;
}

std::vector<std::string> ResponseParser::parseEchoMessageResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;

    parsedResponse = {
        "Response from Server",
        "Message: " + response
    };

    return parsedResponse;
}

/* Private methods */
bool ResponseParser::isErrorResponse(const std::string &response, std::string &errorMessage)
{
    std::istringstream responseStream(response);
    std::string line;

    if (std::getline(responseStream, line) && line.find("ERROR") == 0)
    {
        if (std::getline(responseStream, line) && line.find("message: ") == 0)
        {
            // Remove "message: " prefix
            errorMessage = line.substr(9);
        }
        return true;
    }
    return false;
}
