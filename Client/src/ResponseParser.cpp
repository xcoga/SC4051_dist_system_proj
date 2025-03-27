#include "ResponseParser.hpp"

#include "Serializer.hpp"

#include <iostream>
#include <sstream>

/* Public methods */
std::vector<std::string> ResponseParser::parseQueryFacilityNamesResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;
    std::istringstream responseStream(response);

    if (isErrorResponse(responseStream))
    {
        parsedResponse.push_back("Error");

        std::string line;
        if (std::getline(responseStream, line) && line.find("message: ") == 0)
        {
            // Remove "message: " prefix
            std::string errorMessage = line.substr(9);
            parsedResponse.push_back(errorMessage);
        }
    }
    else
    {
        parsedResponse.push_back("Facility Names");

        // Add comma-separated facility names to the parsed response
        std::string facilityName;
        while (std::getline(responseStream, facilityName, ','))
        {
            if (!facilityName.empty())
            {
                parsedResponse.push_back(facilityName);
            }
        }
    }

    return parsedResponse;
}

std::vector<std::string> ResponseParser::parseQueryAvailabilityResponse(const std::string &response, const std::string &facilityName)
{
    std::vector<std::string> parsedResponse;
    std::istringstream responseStream(response);

    if (isErrorResponse(responseStream))
    {
        parsedResponse.push_back("Error");

        std::string line;
        if (std::getline(responseStream, line) && line.find("message: ") == 0)
        {
            // Remove "message: " prefix
            std::string errorMessage = line.substr(9);
            parsedResponse.push_back(errorMessage);
        }
    }
    else
    {
        parsedResponse.push_back("Availability for " + facilityName);

        std::string line;
        while (std::getline(responseStream, line))
        {
            // Skip the header line
            if (line.find("Available timeslots:") == 0)
            {
                continue;
            }

            // Parse each day's availability
            std::istringstream lineStream(line);
            std::string day, timeslot, formattedTimeslots;
            if (std::getline(lineStream, day, ':'))
            {
                formattedTimeslots = day + ":";

                while (std::getline(lineStream, timeslot, ','))
                {
                    // Ignore whitespaces as well
                    if (!timeslot.empty() && timeslot != " ")
                    {
                        size_t dashPos = timeslot.find('-');
                        if (dashPos != std::string::npos)
                        {
                            timeslot.replace(dashPos, 1, "to");
                        }

                        formattedTimeslots += (timeslot + ",");
                    }
                }

                // Remove trailing comma
                if (formattedTimeslots.size() > 1)
                {
                    formattedTimeslots.erase(formattedTimeslots.size() - 1);
                }

                // Add formatted timeslots to parsed response
                parsedResponse.push_back(formattedTimeslots);
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
bool ResponseParser::isErrorResponse(std::istringstream &responseStream)
{
    std::string line;

    if (std::getline(responseStream, line) && line.find("ERROR") == 0)
    {
        return true;
    }
    return false;
}
