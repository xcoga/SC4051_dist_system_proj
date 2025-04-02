#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

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

/**
 * @class JavaSerializable
 * @brief An abstract base class for objects taht can be serialized and deserialized.
 * 
 * The JavaSerializable class defines the interface for objects that can be serialized into and deserialized from a byte buffer.
 * It ensures compatibility with Java's serialization format.
 */
class JavaSerializable
{
public:
    virtual ~JavaSerializable() = default;

    /**
     * @brief Gets the Java class name for serialization.
     * @return The Java class name as a string.
     */
    virtual std::string getJavaClassName() const = 0;

    /**
     * @brief Gets metadata about the fields in the object.
     * @return A vector of pairs containing field names and their types.
     */
    virtual std::vector<std::pair<std::string, std::string>> getFieldMetadata() const = 0;

    /**
     * @brief Serializes the field values into a byte buffer.
     * @param buffer The byte buffer to write the serialized data into.
     */
    virtual void serializeFieldValues(ByteBuffer &buffer) const = 0;

    /**
     * @brief Deserializes the field values from a byte reader.
     * @param reader The byte reader to read the serialized data from.
     */
    virtual void deserializeFields(ByteReader &reader) = 0;

    /**
     * @brief A map of class names to factory functions for creating objects.
     */
    static std::map<std::string, std::function<std::shared_ptr<JavaSerializable>()>> creators;
};

/**
 * @class JavaSerializer
 * @brief A utility class for serializing JavaSerializable objects into byte buffers.
 *
 * The JavaSerializer class provides methods to serialize objects into a byte buffer in a format compatible with Java's serialization mechanism.
 */
class JavaSerializer
{
private:
    static std::map<const void *, int> serializedObjects; ///< Track serialized objects to handle circular references.
    static int objectCounter; ///< Counter for assigning unique IDs to serialized objects.

public:
    /**
     * @brief Serializes a JavaSerializable object into a byte buffer.
     * @param obj The object to serialize.
     * @return A vector of bytes containing the serialized data.
     */
    static std::vector<uint8_t> serialize(const JavaSerializable *obj);

private:
    /**
     * @brief Serializes the object and writes it into a byte buffer.
     * @param obj The object to serialize.
     * @param buffer The byte buffer to write the serialized data into.
     */
    static void serializeObject(const JavaSerializable *obj, ByteBuffer &buffer);
};

/**
 * @class ObjectFactory
 * @brief A factory class for creating JavaSerializable objects.
 *
 * The ObjectFactory class provides methods to register and create objects based on their class names.
 */
class ObjectFactory
{
public:
    /**
     * @brief Registers a class with the factory.
     * @tparam T The class type to register.
     * @param className The name of the class to register.
     */
    template <typename T>
    static void registerClass(const std::string &className)
    {
        creators[className] = []()
        { return std::make_shared<T>(); };
    }

    /**
     * @brief Creates an object based on its class name.
     * @param className The name of the class.
     * @return A shared pointer to the created object.
     * @throws std::runtime_error if the class name is not registered.
     */
    static std::shared_ptr<JavaSerializable> createObject(const std::string &className);

    /**
     * @brief A map of class names to factory functions for creating objects.
     */
    static std::map<std::string, std::function<std::shared_ptr<JavaSerializable>()>> creators;
};

/**
 * @class JavaDeserializer
 * @brief A utility class for deserializing JavaSerializable objects from byte buffers.
 * 
 * The JavaDeserializer class provides methods to deserialize objects from a byte buffer in a format compatible with Java's serialization mechanism.
 */
class JavaDeserializer
{
private:
    static std::map<int, std::shared_ptr<JavaSerializable>> deserializedObjects; ///< Track deserialized objects to handle circular references.
    static int objectCounter; ///< Counter for assigning unique IDs to deserialized objects.

public:
    /**
     * @brief Deserializes a JavaSerializable object from a byte buffer.
     * @param data The byte buffer containing the serialized data.
     * @return A shared pointer to the deserialized object.
     * @throws std::runtime_error if deserialization fails.
     */
    static std::shared_ptr<JavaSerializable> deserialize(const std::vector<uint8_t> &data);

private:
    /**
     * @brief Deserializes an object from a byte reader.
     * @param reader The byte reader to read the serialized data from.
     * @return A shared pointer to the deserialized object.
     */
    static std::shared_ptr<JavaSerializable> deserializeObject(ByteReader &reader);
};

#endif // SERIALIZER_HPP
