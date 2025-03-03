package Server.utils;

public class ByteBuffer {
    private byte[] buffer;
    private int position;

    public ByteBuffer(int capacity) {
        buffer = new byte[capacity];
        position = 0;
    }

    public void writeByte(byte value) {
        buffer[position++] = value;
    }

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

    public void writeDouble(double value) {
        long bits = Double.doubleToLongBits(value);
        writeLong(bits);
    }

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

    public void writeString(String value) {
        byte[] bytes = value.getBytes();
        writeInt(bytes.length);
        for (byte b : bytes) {
            writeByte(b);
        }
    }

    public byte[] getBuffer() {
        byte[] result = new byte[position];
        System.arraycopy(buffer, 0, result, 0, position);
        return result;
    }
}
