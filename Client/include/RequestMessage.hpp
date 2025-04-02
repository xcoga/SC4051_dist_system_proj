#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include <string>

#include "Serializer.hpp"

/**
 * @class RequestMessage
 * @brief Represents a request message sent to the server.
 * 
 * The RequestMessage class encapsulates the details of a request sent to the server,
 * including the request type, request ID, and associated data. It supports serialization
 * and deserialization for communication with the server.
 */
class RequestMessage : public JavaSerializable
{
private:
    int requestType; ///< Type of request (e.g., READ, WRITE, etc.).
    int requestID; ///< Unique identifier for the request.
    std::string data; ///<  Associated data for the request.

public:
    /**
     * @enum RequestType
     * @brief Enumerates the types of requests that can be sent to the server.
     */
    enum RequestType
    {
        INVALID = -1, ///< Invalid request type.
        READ = 0, ///< Read operation.
        WRITE = 1, ///< Write operation.
        UPDATE = 2, ///< Update operation.
        DELETE_REQUEST = 3, ///< Delete operation.
        MONITOR = 4, ///< Monitor operation.
        ECHO = 5 ///< Echo operation for debugging.
    };

    /**
     * @brief Default constructor for RequestMessage.
     */
    RequestMessage();

    /**
     * @brief Parameterized constructor for RequestMessage.
     * @param requestType The type of request (e.g., READ, WRITE, etc.).
     * @param requestID The unique identifier for the request.
     * @param data The associated data for the request.
     */
    RequestMessage(int requestType, int requestID, const std::string &data);

    /**
     * @brief Gets the Java class name for serialization.
     * @return The Java class name as a string.
     */
    std::string getJavaClassName() const override;

    /**
     * @brief Gets the metadata about the fields in the request message.
     * @return A vector of pairs representing field names and their types.
     */
    std::vector<std::pair<std::string, std::string>> getFieldMetadata() const override;

    /**
     * @brief Serializes the field values into a byte buffer.
     * @param buffer The byte buffer to write the serialized data to.
     */
    void serializeFieldValues(ByteBuffer &buffer) const override;

    /**
     * @brief Deserializes the fields from a byte reader.
     * @param reader The byte reader to read the serialized data from.
     */
    void deserializeFields(ByteReader &reader) override;

    /**
     * @brief Gets the request type.
     * @return The request type as an integer.
     */
    int getRequestType() const;

    /**
     * @brief Gets the request ID.
     * @return The request ID as an integer.
     */
    int getRequestID() const;

    /**
     * @brief Gets the associated data for the request.
     * @return The associated data as a string.
     */
    std::string getData() const;

    /**
     * @brief Sets the request type.
     * @param type The request type as an integer.
     */
    void setRequestType(int type);

    /**
     * @brief Sets the request ID.
     * @param id The request ID as an integer.
     */
    void setRequestID(int id);

    /**
     * @brief Sets the associated data for the request.
     * @param data The associated data as a string.
     */
    void setData(const std::string &data);
};

#endif // REQUEST_MESSAGE_HPP
