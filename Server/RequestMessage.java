package Server;

/**
 * RequestMessage class represents a message sent between the client and server.
 * It contains the type of request, a unique request ID, and any associated
 * data.
 */
public class RequestMessage {
  private int requestType; // Type of the request (e.g., READ, WRITE, etc.)
  private int requestID; // Unique identifier for the request
  private String data; // Additional data associated with the request

  /**
   * Default constructor required for deserialization.
   */
  public RequestMessage() {
  }

  /**
   * Constructor to initialize a RequestMessage with specific values.
   * 
   * @param requestType The type of the request.
   * @param requestID   The unique identifier for the request.
   * @param data        The data associated with the request.
   */
  public RequestMessage(int requestType, int requestID, String data) {
    this.requestType = requestType;
    this.requestID = requestID;
    this.data = data;
  }

  /**
   * Converts the RequestMessage object to a string representation.
   * 
   * @return A string containing the request type, ID, and data.
   */
  public String toString() {
    return "RequestMessage: " + this.requestType + " " + this.requestID + " " + this.data;
  }

  /**
   * Gets the type of the request.
   * 
   * @return The request type as an integer.
   */
  public int getRequestType() {
    return this.requestType;
  }

  /**
   * Gets the operation type as an Operation enum.
   * 
   * @return The operation type.
   */
  public Operation getOperation() {
    return Operation.fromInt(this.requestType);
  }

  /**
   * Gets the unique identifier for the request.
   * 
   * @return The request ID.
   */
  public int getRequestID() {
    return this.requestID;
  }

  /**
   * Gets the data associated with the request.
   * 
   * @return The request data as a string.
   */
  public String getData() {
    return this.data;
  }
}
