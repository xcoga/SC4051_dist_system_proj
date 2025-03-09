// #include "ByteBuffer.hpp"
#include "../include/ByteBuffer.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cstring>
#include <stdexcept>
#include <functional>

ByteBuffer::ByteBuffer(size_t initialSize)
{
    buffer.reserve(initialSize);
}

void ByteBuffer::writeByte(uint8_t value)
{
    buffer.push_back(value);
}

// Write in big-endian format to match Java
void ByteBuffer::writeInt(int32_t value)
{
    writeByte((value >> 24) & 0xFF);
    writeByte((value >> 16) & 0xFF);
    writeByte((value >> 8) & 0xFF);
    writeByte(value & 0xFF);
}

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

void ByteBuffer::writeDouble(double value)
{
    uint64_t bits;
    memcpy(&bits, &value, sizeof(double));
    writeLong(bits);
}

// Match Java's UTF-8 string format
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

std::vector<uint8_t> ByteBuffer::getBuffer() const
{
    return buffer;
}
