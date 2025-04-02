#ifndef BYTEREADER_HPP
#define BYTEREADER_HPP

#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

/**
 * @class ByteReader
 * @brief A utility class for reading data from a byte buffer in big-endian format.
 * 
 * The ByteReader class provides methods to read various data types (e.g., integers,
 * strings, doubles) from a byte buffer. It ensures compatibility with Java's serialization
 * format by reading data in a big-endian order.
 */
class ByteReader
{
private:
    const std::vector<uint8_t> &buffer; ///< The internal buffer to read data from.
    size_t position = 0; ///< The current read position in the buffer.

public:
    /**
     * @brief Constructs a ByteReader with the given buffer.
     * @param data The byte buffer to read from.
     */
    ByteReader(const std::vector<uint8_t> &data);

    /**
     * @brief Retrieves the remaining unread portion of the buffer.
     * @return A vector containing the unread bytes.
     */
    std::vector<uint8_t> getBuffer() const;

    /**
     * @brief Reads a single byte from the buffer.
     * @return Teh byte value read from the buffer.
     * @throws std::runtime_error if the buffer is exhausted.
     */
    uint8_t readByte();

    /**
     * @brief Reads a 32-bit integer from the buffer in big-endian format.
     * @return The 32-bit integer value read from the buffer.
     * @throws std::runtime_error if the buffer is exhausted.
     */
    int32_t readInt();

    /**
     * @brief Reads a 64-bit integer from the buffer in big-endian format.
     * @return The 64-bit integer value read from the buffer.
     * @throws std::runtime_error if the buffer is exhausted.
     */
    int64_t readLong();

    /**
     * @brief Reads a double-precision floating-point value from the buffer.
     * @return The double value read from the buffer.
     * @throws std::runtime_error if the buffer is exhausted.
     */
    double readDouble();

    /**
     * @brief Reads a string from the buffer.
     * @return The string value read from the buffer.
     * @throws std::runtime_error if the buffer is exhausted.
     */
    std::string readString();
};

#endif // BYTEREADER_HPP
