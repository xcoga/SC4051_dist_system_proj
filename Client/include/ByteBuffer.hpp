#ifndef BYTEBUFFER_HPP
#define BYTEBUFFER_HPP

#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class ByteBuffer
{
private:
    std::vector<uint8_t> buffer;

public:
    ByteBuffer(size_t initialSize = 1024);

    void writeByte(uint8_t value);

    // Write in big-endian format to match Java
    void writeInt(int32_t value);
    void writeLong(int64_t value);
    void writeDouble(double value);
    void writeString(const std::string &str);

    std::vector<uint8_t> getBuffer() const;
};

#endif // BYTEBUFFER_HPP
