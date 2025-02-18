#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cstring>
#include <stdexcept>
#include <functional>

class ByteBuffer
{
private:
    std::vector<uint8_t> buffer;

public:
    ByteBuffer(size_t initialSize = 1024)
    {
        buffer.reserve(initialSize);
    }

    void writeByte(uint8_t value)
    {
        buffer.push_back(value);
    }

    // Write in big-endian format to match Java
    void writeInt(int32_t value)
    {
        writeByte((value >> 24) & 0xFF);
        writeByte((value >> 16) & 0xFF);
        writeByte((value >> 8) & 0xFF);
        writeByte(value & 0xFF);
    }

    void writeLong(int64_t value)
    {
        writeByte((value >> 56) & 0xFF);
        writeByte((value >> 48) & 0xFF);
        writeByte((value >> 40) & 0xFF);
        writeByte((value >> 32) & 0xFF);
        writeByte((value >> 24) & 0xFF);
        writeByte((value >> 16) & 0xFF);
        writeByte((value >> 8) & 0xFF);
        writeByte(value & 0xFF);
    }

    void writeDouble(double value)
    {
        uint64_t bits;
        memcpy(&bits, &value, sizeof(double));
        writeLong(bits);
    }

    // Match Java's UTF-8 string format
    void writeString(const std::string &str)
    {
        // Write string length as 4 bytes
        writeInt(static_cast<int32_t>(str.length()));

        // Write string data
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

class ByteReader
{
private:
    const std::vector<uint8_t> &buffer;
    size_t position = 0;

public:
    ByteReader(const std::vector<uint8_t> &data) : buffer(data) {}

    std::vector<uint8_t> getBuffer() const
    {
        return std::vector<uint8_t>(buffer.begin() + position, buffer.end());
    }

    uint8_t readByte()
    {
        if (position >= buffer.size())
        {
            throw std::runtime_error("Buffer overflow");
        }
        return buffer[position++];
    }

    // Read in big-endian format to match Java
    int32_t readInt()
    {
        int32_t value = 0;
        value |= (static_cast<int32_t>(readByte()) << 24);
        value |= (static_cast<int32_t>(readByte()) << 16);
        value |= (static_cast<int32_t>(readByte()) << 8);
        value |= static_cast<int32_t>(readByte());
        return value;
    }

    int64_t readLong()
    {
        int64_t value = 0;
        value |= (static_cast<int64_t>(readByte()) << 56);
        value |= (static_cast<int64_t>(readByte()) << 48);
        value |= (static_cast<int64_t>(readByte()) << 40);
        value |= (static_cast<int64_t>(readByte()) << 32);
        value |= (static_cast<int64_t>(readByte()) << 24);
        value |= (static_cast<int64_t>(readByte()) << 16);
        value |= (static_cast<int64_t>(readByte()) << 8);
        value |= static_cast<int64_t>(readByte());
        return value;
    }

    double readDouble()
    {
        int64_t bits = readLong();
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
// Abstract base class for objects that can be serialized/deserialized
class JavaSerializable
{
public:
    virtual ~JavaSerializable() = default;
    virtual std::string getJavaClassName() const = 0;
    virtual void deserializeFields(ByteReader &reader) = 0;
    virtual std::vector<std::pair<std::string, std::string>> getFieldMetadata() const = 0;
    virtual void serializeFields(ByteBuffer &buffer) const = 0;
};

class JavaSerializer
{
private:
    static std::map<const void *, int> serializedObjects;
    static int objectCounter;

public:
    static std::vector<uint8_t> serialize(const JavaSerializable *obj)
    {
        serializedObjects.clear();
        objectCounter = 0;
        ByteBuffer buffer;
        serializeObject(obj, buffer);
        return buffer.getBuffer();
    }

private:
    static void serializeObject(const JavaSerializable *obj, ByteBuffer &buffer)
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

        // Write field metadata
        for (const auto &field : fields)
        {
            buffer.writeString(field.first);  // field name
            buffer.writeString(field.second); // field type
        }

        // Write field values
        obj->serializeFields(buffer);
    }
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

    static std::shared_ptr<JavaSerializable> createObject(const std::string &className)
    {
        auto it = creators.find(className);
        if (it == creators.end())
        {
            throw std::runtime_error("Unknown class: " + className);
        }
        return it->second();
    }
};

class JavaDeserializer
{
private:
    static std::map<int, std::shared_ptr<JavaSerializable>> deserializedObjects;
    static int objectCounter;

public:
    static std::shared_ptr<JavaSerializable> deserialize(const std::vector<uint8_t> &data)
    {
        deserializedObjects.clear();
        objectCounter = 0;
        ByteReader reader(data);
        return deserializeObject(reader);
    }

private:
    static std::shared_ptr<JavaSerializable> deserializeObject(ByteReader &reader)
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
};

// Initialize static members
std::map<int, std::shared_ptr<JavaSerializable>> JavaDeserializer::deserializedObjects;
int JavaDeserializer::objectCounter = 0;
std::map<std::string, std::function<std::shared_ptr<JavaSerializable>()>> ObjectFactory::creators;

// Initialize static members
std::map<const void *, int> JavaSerializer::serializedObjects;
int JavaSerializer::objectCounter = 0;

// Example implementation
class Person : public JavaSerializable
{
private:
    std::string name;
    int age;

public:
    Person() : age(0) {} // Default constructor needed for deserialization
    Person(const std::string &n, int a) : name(n), age(a) {}

    std::string getJavaClassName() const override
    {
        return "com.example.Person";
    }

    std::vector<std::pair<std::string, std::string>> getFieldMetadata() const override
    {
        return {
            {"name", "java.lang.String"},
            {"age", "int"}};
    }

    void serializeFields(ByteBuffer &buffer) const override
    {
        if (name.empty())
        {
            buffer.writeByte(0);
        }
        else
        {
            buffer.writeByte(1);
            buffer.writeString(name);
        }
        buffer.writeInt(age);
    }

    void deserializeFields(ByteReader &reader) override
    {
        // Read name
        uint8_t nameNullMarker = reader.readByte();
        if (nameNullMarker == 1)
        {
            name = reader.readString();
        }
        else
        {
            name = "";
        }

        // Read age
        age = reader.readInt();
    }

    // Getters for demonstration
    std::string getName() const { return name; }
    int getAge() const { return age; }
};

class Employee : public JavaSerializable
{
private:
    std::shared_ptr<Person> person;
    std::string department;
    double salary;
    std::string employeeId;

public:
    Employee() : salary(0.0) {} // Default constructor needed for deserialization

    Employee(const std::shared_ptr<Person> &p, const std::string &dept,
             double sal, const std::string &id)
        : person(p), department(dept), salary(sal), employeeId(id) {}

    std::string getJavaClassName() const override
    {
        return "com.example.Employee";
    }

    std::vector<std::pair<std::string, std::string>> getFieldMetadata() const override
    {
        return {
            {"person", "com.example.Person"},
            {"department", "java.lang.String"},
            {"salary", "double"},
            {"employeeId", "java.lang.String"}};
    }

    void serializeFields(ByteBuffer &buffer) const override
    {
        // Serialize person object
        if (person == nullptr)
        {
            buffer.writeByte(0); // null marker
        }
        else
        {
            buffer.writeByte(1); // non-null marker
            buffer.writeByte(0); // new object marker
            buffer.writeString(person->getJavaClassName());

            // Write field metadata for Person
            auto personFields = person->getFieldMetadata();
            buffer.writeInt(static_cast<int32_t>(personFields.size()));
            for (const auto &field : personFields)
            {
                buffer.writeString(field.first);
                buffer.writeString(field.second);
            }

            // Write Person fields
            person->serializeFields(buffer);
        }

        // Serialize department
        if (department.empty())
        {
            buffer.writeByte(0);
        }
        else
        {
            buffer.writeByte(1);
            buffer.writeString(department);
        }

        // Serialize salary
        buffer.writeDouble(salary);

        // Serialize employeeId
        if (employeeId.empty())
        {
            buffer.writeByte(0);
        }
        else
        {
            buffer.writeByte(1);
            buffer.writeString(employeeId);
        }
    }

    void deserializeFields(ByteReader &reader) override
    {
        // Deserialize person object
        uint8_t personNullMarker = reader.readByte();
        if (personNullMarker == 1)
        {
            uint8_t newObjectMarker = reader.readByte();
            if (newObjectMarker == 0)
            {
                std::string className = reader.readString();
                if (className != "com.example.Person")
                {
                    throw std::runtime_error("Invalid person class name: " + className);
                }

                // Read field metadata
                int32_t fieldCount = reader.readInt();
                for (int i = 0; i < fieldCount; i++)
                {
                    reader.readString(); // field name
                    reader.readString(); // field type
                }

                // Create and deserialize person
                person = std::make_shared<Person>();
                person->deserializeFields(reader);
            }
            else
            {
                throw std::runtime_error("Reference handling not implemented");
            }
        }
        else
        {
            person = nullptr;
        }

        // Deserialize department
        uint8_t deptNullMarker = reader.readByte();
        if (deptNullMarker == 1)
        {
            department = reader.readString();
        }
        else
        {
            department = "";
        }

        // Deserialize salary
        salary = reader.readDouble();

        // Deserialize employeeId
        uint8_t idNullMarker = reader.readByte();
        if (idNullMarker == 1)
        {
            employeeId = reader.readString();
        }
        else
        {
            employeeId = "";
        }
    }

    // Getters
    std::shared_ptr<Person> getPerson() const { return person; }
    std::string getDepartment() const { return department; }
    double getSalary() const { return salary; }
    std::string getEmployeeId() const { return employeeId; }
};

// Example usage:
void registerClasses()
{
    ObjectFactory::registerClass<Person>("com.example.Person");
    ObjectFactory::registerClass<Employee>("com.example.Employee");
}

int main()
{
    // Register classes
    registerClasses();

    // Create a person
    auto person = std::make_shared<Person>("John Doe", 30);

    // Create an employee
    Employee employee(person, "Engineering", 75000.0, "EMP001");

    // Serialize
    std::vector<uint8_t> serializedData = JavaSerializer::serialize(&employee);

    // Deserialize
    auto deserialized = std::dynamic_pointer_cast<Employee>(
        JavaDeserializer::deserialize(serializedData));

    if (deserialized)
    {
        std::cout << "Employee ID: " << deserialized->getEmployeeId() << std::endl;
        std::cout << "Name: " << deserialized->getPerson()->getName() << std::endl;
        std::cout << "Department: " << deserialized->getDepartment() << std::endl;
        std::cout << "Salary: " << deserialized->getSalary() << std::endl;
    }
}