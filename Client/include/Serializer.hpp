#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cstring>
#include <stdexcept>
#include <functional>

#include "ByteBuffer.hpp"
#include "ByteReader.hpp"

class JavaSerializable
{
public:
    virtual ~JavaSerializable() = default;
    virtual std::string getJavaClassName() const = 0;
    virtual std::vector<std::pair<std::string, std::string>> getFieldMetadata() const = 0;

    // This remains for backward compatibility but will be empty in new implementations
    virtual void serializeFields(ByteBuffer &buffer) const {}

    // New method to serialize field values
    virtual void serializeFieldValues(ByteBuffer &buffer) const = 0;

    virtual void deserializeFields(ByteReader &reader) = 0;
};

class JavaSerializer
{
private:
    static std::map<const void *, int> serializedObjects;
    static int objectCounter;

public:
    static std::vector<uint8_t> serialize(const JavaSerializable *obj);

private:
    static void serializeObject(const JavaSerializable *obj, ByteBuffer &buffer);
};

class ObjectFactory
{
private:
    static std::map<std::string, std::function<std::shared_ptr<JavaSerializable>()>> creators;

public:
    template <typename T>
    static void registerClass(const std::string &className)
    {
        creators[className] = []()
        { return std::make_shared<T>(); };
    }

    static std::shared_ptr<JavaSerializable> createObject(const std::string &className);
};

class JavaDeserializer
{
private:
    static std::map<int, std::shared_ptr<JavaSerializable>> deserializedObjects;
    static int objectCounter;

public:
    static std::shared_ptr<JavaSerializable> deserialize(const std::vector<uint8_t> &data);

private:
    static std::shared_ptr<JavaSerializable> deserializeObject(ByteReader &reader);
};

#endif // SERIALIZER_HPP