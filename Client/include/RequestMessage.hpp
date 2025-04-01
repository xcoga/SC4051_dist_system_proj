#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include <string>

#include "Serializer.hpp"

class RequestMessage : public JavaSerializable
{
private:
    int requestType;
    int requestID;
    std::string data;

public:
    enum RequestType
    {
        INVALID = -1,
        READ = 0,
        WRITE = 1,
        UPDATE = 2,
        DELETE_REQUEST = 3,
        MONITOR = 4,
        ECHO = 5
    };

    RequestMessage();
    RequestMessage(int requestType, int requestID, const std::string &data);

    std::string getJavaClassName() const override;
    std::vector<std::pair<std::string, std::string>> getFieldMetadata() const override;
    void serializeFieldValues(ByteBuffer &buffer) const override;
    void deserializeFields(ByteReader &reader) override;

    // Getters
    int getRequestType() const;
    int getRequestID() const;
    std::string getData() const;

    // Setters
    void setRequestType(int type);
    void setRequestID(int id);
    void setData(const std::string &data);
};

#endif // REQUEST_MESSAGE_HPP
