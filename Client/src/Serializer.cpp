#include "Serializer.hpp"

#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "ByteBuffer.hpp"
#include "ByteReader.hpp"
#include "Parity.hpp"

// Initialize static members
std::map<int, std::shared_ptr<JavaSerializable>> JavaDeserializer::deserializedObjects;
int JavaDeserializer::objectCounter = 0;
std::map<std::string, std::function<std::shared_ptr<JavaSerializable>()>> ObjectFactory::creators;
std::map<const void *, int> JavaSerializer::serializedObjects;
int JavaSerializer::objectCounter = 0;

/* JavaSerializer */
/**
 * @brief Serializes a JavaSerializable object into a byte buffer.
 * 
 * This method handles circular references by maintaining a map of serialized objects.
 * It writes the object type, field metadata, and field values into a byte buffer.
 * 
 * @param obj The object to serialize.
 * 
 * @return A vector of bytes containing the serialized data.
 */
std::vector<uint8_t> JavaSerializer::serialize(const JavaSerializable *obj)
{
    serializedObjects.clear();
    objectCounter = 0;
    ByteBuffer buffer;
    serializeObject(obj, buffer);

    // Modify buffer in memory
    uint8_t parityBit = Parity::calculateEvenParityBit(buffer.getBuffer());
    buffer.writeByte(parityBit); // Assuming ByteBuffer has an append method

    return buffer.getBuffer();
}

/**
 * @brief Serializes the object and writes it into a byte buffer.
 * 
 * This method handles circular references by checking if the object has already been serialized.
 * It writes the object type, field metadata, and field values into the buffer.
 * 
 * @param obj The object to serialize.
 * @param buffer The byte buffer to write the serialized data into.
 */
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

/* ObjectFactory */
/**
 * @brief Creates an object based on its class name.
 * 
 * This method uses a factory pattern to create objects of different types.
 * 
 * @param className The name of the class.
 * 
 * @return A shared pointer to the created object.
 * 
 * @throws std::runtime_error if the class name is not registered.
 */
std::shared_ptr<JavaSerializable> ObjectFactory::createObject(const std::string &className)
{
    auto it = creators.find(className);
    if (it == creators.end())
    {
        throw std::runtime_error("Unknown class: " + className);
    }
    return it->second();
}

/* JavaDeserializer */
/**
 * @brief Deserializes a JavaSerializable object from a byte buffer.
 * 
 * This method verifies the parity of the received data and then deserializes the object.
 * It handles circular references by maintaining a map of deserialized objects.
 * 
 * @param data The byte buffer containing the serialized data.
 * 
 * @return A shared pointer to the deserialized object.
 * 
 * @throws std::runtime_error if deserialization fails or if the parity check fails.
 */
std::shared_ptr<JavaSerializable> JavaDeserializer::deserialize(const std::vector<uint8_t> &data)
{
    if (data.empty())
    {
        throw std::runtime_error("Empty data received for deserialization");
    }

    // Extract the data and parity bit
    std::vector<uint8_t> serializedData(data.begin(), data.end() - 1);
    uint8_t receivedParityBit = data.back();

    // Verify the parity
    if (!Parity::verifyEvenParity(serializedData, receivedParityBit))
    {
        throw std::runtime_error("Message parity check failed during deserialization");
    }

    deserializedObjects.clear();
    objectCounter = 0;
    ByteReader reader(data);
    return deserializeObject(reader);
}

/**
 * @brief Deserializes an object from a byte reader.
 * 
 * This method reads the object type, field metadata, and field values from the byte reader.
 * It handles circular references by maintaining a map of deserialized objects.
 * 
 * @param reader The byte reader to read the serialized data from.
 * 
 * @return A shared pointer to the deserialized object.
 * 
 * @throws std::runtime_error if deserialization fails or if the field metadata does not match.
 */
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
