package Server;

/**
 * Operation enum defines the types of requests that the server can process.
 * Each operation is associated with a unique integer code.
 */
public enum Operation {
  NONE(-1),    // No operation
  READ(0),     // Read operation
  WRITE(1),    // Write operation
  UPDATE(2),   // Update operation
  DELETE(3),   // Delete operation
  MONITOR(4),  // Monitor operation
  ECHO(5),     // Echo operation
  RATING(6);   // Rating operation

  private int op; // The integer code associated with the operation

  /**
   * Constructor to initialize the operation with its integer code.
   * 
   * @param op The integer code for the operation.
   */
  private Operation(final int op) {
    this.op = op;
  }

  /**
   * Gets the integer code associated with the operation.
   * 
   * @return The operation code.
   */
  public int getOpCode() {
    return op;
  }

  /**
   * Converts an integer code to its corresponding Operation enum.
   * 
   * @param opCode The integer code to convert.
   * @return The corresponding Operation enum, or NONE if no match is found.
   */
  public static Operation fromInt(int opCode) {
    for (Operation op : Operation.values()) {
      if (op.getOpCode() == opCode) {
        return op;
      }
    }
    return Operation.NONE; // Default to NONE if no match is found
  }
}
