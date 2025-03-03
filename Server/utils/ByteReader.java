package Server.utils;

public class ByteReader {
    private byte[] buffer;
    private int position;

    public ByteReader(byte[] data) {
        this.buffer = data;
        this.position = 0;
    }

    public byte readByte() {
        return buffer[position++];
    }

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

    public double readDouble() {
        long bits = readLong();
        return Double.longBitsToDouble(bits);
    }

    public String readString() {
        int length = readInt();
        byte[] bytes = new byte[length];
        for (int i = 0; i < length; i++) {
            bytes[i] = readByte();
        }
        return new String(bytes);
    }
}
