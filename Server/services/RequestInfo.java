package Server.services;

import java.net.InetAddress;

import Server.RequestMessage;

/**
 * RequestInfo class is used to store information about a client request.
 * This includes the request message, client address, client port, and the
 * response message.
 * It is used to prevent duplicate requests for non-idempotent operations.
 */
public class RequestInfo {
  public RequestMessage requestMessage; // The original request message
  public InetAddress clientAddress; // The address of the client
  public int clientPort; // The port of the client
  public RequestMessage responseMessage; // The response message for the request

  /**
   * Constructor to initialize a RequestInfo object.
   * 
   * @param requestMessage  The request message.
   * @param clientAddress   The client's address.
   * @param clientPort      The client's port.
   * @param responseMessage The response message for the request.
   */
  public RequestInfo(RequestMessage requestMessage, InetAddress clientAddress, int clientPort,
      RequestMessage responseMessage) {
    this.requestMessage = requestMessage;
    this.clientAddress = clientAddress;
    this.clientPort = clientPort;
    this.responseMessage = responseMessage;
  }

  /**
   * Converts the RequestInfo object to a string representation.
   * 
   * @return A string containing the request message, client address, client port,
   *         and response message.
   */
  public String toString() {
    return "RequestInfo: " + this.requestMessage.toString() + ", " + this.clientAddress + ":" + this.clientPort + ", "
        + this.responseMessage.toString();
  }
}
