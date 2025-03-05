package Server.services;

import java.net.InetAddress;

import Server.RequestMessage;

/**
 * Request class is used to store request information.
 * To be used to prevent duplicate requests for non-idempotent requests.
 */
public class RequestInfo {
  public RequestMessage requestMessage; // request message
  public InetAddress clientAddress; // client address
  public int clientPort; // client port
  public RequestMessage responseMessage; // response message

  public RequestInfo(RequestMessage requestMessage, InetAddress clientAddress, int clientPort,
      RequestMessage responseMessage) {
    this.requestMessage = requestMessage;
    this.clientAddress = clientAddress;
    this.clientPort = clientPort;
    this.responseMessage = responseMessage;
  }

  public String toString() {
    return "RequestInfo: " + this.requestMessage.toString() + ", " + this.clientAddress + ":" + this.clientPort + ", "
        + this.responseMessage.toString();
  }
}
