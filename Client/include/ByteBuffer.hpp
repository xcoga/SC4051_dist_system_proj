#ifndef BYTEBUFFER_HPP
#define BYTEBUFFER_HPP

#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @class ByteBuffer
 * @brief A utility class for writing data into a byte buffer in big-endian format.
 * 
 * The ByteBuffer class provides methods to write various data types (e.g., integers,
 * strings, doubles) into a byte buffer. The data is written in big-endian format to
 * ensure compatibility with Java's serialization format.
 */
class ByteBuffer
{
private:
    std::vector<uint8_t> buffer; ///< The internal buffer to store the serialized data.

public:
    /**
     * @brief Constructs a ByteBuffer with an optional initial size.
     * @param initialSize The initial size of the buffer (default is 1024 bytes).
     */
    ByteBuffer(size_t initialSize = 1024);

    /**
     * @brief Writes a single byte to the buffer.
     * @param value The byte value to write.
     */
    void writeByte(uint8_t value);

    /**
     * @brief Writes a 32-bit integer to the buffer in big-endian format.
     * @param value The 32-bit integer to write.
     */
    void writeInt(int32_t value);

    /**
     * @brief Wrties a 64-bit integer to the buffer in big-endian foramt.
     * @param value The 64-bit integer to write.
     */
    void writeLong(int64_t value);

    /**
     * @brief Writes a double-precision floating-point value to the buffer.
     * @param value The double value to write.
     */
    void writeDouble(double value);

    /**
     * @brief Writes a UTF-8 encoded string to the buffer.
     * @param str The string to write.
     */
    void writeString(const std::string &str);

    /**
     * @brief Retrieves the internal buffer as a vector of bytes.
     * @return A vector containing the serialized data.
     */
    std::vector<uint8_t> getBuffer() const;
};

#endif // BYTEBUFFER_HPP
