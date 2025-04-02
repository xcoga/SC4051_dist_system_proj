#include "ByteReader.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief Constructs a ByteReader with the given buffer.
 * 
 * @param data The byte buffer to read from.
 */
ByteReader::ByteReader(const std::vector<uint8_t> &data) : buffer(data) {}

/**
 * @brief Retrieves the remaining unread portion of the buffer.
 * 
 * @return A vector containing the unread bytes.
 */
std::vector<uint8_t> ByteReader::getBuffer() const
{
    return std::vector<uint8_t>(buffer.begin() + position, buffer.end());
}

/**
 * @brief Reads a single byte from the buffer.
 * 
 * @return The byte value read from the buffer.
 * @throws std::runtime_error if the buffer is exhausted.
 */
uint8_t ByteReader::readByte()
{
    if (position >= buffer.size())
    {
        throw std::runtime_error("Buffer overflow");
    }
    return buffer[position++];
}

/**
 * @brief Reads a 32-bit integer from the buffer in big-endian format.
 * 
 * The integer is reconstructed from 4 bytes in the order of most significant byte
 * (MSB) to least significant byte (LSB).
 * 
 * @return The 32-bit integer value read from the buffer.
 * @throws std::runtime_error if the buffer is exhausted.
 */
int32_t ByteReader::readInt()
{
    int32_t value = 0;
    value |= (static_cast<int32_t>(readByte()) << 24);
    value |= (static_cast<int32_t>(readByte()) << 16);
    value |= (static_cast<int32_t>(readByte()) << 8);
    value |= static_cast<int32_t>(readByte());
    return value;
}

/**
 * @brief Reads a 64-bit integer from the buffer in big-endian format.
 * 
 * The integer is reconstructed from 8 bytes in the order of most significant byte
 * (MSB) to least significant byte (LSB).
 * 
 * @return The 64-bit integer value read from the buffer.
 * @throws std::runtime_error if the buffer is exhausted.
 */
int64_t ByteReader::readLong()
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

/**
 * @brief Reads a double-precision floating-point value from the buffer.
 * 
 * The binary representation of the double is copied into a 64-bit variable, ensuring its preservation.
 * Then, it is converted to its IEEE 754 representation.
 * 
 * @return The double value read from the buffer.
 * @throws std::runtime_error if the buffer is exhausted.
 */
double ByteReader::readDouble()
{
    int64_t bits = readLong();
    double value;
    memcpy(&value, &bits, sizeof(double));
    return value;
}

/**
 * @brief Reads a string from the buffer.
 * 
 * The string is read in the following format:
 * - The length of the string (4 bytes, big-endian).
 * - The string data as UTF-8 bytes.
 * 
 * @return The string value read from the buffer.
 * @throws std::runtime_error if the buffer is exhausted.
 */
std::string ByteReader::readString()
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
