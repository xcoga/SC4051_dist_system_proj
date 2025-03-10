package Server.utils;

public class Parity {
    /**
     * Calculate even parity bit for an array of bytes
     * 
     * @param data The byte array to calculate parity for
     * @return 1 if total number of 1 bits is odd, 0 if total is already even
     */
    public static byte calculateEvenParityBit(byte[] data) {
        byte parityBit = 0;

        // XOR all bytes together to get odd/even count of 1 bits
        for (byte b : data) {
            // Process each bit in the byte
            byte temp = b;
            for (int i = 0; i < 8; i++) {
                // XOR the lowest bit with our running parity
                parityBit ^= (temp & 1);
                temp >>= 1;
            }
        }

        // For even parity, if parityBit is 1, we need to add a 1 to make total 1s even
        return parityBit;
    }

    /**
     * Verify even parity for data and provided parity bit
     * 
     * @param data      The byte array to verify
     * @param parityBit The parity bit to check against
     * @return true if parity is valid, false otherwise
     */
    public static boolean verifyEvenParity(byte[] data, byte parityBit) {
        // Calculate what the parity should be
        byte calculatedParity = calculateEvenParityBit(data);

        // For even parity, the calculated parity should match the provided parity bit
        return calculatedParity == parityBit;
    }
}
