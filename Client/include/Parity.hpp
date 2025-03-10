#ifndef PARITY_HPP
#define PARITY_HPP

#include <cstdint>
#include <vector>

namespace Parity
{
    uint8_t calculateEvenParityBit(const std::vector<uint8_t> &data);
    bool verifyEvenParity(const std::vector<uint8_t> &data, uint8_t parityBit);
}

#endif // PARITY_UTILS_HPP
