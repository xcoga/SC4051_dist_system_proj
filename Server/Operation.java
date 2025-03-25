package Server;

/**
 * Operation enum is used to define the types of requests that server can
 * process.
 */
public enum Operation {
  NONE(-1),
  READ(0), 
  WRITE(1), 
  UPDATE(2), 
  DELETE(3), 
  MONITOR(4),
  ECHO(5),
  RATING(6);

  private int op;

  private Operation(final int op) {
    this.op = op;
  }

  public int getOpCode() {
    return op;
  }

  public static Operation fromInt(int opCode) {
    for (Operation op : Operation.values()) {
      if (op.getOpCode() == opCode) {
        return op;
      }
    }
    return Operation.NONE;
  }
}
