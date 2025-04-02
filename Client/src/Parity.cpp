#include "Parity.hpp"

/**
 * @brief Calculates the even parity bit for a given set of data.
 * 
 * The even parity bit is calculated such that the totla number of 1s in the data (including the parity bit) is even.
 * 
 * @param data A vector of bytes for which the parity bit is to be calculated.
 * 
 * @return The calculated even parity bit (0 or 1).
 */
uint8_t Parity::calculateEvenParityBit(const std::vector<uint8_t> &data)
{
    uint8_t parityBit = 0;
    for (const auto &byte : data)
    {
        uint8_t temp = byte;
        while (temp)
        {
            parityBit ^= (temp & 1); // XOR the least significant bit
            temp >>= 1; // Shift right to process the next bit
        }
    }
    return parityBit;
}

/**
 * @brief Verifies the even parity of a given set of data and a parity bit.
 * 
 * This function caluclates the evan parity bit for the given data and compares it with the provided parity bit to determine if the parity is correct.
 * 
 * @param data A vector of bytes for which the parity is to be verified.
 * @param parityBit The provided parity bit to verify against.
 * 
 * @return True if the calculated parity matches the provided parity bit, false otherwise.
 */
bool Parity::verifyEvenParity(const std::vector<uint8_t> &data, uint8_t parityBit)
{
    uint8_t calculatedParity = calculateEvenParityBit(data);
    return calculatedParity == parityBit;
}
