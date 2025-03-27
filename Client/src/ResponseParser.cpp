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
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            // Remove "message: " prefix
            std::string errorMessage = line.substr(8);
            parsedResponse.push_back(errorMessage);
        }
    }
    else
    {
        parsedResponse.push_back("Facility Names");

        // Add comma-separated facility names to the parsed response
        std::string line;
        while (std::getline(responseStream, line))
        {
            if (line.find("facilityNames:") == 0)
            {
                // Extract the list of facility names
                std::string facilityNames = line.substr(14); // Remove "facilityNames:" prefix
                std::istringstream facilityNamesStream(facilityNames);
                std::string facilityName;

                // Split the facility names by comma
                while (std::getline(facilityNamesStream, facilityName, ','))
                {
                    if (!facilityName.empty())
                    {
                        parsedResponse.push_back(facilityName);
                    }
                }
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
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            // Remove "message: " prefix
            std::string errorMessage = line.substr(8);
            parsedResponse.push_back(errorMessage);
        }
    }
    else
    {
        parsedResponse.push_back("Facility Availability");

        std::string line;
        while (std::getline(responseStream, line))
        {
            // Extract facility name
            if (line.find("facility:") == 0)
            {
                // Remove "facility:" prefix
                parsedResponse.push_back("Facility: " + line.substr(9));
                continue;
            }

            // Skip the header line
            if (line.find("availableTimeslots:") == 0)
            {
                continue;
            }

            // Parse each day's availability
            std::istringstream lineStream(line);
            std::string day, timeslot, formattedTimeslots;
            if (std::getline(lineStream, day, ':'))
            {
                formattedTimeslots = day + ": ";

                while (std::getline(lineStream, timeslot, ','))
                {
                    // Ignore whitespaces as well
                    if (!timeslot.empty())
                    {
                        size_t dashPos = timeslot.find('-');
                        if (dashPos != std::string::npos)
                        {
                            timeslot.replace(dashPos, 1, "to");
                        }

                        formattedTimeslots += (timeslot + ", ");
                    }
                }

                // Remove trailing comma and space
                if (formattedTimeslots.size() > 2)
                {
                    formattedTimeslots.erase(formattedTimeslots.size() - 2, 2);
                }

                // Add formatted timeslots to parsed response
                parsedResponse.push_back(formattedTimeslots);
            }
        }
    }

    return parsedResponse;
}

std::vector<std::string> ResponseParser::parseBookFacilityResponse(const std::string &response)
{
    // Both queryBooking and bookFacility responses are the same
    return parseQueryBookingResponse(response);
}

std::vector<std::string> ResponseParser::parseQueryBookingResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;
    std::istringstream responseStream(response);

    if (isErrorResponse(responseStream))
    {
        parsedResponse.push_back("Error");

        std::string line;
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            // Remove "message: " prefix
            std::string errorMessage = line.substr(8);
            parsedResponse.push_back(errorMessage);
        }
    }
    else
    {
        parsedResponse.push_back("Booking Details");

        std::string line;
        while (std::getline(responseStream, line))
        {
            if (line.find("bookingID:") == 0)
            {
                // Remove "bookingID:" prefix
                parsedResponse.push_back("Booking ID: " + line.substr(10));
            }

            if (line.find("user:") == 0)
            {
                // Remove "user:" prefix
                parsedResponse.push_back("User: " + line.substr(5));
            }

            if (line.find("facility:") == 0)
            {
                // Remove "facility:" prefix
                parsedResponse.push_back("Facility: " + line.substr(9));
            }

            if (line.find("day:") == 0)
            {
                // Remove "day:" prefix
                parsedResponse.push_back("Day: " + line.substr(4));
            }

            if (line.find("startTime:") == 0)
            {
                // Remove "startTime:" prefix
                parsedResponse.push_back("Start Time: " + line.substr(10));
            }

            if (line.find("endTime:") == 0)
            {
                // Remove "endTime:" prefix
                parsedResponse.push_back("End Time: " + line.substr(8));
            }
        }
    }

    return parsedResponse;
}

