#ifndef PARITY_HPP
#define PARITY_HPP

#include <cstdint>
#include <vector>

/**
 * @namespace Parity
 * @brief Provides utility functions for calculating and verifying parity bits.
 *
 * The `Parity` namespace contains functions to calculate even parity bits for a given set of data and to verify the correctness of a parity bit.
 * These functions are useful for error detection in data transmission.
 */
namespace Parity
{
    /**
     * @brief Calcualtes the even parity bit for a given set of data.
     * @param data A vector of bytes for which the parity bit is to be calculated.
     * @return The calculated even parity bit (0 or 1).
     */
    uint8_t calculateEvenParityBit(const std::vector<uint8_t> &data);

    /**
     * @brief Verifies the even parity of a given set of data and a parity bit.
     * @param data A vector of bytes for which the parity is to be verified.
     * @param parityBit The provided parity bit to verify against.
     * @return True if the calculated parity matches the provided parity bit, false otherwise.
     */
    bool verifyEvenParity(const std::vector<uint8_t> &data, uint8_t parityBit);
}

#endif // PARITY_HPP
