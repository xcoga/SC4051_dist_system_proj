package Server;

public class RequestMessage {
  private int requestType;
  private int requestID;
  private String data;

  public RequestMessage() {
  } // Required for deserialization

  public RequestMessage(int requestType, int requestID, String data) {
    this.requestType = requestType;
    this.requestID = requestID;
    this.data = data;
  }
  // public RequestMessage(int requestID, String data) {
  // this.requestID = requestID;
  // this.data = data;
  // }

  public int getRequestType() {
    return this.requestType;
  }

  public Operation getOperation() {
    return Operation.fromInt(this.requestType);
  }

  public int getRequestID() {
    return this.requestID;
  }

  public String getData() {
    return this.data;
  }
}