std::vector<std::string> ResponseParser::parseChangeBookingResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;
    std::istringstream responseStream(response);

    if (isErrorResponse(responseStream))
    {
        parsedResponse.push_back("Error");

        std::string line;
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            // Remove "message: " prefix
            std::string errorMessage = line.substr(8);
            parsedResponse.push_back(errorMessage);
        }
    }
    else
    {
        parsedResponse.push_back("New Booking Details");

        std::string line;
        while (std::getline(responseStream, line))
        {
            if (line.find("oldBookingID:") == 0)
            {
                // Remove "oldBookingID:" prefix
                parsedResponse.push_back("Successfully changed booking " + line.substr(12) + ".");
            }

            if (line.find("newBookingID:"))
            {
                // Remove "newBookingID:" prefix
                parsedResponse.push_back("New Booking ID: " + line.substr(12));
            }

            if (line.find("user:") == 0)
            {
                // Remove "user:" prefix
                parsedResponse.push_back("User: " + line.substr(5));
            }

            if (line.find("facility:") == 0)
            {
                // Remove "facility:" prefix
                parsedResponse.push_back("Facility: " + line.substr(9));
            }

            if (line.find("day:") == 0)
            {
                // Remove "day:" prefix
                parsedResponse.push_back("New Day: " + line.substr(4));
            }

            if (line.find("startTime:") == 0)
            {
                // Remove "startTime:" prefix
                parsedResponse.push_back("New Start Time: " + line.substr(10));
            }

            if (line.find("endTime:") == 0)
            {
                // Remove "endTime:" prefix
                parsedResponse.push_back("New End Time: " + line.substr(8));
            }
        }
    }

    return parsedResponse;
}

std::vector<std::string> ResponseParser::parseDeleteBookingResponse(const std::string &response)
{
    // TODO
}

std::vector<std::string> ResponseParser::parseMonitorAvailabilityResponse(const std::string &response)
{
    // TODO
}

std::vector<std::string> ResponseParser::parseRateFacilityResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;
    std::istringstream responseStream(response);

    if (isErrorResponse(responseStream))
    {
        parsedResponse.push_back("Error");

        std::string line;
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            // Remove "message: " prefix
            std::string errorMessage = line.substr(8);
            parsedResponse.push_back(errorMessage);
        }
    }
    else
    {
        parsedResponse.push_back("Rating Added");

        std::string line;
        while (std::getline(responseStream, line))
        {
            if (line.find("user:") == 0)
            {
                // Remove "user:" prefix
                parsedResponse.push_back("User: " + line.substr(5));
            }

            if (line.find("facility:") == 0)
            {
                // Remove "facility:" prefix
                parsedResponse.push_back("Facility: " + line.substr(9));
            }

            if (line.find("rating:") == 0)
            {
                // Remove "rating:" prefix
                parsedResponse.push_back("Rating: " + line.substr(7));
            }
        }
    }

    return parsedResponse;
}

std::vector<std::string> ResponseParser::parseQueryRatingResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;
    std::istringstream responseStream(response);

    if (isErrorResponse(responseStream))
    {
        parsedResponse.push_back("Error");

        std::string line;
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            // Remove "message: " prefix
            std::string errorMessage = line.substr(8);
            parsedResponse.push_back(errorMessage);
        }
    }
    else
    {
        parsedResponse.push_back("Facility Rating");

        std::string line;
        while (std::getline(responseStream, line))
        {
            if (line.find("facility:") == 0)
            {
                // Remove "facility:" prefix
                parsedResponse.push_back("Facility: " + line.substr(9));
            }

            if (line.find("rating:") == 0)
            {
                // Remove "rating:" prefix
                parsedResponse.push_back("Rating: " + line.substr(7));
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

    if (std::getline(responseStream, line) && line.find("status:ERROR") == 0)
    {
        return true;
    }
    return false;
}
