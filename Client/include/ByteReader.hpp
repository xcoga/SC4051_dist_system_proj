#ifndef BYTEREADER_HPP
#define BYTEREADER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstring>

class ByteReader
{
private:
    const std::vector<uint8_t> &buffer;
    size_t position = 0;

public:
    ByteReader(const std::vector<uint8_t> &data);

    std::vector<uint8_t> getBuffer() const;

    uint8_t readByte();

    int32_t readInt();
    int64_t readLong();
    double readDouble();
    std::string readString();
};

#endif // BYTEREADER_HPP