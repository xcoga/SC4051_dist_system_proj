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
        std::string line, errorMessage;
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            errorMessage = line.substr(8); // Remove "message:" prefix
        }

        parsedResponse.push_back("Error");
        parsedResponse.push_back(errorMessage);
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

std::vector<std::string> ResponseParser::parseQueryAvailabilityResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;
    std::istringstream responseStream(response);

    if (isErrorResponse(responseStream))
    {
        std::string line, errorMessage;
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            errorMessage = line.substr(8); // Remove "message:" prefix
        }

        parsedResponse.push_back("Error");
        parsedResponse.push_back(errorMessage);
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
        std::string line, errorMessage;
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            errorMessage = line.substr(8); // Remove "message:" prefix
        }

        parsedResponse.push_back("Error");
        parsedResponse.push_back(errorMessage);
    }
    else
    {
        std::string line, bookingID, userInfo, facility, day, startTime, endTime;
        while (std::getline(responseStream, line))
        {
            if (line.find("bookingID:") == 0)
            {
                bookingID = line.substr(10); // Remove "bookingID:" prefix
            }
            else if (line.find("user:") == 0)
            {
                userInfo = line.substr(5); // Remove "user:" prefix
            }
            else if (line.find("facility:") == 0)
            {
                facility = line.substr(9); // Remove "facility:" prefix
            }
            else if (line.find("day:") == 0)
            {
                day = line.substr(4); // Remove "day:" prefix
            }
            else if (line.find("startTime:") == 0)
            {
                startTime = line.substr(10); // Remove "startTime:" prefix
            }
            else if (line.find("endTime:") == 0)
            {
                endTime = line.substr(8);// Remove "endTime:" prefix
            }
        }

        // Push elements into parsedResponse in the desired order
        parsedResponse.push_back("Booking Details");
        parsedResponse.push_back("Booking ID: " + bookingID);
        parsedResponse.push_back("User: " + userInfo);
        parsedResponse.push_back("Facility: " + facility);
        parsedResponse.push_back("Day: " + day);
        parsedResponse.push_back("Start Time: " + startTime);
        parsedResponse.push_back("End Time: " + endTime);
    }

    return parsedResponse;
}

std::vector<std::string> ResponseParser::parseUpdateBookingResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;
    std::istringstream responseStream(response);

    if (isErrorResponse(responseStream))
    {
        std::string line, errorMessage;
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            errorMessage = line.substr(8); // Remove "message:" prefix
        }

        parsedResponse.push_back("Error");
        parsedResponse.push_back(errorMessage);
    }
    else
    {
        std::string line, oldBookingID, newBookingID, userInfo, facility, day, startTime, endTime;
        while (std::getline(responseStream, line))
        {
            if (line.find("oldBookingID:") == 0)
            {
                oldBookingID = line.substr(12); // Remove "oldBookingID:" prefix
            }
            else if (line.find("newBookingID:") == 0)
            {
                newBookingID = line.substr(12); // Remove "newBookingID:" prefix
            }
            else if (line.find("user:") == 0)
            {
                userInfo = line.substr(5); // Remove "user:" prefix
            }
            else if (line.find("facility:") == 0)
            {
                facility = line.substr(9); // Remove "facility:" prefix
            }
            else if (line.find("day:") == 0)
            {
                day = line.substr(4); // Remove "day:" prefix
            }
            else if (line.find("startTime:") == 0)
            {
                startTime = line.substr(10); // Remove "startTime:" prefix
            }
            else if (line.find("endTime:") == 0)
            {
                endTime = line.substr(8); // Remove "endTime:" prefix
            }
        }

        parsedResponse.push_back("New Booking Details");
        parsedResponse.push_back("New Booking ID: " + newBookingID);
        parsedResponse.push_back("User: " + userInfo);
        parsedResponse.push_back("Facility: " + facility);
        parsedResponse.push_back("New Day: " + day);
        parsedResponse.push_back("New Start Time: " + startTime);
        parsedResponse.push_back("New End Time: " + endTime);
    }

    return parsedResponse;
}

std::vector<std::string> ResponseParser::parseDeleteBookingResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;
    std::istringstream responseStream(response);

    if (isErrorResponse(responseStream))
    {
        std::string line, errorMessage;
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            errorMessage = line.substr(8); // Remove "message:" prefix
        }

        parsedResponse.push_back("Error");
        parsedResponse.push_back(errorMessage);
    }
    else
    {
        std::string line, oldBookingID, userInfo;
        while (std::getline(responseStream, line))
        {
            if (line.find("bookingID:") == 0)
            {
                oldBookingID = line.substr(10); // Remove "bookingID:" prefix
            }
            else if (line.find("user:") == 0)
            {
                userInfo = line.substr(5); // Remove "user:" prefix
            }
        }

        parsedResponse.push_back("Booking Deleted");
        parsedResponse.push_back("Old Booking ID: " + oldBookingID);
        parsedResponse.push_back("User: " + userInfo);
    }

    return parsedResponse;
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
        std::string line, errorMessage;
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            errorMessage = line.substr(8); // Remove "message:" prefix
        }

        parsedResponse.push_back("Error");
        parsedResponse.push_back(errorMessage);
    }
    else
    {
        std::string line, userInfo, facility, rating;
        while (std::getline(responseStream, line))
        {
            if (line.find("user:") == 0)
            {
                userInfo = line.substr(5); // Remove "user:" prefix
            }
            else if (line.find("facility:") == 0)
            {
                facility = line.substr(9); // Remove "facility:" prefix
            }
            else if (line.find("rating:") == 0)
            {
                rating = line.substr(7); // Remove "rating:" prefix
            }
        }

        parsedResponse.push_back("Rating Added");
        parsedResponse.push_back("User: " + userInfo);
        parsedResponse.push_back("Facility: " + facility);
        parsedResponse.push_back("Rating: " + rating);
    }

    return parsedResponse;
}

std::vector<std::string> ResponseParser::parseQueryRatingResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;
    std::istringstream responseStream(response);

    if (isErrorResponse(responseStream))
    {
        std::string line, errorMessage;
        if (std::getline(responseStream, line) && line.find("message:") == 0)
        {
            errorMessage = line.substr(8); // Remove "message:" prefix
        }

        parsedResponse.push_back("Error");
        parsedResponse.push_back(errorMessage);
    }
    else
    {
        std::string line, facility, rating;
        while (std::getline(responseStream, line))
        {
            if (line.find("facility:") == 0)
            {
                facility = line.substr(9); // Remove "facility:" prefix
            }
            else if (line.find("rating:") == 0)
            {
                rating = line.substr(7); // Remove "rating:" prefix
            }
        }

        parsedResponse.push_back("Facility Rating");
        parsedResponse.push_back("Facility: " + facility);
        parsedResponse.push_back("Rating: " + rating);
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
