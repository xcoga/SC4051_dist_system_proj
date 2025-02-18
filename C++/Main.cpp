#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cstring>
#include <stdexcept>
#include <functional>
using namespace std;

#include "utils/Serializer.cpp"

/*In serialisation, only function field + fieldtypes are serialised. Methods arent serialised.
So, we can write serilisation methods within the class itself, without any issues.
 */
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
        return "SC4051_dist_system_proj.Person";
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
        return "SC4051_dist_system_proj.Employee";
    }

    std::vector<std::pair<std::string, std::string>> getFieldMetadata() const override
    {
        return {
            {"person", "SC4051_dist_system_proj.Person"},
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
                if (className != "SC4051_dist_system_proj.Person")
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

class RequestMessage : public JavaSerializable
{
private:
    int requestID;
    string data;

public:
    RequestMessage() : requestID(0) {} // Default constructor needed for deserialization
    RequestMessage(int id, const string &d) : requestID(id), data(d) {}

    string getJavaClassName() const override
    {
        return "SC4051_dist_system_proj.RequestMessage";
    }

    vector<pair<string, string>> getFieldMetadata() const override
    {
        return {
            {"requestID", "int"},
            {"data", "java.lang.String"}};
    }
    void serializeFields(ByteBuffer &buffer) const override
    {
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

    void deserializeFields(ByteReader &reader) override
    {
        requestID = reader.readInt();

        uint8_t dataNullMarker = reader.readByte();
        if (dataNullMarker == 1)
        {
            data = reader.readString();
        }
        else
        {
            data = "";
        }
    }

    // Getters
    int getRequestID() const { return requestID; }
    string getData() const { return data; }
};

void registerClasses()
{
    // Need to match package name in Java
    ObjectFactory::registerClass<Person>("SC4051_dist_system_proj.Person");
    ObjectFactory::registerClass<Employee>("SC4051_dist_system_proj.Employee");
    ObjectFactory::registerClass<RequestMessage>("SC4051_dist_system_proj.RequestMessage");
}

int main()
{
    // Register classes
    registerClasses();

    // Create a person
    auto person = std::make_shared<Person>("John Doe", 30);
    auto requestmessage = RequestMessage(1, "Hello");

    // Create an employee
    Employee employee(person, "Engineering", 75000.0, "EMP001");

    // Serialize
    std::vector<uint8_t> serializedData = JavaSerializer::serialize(&employee);
    std::vector<uint8_t> serializedData2 = JavaSerializer::serialize(&requestmessage);

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

    auto deserialized2 = std::dynamic_pointer_cast<RequestMessage>(
        JavaDeserializer::deserialize(serializedData2));

    if (deserialized2)
    {
        std::cout << "Request ID: " << deserialized2->getRequestID() << std::endl;
        std::cout << "Data: " << deserialized2->getData() << std::endl;
    }
}