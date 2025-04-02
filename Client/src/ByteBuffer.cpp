#include "ByteBuffer.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief Constructs a ByteBuffer with the specified initial size.
 * 
 * The buffer is reserved with the given size to minimize reallocations.
 * 
 * @param initialSize The initial size of the buffer.
 */
ByteBuffer::ByteBuffer(size_t initialSize)
{
    buffer.reserve(initialSize);
}

/**
 * @brief Writes a single byte to the buffer.
 * 
 * @param value The byte value to write.
 */
void ByteBuffer::writeByte(uint8_t value)
{
    buffer.push_back(value);
}

/**
 * @brief Writes a 32-bit integer to the buffer in big-endian format.
 * 
 * The integer is split into 4 bytes and written in the order of most significant byte
 * (MSB) to least significant byte (LSB).
 * 
 * @param value The 32-bit integer to write.
 */
void ByteBuffer::writeInt(int32_t value)
{
    writeByte((value >> 24) & 0xFF);
    writeByte((value >> 16) & 0xFF);
    writeByte((value >> 8) & 0xFF);
    writeByte(value & 0xFF);
}

/**
 * @brief Writes a 64-bit integer to the buffer in big-endian format.
 * 
 * The integer is split into 8 bytes and written in the order of most significant byte
 * (MSB) to least significant byte (LSB).
 * 
 * @param value The 64-bit integer to write.
 */
void ByteBuffer::writeLong(int64_t value)
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

/**
 * @brief Writes a double-precision floating-point value to the buffer.
 * 
 * The double is in IEE 754 represendtation, making it 64 bits long.
 * Copying it directly into 64-bit variable ensures that the binary representation is preserved exactly.
 * 
 * @param value The double value to write.
 */
void ByteBuffer::writeDouble(double value)
{
    uint64_t bits;
    memcpy(&bits, &value, sizeof(double));
    writeLong(bits);
}

/**
 * @brief Writes a UTF-8 encoded string to the buffer.
 * 
 * The string is written in the following format:
 * - The length of the string (4 bytes, big-endian).
 * - The string data as UTF-8 bytes.
 * 
 * @param str The string to write.
 */
void ByteBuffer::writeString(const std::string &str)
{
    // Write string length as 4 bytes
    writeInt(static_cast<int32_t>(str.length()));

    // Write string data
    for (char c : str)
    {
        writeByte(static_cast<uint8_t>(c));
    }
}

/**
 * @brief Retrieves the internal buffer as a vector of bytes.
 * 
 * @return A vector containing the serialized data.
 */
std::vector<uint8_t> ByteBuffer::getBuffer() const
{
    return buffer;
}
