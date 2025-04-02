package Server.utils;

/**
 * ByteBuffer class provides methods to write primitive types and strings to a
 * byte array.
 * It is used during serialization to construct a serialized byte array.
 */
public class ByteBuffer {
    private byte[] buffer; // The byte array to write to
    private int position; // Current position in the byte array

    /**
     * Constructor to initialize the ByteBuffer with a specified capacity.
     * 
     * @param capacity The initial capacity of the buffer.
     */
    public ByteBuffer(int capacity) {
        this.buffer = new byte[capacity];
        this.position = 0;
    }

    /**
     * Write a single byte to the buffer.
     * 
     * @param value The byte to write.
     */
    public void writeByte(byte value) {
        buffer[position++] = value;
    }

    /**
     * Write an integer (4 bytes) to the buffer.
     * 
     * @param value The integer to write.
     */
    public void writeInt(int value) {
        /*
         * This function stores Integers into the buffer.
         * 
         * Right shift operations are necessary because:
         * An int in Java is 4 bytes (32 bits).
         * Since a byte can only hold 8 bits, we need to extract each byte from the int
         * and store it separately in the buffer.
         * 
         * Since the Integer's MSB is at the LSB of the buffer,
         * func writeInt is in Big Endian format.
         */

        writeByte((byte) ((value >> 24) & 0xFF));
        writeByte((byte) ((value >> 16) & 0xFF));
        writeByte((byte) ((value >> 8) & 0xFF));
        writeByte((byte) (value & 0xFF));
    }

    /**
     * Write a double (8 bytes) to the buffer.
     * 
     * @param value The double to write.
     */
    public void writeDouble(double value) {
        long bits = Double.doubleToLongBits(value);
        writeLong(bits);
    }

    /**
     * Write a long (8 bytes) to the buffer.
     * 
     * @param value The long to write.
     */
    public void writeLong(long value) {
        writeByte((byte) ((value >> 56) & 0xFF));
        writeByte((byte) ((value >> 48) & 0xFF));
        writeByte((byte) ((value >> 40) & 0xFF));
        writeByte((byte) ((value >> 32) & 0xFF));
        writeByte((byte) ((value >> 24) & 0xFF));
        writeByte((byte) ((value >> 16) & 0xFF));
        writeByte((byte) ((value >> 8) & 0xFF));
        writeByte((byte) (value & 0xFF));
    }

    /**
     * Write a string to the buffer.
     * 
     * @param value The string to write.
     */
    public void writeString(String value) {
        byte[] bytes = value.getBytes();
        writeInt(bytes.length);
        for (byte b : bytes) {
            writeByte(b);
        }
    }

    /**
     * Get the contents of the buffer as a byte array.
     * 
     * @return A byte array containing the written data.
     */
    public byte[] getBuffer() {
        byte[] result = new byte[position];
        System.arraycopy(buffer, 0, result, 0, position);
        return result;
    }
}
