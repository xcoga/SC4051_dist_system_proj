#include <vector>
#include <string>
#include <map>
#include <memory>
#include <typeinfo>
#include <cstring>
#include <stdexcept>

// ByteBuffer class for writing data
class ByteBuffer
{
private:
    std::vector<uint8_t> buffer;
    size_t position = 0;

public:
    ByteBuffer(size_t initialSize = 1024)
    {
        buffer.reserve(initialSize);
    }

    void writeByte(uint8_t value)
    {
        buffer.push_back(value);
    }

    void writeInt(int32_t value)
    {
        for (int i = 0; i < 4; i++)
        {
            writeByte((value >> (i * 8)) & 0xFF);
        }
    }

    void writeLong(int64_t value)
    {
        for (int i = 0; i < 8; i++)
        {
            writeByte((value >> (i * 8)) & 0xFF);
        }
    }

    void writeDouble(double value)
    {
        uint64_t bits;
        memcpy(&bits, &value, sizeof(double));
        for (int i = 0; i < 8; i++)
        {
            writeByte((bits >> (i * 8)) & 0xFF);
        }
    }

    void writeString(const std::string &str)
    {
        writeInt(static_cast<int32_t>(str.length()));
        for (char c : str)
        {
            writeByte(static_cast<uint8_t>(c));
        }
    }

    std::vector<uint8_t> getBuffer() const
    {
        return buffer;
    }
};

// ByteReader class for reading data
class ByteReader
{
private:
    const std::vector<uint8_t> &buffer;
    size_t position = 0;

public:
    ByteReader(const std::vector<uint8_t> &data) : buffer(data) {}

    uint8_t readByte()
    {
        if (position >= buffer.size())
        {
            throw std::out_of_range("Buffer overflow");
        }
        return buffer[position++];
    }

    int32_t readInt()
    {
        int32_t value = 0;
        for (int i = 0; i < 4; i++)
        {
            value |= (static_cast<int32_t>(readByte()) << (i * 8));
        }
        return value;
    }

    int64_t readLong()
    {
        int64_t value = 0;
        for (int i = 0; i < 8; i++)
        {
            value |= (static_cast<int64_t>(readByte()) << (i * 8));
        }
        return value;
    }

    double readDouble()
    {
        uint64_t bits = 0;
        for (int i = 0; i < 8; i++)
        {
            bits |= (static_cast<uint64_t>(readByte()) << (i * 8));
        }
        double value;
        memcpy(&value, &bits, sizeof(double));
        return value;
    }

    std::string readString()
    {
        int32_t length = readInt();
        std::string str;
        str.reserve(length);
        for (int i = 0; i < length; i++)
        {
            str += static_cast<char>(readByte());
        }
        return str;
    }
};

// Base class for serializable objects
class Serializable
{
public:
    virtual ~Serializable() = default;
    virtual std::string getClassName() const = 0;
    virtual void serialize(ByteBuffer &buffer) const = 0;
    virtual void deserialize(ByteReader &reader) = 0;
};

// Serializer class
class Serializer
{
private:
    static std::map<const void *, int> serializedObjects;
    static std::map<int, std::shared_ptr<Serializable>> deserializedObjects;
    static int objectCounter;

    static void serializeObject(const Serializable *obj, ByteBuffer &buffer)
    {
        if (obj == nullptr)
        {
            buffer.writeByte(0); // null marker
            return;
        }

        buffer.writeByte(1); // non-null marker

        // Check if object was already serialized
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
        buffer.writeString(obj->getClassName());
        obj->serialize(buffer);
    }

    static std::shared_ptr<Serializable> deserializeObject(ByteReader &reader)
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
            return deserializedObjects[objectId];
        }

        std::string className = reader.readString();
        // Here you would need to implement a factory method to create
        // the appropriate object based on className
        auto obj = createObject(className);

        if (!obj)
        {
            throw std::runtime_error("Unknown class: " + className);
        }

        deserializedObjects[objectCounter++] = obj;
        obj->deserialize(reader);
        return obj;
    }

    static std::shared_ptr<Serializable> createObject(const std::string &className)
    {
        // Implement object creation based on class name
        // This would need to be customized for your specific classes
        throw std::runtime_error("Object creation not implemented");
    }

public:
    static std::vector<uint8_t> serialize(const Serializable *obj)
    {
        serializedObjects.clear();
        objectCounter = 0;
        ByteBuffer buffer;
        serializeObject(obj, buffer);
        return buffer.getBuffer();
    }

    static std::shared_ptr<Serializable> deserialize(const std::vector<uint8_t> &data)
    {
        deserializedObjects.clear();
        objectCounter = 0;
        ByteReader reader(data);
        return deserializeObject(reader);
    }
};

// Initialize static members
std::map<const void *, int> Serializer::serializedObjects;
std::map<int, std::shared_ptr<Serializable>> Serializer::deserializedObjects;
int Serializer::objectCounter = 0;