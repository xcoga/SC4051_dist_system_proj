#include "Serializer.hpp"

#include "ByteBuffer.hpp"
#include "ByteReader.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cstring>
#include <stdexcept>
#include <functional>

// Initialize static members
std::map<int, std::shared_ptr<JavaSerializable>> JavaDeserializer::deserializedObjects;
int JavaDeserializer::objectCounter = 0;
std::map<std::string, std::function<std::shared_ptr<JavaSerializable>()>> ObjectFactory::creators;

// Initialize static members
std::map<const void *, int> JavaSerializer::serializedObjects;
int JavaSerializer::objectCounter = 0;

// JavaSerializer
std::vector<uint8_t> JavaSerializer::serialize(const JavaSerializable *obj)
{
    serializedObjects.clear();
    objectCounter = 0;
    ByteBuffer buffer;
    serializeObject(obj, buffer);
    return buffer.getBuffer();
}

void JavaSerializer::serializeObject(const JavaSerializable *obj, ByteBuffer &buffer)
{
    if (obj == nullptr)
    {
        buffer.writeByte(0); // null marker
        return;
    }

    buffer.writeByte(1); // non-null marker

    // Check for circular reference
    auto it = serializedObjects.find(obj);
    if (it != serializedObjects.end())
    {
        buffer.writeByte(1); // reference marker
        buffer.writeInt(it->second);
        return;
    }

    buffer.writeByte(0); // new object marker
    serializedObjects[obj] = objectCounter++;

    // Write class metadata
    std::string className = obj->getJavaClassName();
    buffer.writeString(className);

    // Write field count and metadata
    auto fields = obj->getFieldMetadata();
    buffer.writeInt(static_cast<int32_t>(fields.size()));

    // Write ALL field metadata first
    for (const auto &field : fields)
    {
        buffer.writeString(field.first);  // field name
        buffer.writeString(field.second); // field type
    }

    // Then call a new method to write values separately
    obj->serializeFieldValues(buffer);
}

// ObjectFactory
std::shared_ptr<JavaSerializable> ObjectFactory::createObject(const std::string &className)
{
    auto it = creators.find(className);
    if (it == creators.end())
    {
        throw std::runtime_error("Unknown class: " + className);
    }
    return it->second();
}

// JavaDeserializer

std::shared_ptr<JavaSerializable> JavaDeserializer::deserialize(const std::vector<uint8_t> &data)
{
    deserializedObjects.clear();
    objectCounter = 0;
    ByteReader reader(data);
    return deserializeObject(reader);
}

std::shared_ptr<JavaSerializable> JavaDeserializer::deserializeObject(ByteReader &reader)
{
    uint8_t nullMarker = reader.readByte();
    if (nullMarker == 0)
    {
        return nullptr;
    }

    uint8_t referenceMarker = reader.readByte();
    if (referenceMarker == 1)
    {
        int objectId = reader.readInt();
        auto it = deserializedObjects.find(objectId);
        if (it == deserializedObjects.end())
        {
            throw std::runtime_error("Invalid object reference");
        }
        return it->second;
    }

    // Read class metadata
    std::string className = reader.readString();

    // Create object using factory
    auto obj = ObjectFactory::createObject(className);
    deserializedObjects[objectCounter++] = obj;

    // Read field count
    int fieldCount = reader.readInt();

    // Read field metadata
    std::vector<std::pair<std::string, std::string>> expectedFields = obj->getFieldMetadata();
    if (fieldCount != expectedFields.size())
    {
        throw std::runtime_error("Field count mismatch");
    }

    // Read and verify field metadata
    for (int i = 0; i < fieldCount; i++)
    {
        std::string fieldName = reader.readString();
        std::string fieldType = reader.readString();

        if (fieldName != expectedFields[i].first ||
            fieldType != expectedFields[i].second)
        {
            throw std::runtime_error("Field metadata mismatch");
        }
    }

    // Deserialize fields
    obj->deserializeFields(reader);
    return obj;
}
