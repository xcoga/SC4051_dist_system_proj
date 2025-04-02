package Server.utils;

/**
 * ByteReader class provides methods to read primitive types and strings from a
 * byte array.
 * It is used during deserialization to extract data from a serialized byte
 * array.
 */
public class ByteReader {
    private byte[] buffer; // The byte array to read from
    private int position; // Current position in the byte array

    /**
     * Constructor to initialize the ByteReader with a byte array.
     * 
     * @param data The byte array to read from.
     */
    public ByteReader(byte[] data) {
        this.buffer = data;
        this.position = 0;
    }

    /**
     * Read a single byte from the buffer.
     * 
     * @return The byte read.
     */
    public byte readByte() {
        return buffer[position++];
    }

    /**
     * Read an integer (4 bytes) from the buffer.
     * 
     * @return The integer read.
     */
    public int readInt() {
        /*
         * This function retrieves Integers from the buffer.
         * 
         * Left shift operations are necessary because:
         * The data is stored in Big Endian format. (defined in ByteBuffer.java)
         * Left shift will bring the MSB back to the correct position
         * An int in Java is 4 bytes (32 bits).
         * Since a byte can only hold 8 bits, we need to extract each byte from the
         * buffer
         */
        return ((readByte() & 0xFF) << 24) |
                ((readByte() & 0xFF) << 16) |
                ((readByte() & 0xFF) << 8) |
                (readByte() & 0xFF);
    }

    /**
     * Read a long (8 bytes) from the buffer.
     * 
     * @return The long read.
     */
    public long readLong() {
        return ((long) (readByte() & 0xFF) << 56) |
                ((long) (readByte() & 0xFF) << 48) |
                ((long) (readByte() & 0xFF) << 40) |
                ((long) (readByte() & 0xFF) << 32) |
                ((long) (readByte() & 0xFF) << 24) |
                ((long) (readByte() & 0xFF) << 16) |
                ((long) (readByte() & 0xFF) << 8) |
                (readByte() & 0xFF);
    }

    /**
     * Read a double (8 bytes) from the buffer.
     * 
     * @return The double read.
     */
    public double readDouble() {
        long bits = readLong();
        return Double.longBitsToDouble(bits);
    }

    /**
     * Read a string from the buffer.
     * 
     * @return The string read.
     */
    public String readString() {
        int length = readInt();
        byte[] bytes = new byte[length];
        for (int i = 0; i < length; i++) {
            bytes[i] = readByte();
        }
        return new String(bytes);
    }
}
