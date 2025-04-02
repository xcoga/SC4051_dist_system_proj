#include "ResponseParser.hpp"

#include <iostream>
#include <sstream>
#include <unordered_set>

#include "Constants.hpp"
#include "Serializer.hpp"

/**
 * @brief Parses the response for querying facility names.
 * 
 * This function takes the raw server response as input and parses it to extract facility names.
 * If the response indicates an error, it extracts the error message and returns it in a vector.
 * Otherwise, it returns a vector containing the parsed facility names.
 * 
 * @param response The raw server response.
 * 
 * @return A vector of strings containing the parsed facility names or an error message.
 */
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

/**
 * @brief Parses the response for querying facility availability.
 * 
 * This function takes the raw server response and an optional list of requested days as input.
 * It parses the response to extract availability information for the specified days.
 * If the response indicates an error, it extracts the error message and returns it in a vector.
 * Otherwise, it returns a vector containing the parsed availability information.
 * 
 * @param response The raw server response.
 * @param daysRequested A comma-separated list of days to filter availability (optional).
 * 
 * @return A vector of strings containing the parsed availability information or an error message.
 */
std::vector<std::string> ResponseParser::parseQueryAvailabilityResponse(
    const std::string &response,
    const std::string &daysRequested
)
{
    std::vector<std::string> parsedResponse;
    std::istringstream responseStream(response);

    bool filterDays = !daysRequested.empty(); // Only filter if daysRequested is not empty
    std::unordered_set<std::string> requestedDaysSet;

    if (filterDays)
    {
        std::istringstream daysStream(daysRequested);
        std::string dayToken;
        while (std::getline(daysStream, dayToken, ','))
        {
            requestedDaysSet.insert(dayToken);
        }
    }

    std::unordered_map<std::string, std::string> availabilityMap;

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
                availabilityMap[day] = formattedTimeslots;
                // parsedResponse.push_back(formattedTimeslots);
            }
        }

        // Only filter days if requested by user
        if (filterDays)
        {
            for (const std::string &day : Constants::DAYS_OF_WEEK)
            {
                if (requestedDaysSet.count(day))
                {
                    if (availabilityMap.find(day) != availabilityMap.end())
                    {
                        parsedResponse.push_back(availabilityMap[day]);
                    }
                    else
                    {
                        parsedResponse.push_back(day + ": Closed");
                    }
                }
            }
        }
        else
        {
            for (const std::string &day : Constants::DAYS_OF_WEEK)
            {
                if (availabilityMap.find(day) != availabilityMap.end())
                {
                    parsedResponse.push_back(availabilityMap[day]);
                }
            }
        }
    }

    return parsedResponse;
}

/**
 * @brief Parses the response for booking a facility.
 * 
 * The server returns the same message structure upon successful booking and querying booking details.
 * Hence, this function is identical to parseQueryBookingResponse() and serves only as a wrapper.
 * 
 * @param response The raw server response.
 * 
 * @return A vector of strings containing the parsed booking details or an error message.
 */
std::vector<std::string> ResponseParser::parseBookFacilityResponse(const std::string &response)
{
    // Both queryBooking and bookFacility responses are the same
    return parseQueryBookingResponse(response);
}

/**
 * @brief Parses the response for querying booking details.
 * 
 * This function takes the raw server response as input and parses it to extract booking details.
 * If the response indicates an error, it extracts the error message and returns it in a vector.
 * Otherwise, it returns a vector containing the parsed booking details.
 * 
 * @param response The raw server response.
 * 
 * @return A vector of strings containing the parsed booking details or an error message.
 */
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

/**
 * @brief Parses the response for updating a booking.
 * 
 * This function takes the raw server response as input and parses it to extract updated booking details.
 * If the response indicates an error, it extracts the error message and returns it in a vector.
 * Otherwise, it returns a vector containing the parsed updated booking details.
 * 
 * @param response The raw server response.
 * 
 * @return A vector of strings containing the parsed updated booking details or an error message.
 */
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
                oldBookingID = line.substr(13); // Remove "oldBookingID:" prefix
            }
            else if (line.find("newBookingID:") == 0)
            {
                newBookingID = line.substr(13); // Remove "newBookingID:" prefix
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

/**
 * @brief Parses the response for deleting a booking.
 * 
 * This function takes the raw server response as input and parses it to extract deletion confirmation.
 * If the response indicates an error, it extracts the error message and returns it in a vector.
 * Otherwise, it returns a vector containing the parsed deletion confirmation.
 * 
 * @param response The raw server response.
 * 
 * @return A vector of strings containing the parsed deletion confirmation or an error message.
 */
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

/**
 * @brief Parses the response for monitoring facility availability.
 * 
 * This function takes the raw server response as input and parses it to extract monitoring registration details.
 * If the response indicates an error, it extracts the error message and returns it in a vector.
 * Otherwise, it returns a vector containing the parsed monitoring registration details.
 * 
 * @param response The raw server response.
 * 
 * @return A vector of strings containing the parsed monitoring registration details or an error message.
 */
std::vector<std::string> ResponseParser::parseMonitorAvailabilityResponse(const std::string &response)
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
        std::string line, facility, duration;
        while (std::getline(responseStream, line))
        {
            if (line.find("facility:") == 0)
            {
                facility = line.substr(9); // Remove "facility:" prefix
            }
            else if (line.find("interval:") == 0)
            {
                duration = line.substr(9); // Remove "interval:" prefix
            }
        }

        parsedResponse.push_back("Monitoring Registration");
        parsedResponse.push_back("Facility: " + facility);
        parsedResponse.push_back("Duration: " + duration + "s");
    }

    return parsedResponse;
}

/**
 * @brief Parses the response for adding a rating to a facility.
 * 
 * This function takes the raw server response as input and parses it to extract rating confirmation.
 * If the response indicates an error, it extracts the error message and returns it in a vector.
 * Otherwise, it returns a vector containing the parsed rating confirmation.
 * 
 * @param response The raw server response.
 * 
 * @return A vector of strings containing the parsed rating confirmation or an error message.
 */
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

/**
 * @brief Parses the response for echoing a message.
 * 
 * This function takes the raw server response as input and parses it to extract the echoed message.
 * If the response indicates an error, it extracts the error message and returns it in a vector.
 * Otherwise, it returns a vector containing the parsed echoed message.
 * 
 * @param response The raw server response.
 * 
 * @return A vector of strings containing the parsed echoed message or an error message.
 */
std::vector<std::string> ResponseParser::parseEchoMessageResponse(const std::string &response)
{
    std::vector<std::string> parsedResponse;

    parsedResponse = {
        "Response from Server",
        "Message: " + response
    };

    return parsedResponse;
}

/**
 * @brief Checks if the response indicates an error.
 * 
 * This function takes an input stream containing the server response and checks if it indicates an error.
 * It returns true if the response indicates an error, false otherwise.
 * 
 * @param responseStream The input stream containing the server response.
 * 
 * @return True if the response indicates an error, false otherwise.
 */
bool ResponseParser::isErrorResponse(std::istringstream &responseStream)
{
    std::string line;

    if (std::getline(responseStream, line) && line.find(Constants::STATUS_ERROR) == 0)
    {
        return true;
    }
    return false;
}
