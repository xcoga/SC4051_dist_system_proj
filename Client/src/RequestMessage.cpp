#include "RequestMessage.hpp"

/**
 * @brief Default constructor for RequestMessage.
 * 
 * Initializes the request type and ID to 0 and the data to an empty string.
 */
RequestMessage::RequestMessage() : requestType(0), requestID(0), data("") {}

/**
 * @brief Parameterized constructor for RequestMessage.
 * 
 * This constructor is used to create request messages with specific parameters.
 * 
 * @param requestType The type of request (e.g., READ, WRITE, etc.).
 * @param requestID The unique identifier for the request.
 * @param data The associated data for the request.
 */
RequestMessage::RequestMessage(int requestType, int requestID, const std::string &data)
    : requestType(requestType), requestID(requestID), data(data) {}

/**
 * @brief Gets the Java class name for serialization.
 * 
 * This method returns the Java class name that corresponds to this C++ class.
 * 
 * @return The Java class name as a string.
 */
std::string RequestMessage::getJavaClassName() const
{
    return "Server.RequestMessage";
}

/**
 * @brief Gets the metadata about the fields in the request message.
 * 
 * This method returns a vector of pairs representing field names and their types.
 * 
 * @return A vector of pairs representing field names and their types.
 */
std::vector<std::pair<std::string, std::string>> RequestMessage::getFieldMetadata() const
{
    return {
        {"requestType", "int"},
        {"requestID", "int"},
        {"data", "java.lang.String"}
    };
}

/**
 * @brief Serializes the field values into a byte buffer.
 * 
 * This method writes the field values of the request message into a byte buffer.
 * 
 * @param buffer The byte buffer to write the serialized data to.
 */
void RequestMessage::serializeFieldValues(ByteBuffer &buffer) const
{
    buffer.writeInt(requestType);
    buffer.writeInt(requestID);

    if (data.empty())
    {
        buffer.writeByte(0);
    }
    else
    {
        buffer.writeByte(1);
        buffer.writeString(data);
    }
}

/**
 * @brief Deserializes the fields from a byte reader.
 * 
 * This method reads the field values of the request message from a byte reader.
 * 
 * @param reader The byte reader to read the serialized data from.
 */
void RequestMessage::deserializeFields(ByteReader &reader)
{
    requestType = reader.readInt();
    requestID = reader.readInt();

    int dataNullMarker = reader.readByte();
    if (dataNullMarker == 1)
    {
        data = reader.readString();
    }
    else
    {
        data = "";
    }
}

/**
 * @brief Gets the request type.
 * 
 * @return The request type as an integer.
 */
int RequestMessage::getRequestType() const
{
    return requestType;
}

/**
 * @brief Gets the request ID.
 * 
 * @return The request ID as an integer.
 */
int RequestMessage::getRequestID() const
{
    return requestID;
}

/**
 * @brief Gets the associated data for the request.
 * 
 * @return The associated data as a string.
 */
std::string RequestMessage::getData() const
{
    return data;
}

/**
 * @brief Sets the request type.
 * 
 * @param type The request type as an integer.
 */
void RequestMessage::setRequestType(int type)
{
    requestType = type;
}

/**
 * @brief Sets the request ID.
 * 
 * @param id The request ID as an integer.
 */
void RequestMessage::setRequestID(int id)
{
    requestID = id;
}

/**
 * @brief Sets the associated data for the request.
 * 
 * @param d The associated data as a string.
 */
void RequestMessage::setData(const std::string &d)
{
    data = d;
}
