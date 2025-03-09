#include "RequestMessage.hpp"

RequestMessage::RequestMessage() : requestType(0), requestID(0), data("") {}

RequestMessage::RequestMessage(int requestType, int requestID, const std::string &data)
    : requestType(requestType), requestID(requestID), data(data) {}

std::string RequestMessage::getJavaClassName() const
{
    return "Server.RequestMessage";
}

std::vector<std::pair<std::string, std::string>> RequestMessage::getFieldMetadata() const
{
    return {
        {"requestType", "int"},
        {"requestID", "int"},
        {"data", "java.lang.String"}};
}

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

int RequestMessage::getRequestType() const
{
    return requestType;
}

int RequestMessage::getRequestID() const
{
    return requestID;
}

std::string RequestMessage::getData() const
{
    return data;
}

void RequestMessage::setRequestType(int type)
{
    requestType = type;
}

void RequestMessage::setRequestID(int id)
{
    requestID = id;
}

void RequestMessage::setData(const std::string &d)
{
    data = d;
}
