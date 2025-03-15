#include "ByteReader.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cstring>
#include <stdexcept>
#include <functional>

ByteReader::ByteReader(const std::vector<uint8_t> &data) : buffer(data) {}

std::vector<uint8_t> ByteReader::getBuffer() const
{
    return std::vector<uint8_t>(buffer.begin() + position, buffer.end());
}

uint8_t ByteReader::readByte()
{
    if (position >= buffer.size())
    {
        throw std::runtime_error("Buffer overflow");
    }
    return buffer[position++];
}

// Read in big-endian format to match Java
int32_t ByteReader::readInt()
{
    int32_t value = 0;
    value |= (static_cast<int32_t>(readByte()) << 24);
    value |= (static_cast<int32_t>(readByte()) << 16);
    value |= (static_cast<int32_t>(readByte()) << 8);
    value |= static_cast<int32_t>(readByte());
    return value;
}

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

double ByteReader::readDouble()
{
    int64_t bits = readLong();
    double value;
    memcpy(&value, &bits, sizeof(double));
    return value;
}

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