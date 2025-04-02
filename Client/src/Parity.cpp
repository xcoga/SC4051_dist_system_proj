#include "Parity.hpp"

namespace Parity
{
    uint8_t calculateEvenParityBit(const std::vector<uint8_t> &data)
    {
        uint8_t parityBit = 0;
        for (const auto &byte : data)
        {
            uint8_t temp = byte;
            while (temp)
            {
                parityBit ^= (temp & 1);
                temp >>= 1;
            }
        }
        return parityBit;
    }

    bool verifyEvenParity(const std::vector<uint8_t> &data, uint8_t parityBit)
    {
        uint8_t calculatedParity = calculateEvenParityBit(data);
        return calculatedParity == parityBit;
    }
}
