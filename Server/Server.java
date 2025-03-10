package Server;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.Arrays;

import Server.services.MonitorService;
import Server.services.RequestInfo;
import Server.services.RequestHistory;
import Server.utils.Serializer;

import java.io.IOException;

public class Server {
  private static MonitorService facultyMonitorService;
  private static RequestHistory requestHistory;

  private static DatagramSocket aSocket = null;

  public static void main(String[] args) {
    testSerialisation();
    init();
    RequestMessage requestMessage = null;

    try {

      // Bind socket to port 6789
      aSocket = new DatagramSocket(6789);
      byte[] buffer = new byte[1000];

      System.out.println("UDP Server is running on port 6789...");
      while (true) {
        // Receive request
        DatagramPacket request = new DatagramPacket(buffer, buffer.length);
        aSocket.receive(request); // Blocks until a packet is received

        // Deserialize request
        try {
          byte[] receivedData = Arrays.copyOfRange(request.getData(), 0, request.getLength());

          // Print out the data
          System.out.println("Received data size: " + receivedData.length + " bytes");
          for (byte b : receivedData) {
            System.out.print(String.format("%02X ", b));
          }
          System.out.println();

          printSupposedData(1, 1, "Hello from client!");

          requestMessage = (RequestMessage) Serializer.deserialize(receivedData);
          // requestMessage = (RequestMessage) Serializer.deserialize(request.getData());
        } catch (Exception e) {
          System.err.println("Error deserializing request: " + e.getMessage());

          // for cpp client
          requestMessage = new RequestMessage(0, 0, "ERROR: bad request");
          byte[] replybuff = null;
          try {
            replybuff = Serializer.serialize(requestMessage);
          } catch (Exception e1) {
            System.err.println("Error serializing bad response: " + e1.getMessage());
          }

          DatagramPacket reply = new DatagramPacket(replybuff,
              replybuff.length,
              request.getAddress(),
              request.getPort());
          aSocket.send(reply);
          continue;
        }

        // RequestMessage responseMessage = handleRequest(request);
        RequestMessage responseMessage = new RequestMessage(0, 0, "Hello from Server!");

        // send response
        try {
          byte[] replybuff = Serializer.serialize(responseMessage);
          System.out.println("serialized. Reply buff byte data: ");
          for (byte b : replybuff) {
            System.out.print(String.format("%02X ", b));
          }
          System.out.println("bytebuff length: " + replybuff.length);
          // Send response (echo the received message)
          DatagramPacket reply = new DatagramPacket(replybuff,
              replybuff.length,
              request.getAddress(),
              request.getPort());
          aSocket.send(reply);
        } catch (Exception e) {
          System.out.println("Error serializing response: " + e.getMessage());
        }

        System.out.println("Replied to: " + request.getAddress() + ":" + request.getPort());
      }
    } catch (SocketException e) {
      System.err.println("Socket error: " + e.getMessage());
    } catch (IOException e) {
      System.err.println("IO error: " + e.getMessage());
    } finally {
      if (aSocket != null && !aSocket.isClosed()) {
        aSocket.close();
        System.out.println("Server socket closed.");
      }
    }
  }

  // initailize the service classes
  private static void init() {
    facultyMonitorService = new MonitorService();
    requestHistory = new RequestHistory();
  }

  /**
   * Implements server logic to handle requests from clients.
   * 
   * @param request DatagramPacket request from client.
   * @return RequestMessage response to client.
   */
  public static RequestMessage handleRequest(DatagramPacket request) {
    RequestMessage requestMessage = null;
    RequestMessage responseMessage = null;
    // Deserialize request
    try {
      requestMessage = (RequestMessage) Serializer.deserialize(request.getData());
    } catch (Exception e) {
      System.err.println("Error deserializing request: " + e.getMessage());
      // reply that message is bad, comment out if not needed, use line below if
      // commented out
      // requestMessage = new RequestMessage(0, 0, "manual set server");

      // TODO uncomment
      // responseMessage = new RequestMessage(Operation.READ.getOpCode(), 0, "ERROR:
      // bad request");
      return responseMessage;
    }

    // TODO uncomment
    // responseMessage = new RequestMessage(0, 0, "Good: good request");
    return responseMessage;

    // RequestInfo requestInformation = new RequestInfo(responseMessage,
    // request.getAddress(), request.getPort(), null);

    // // Check if request is in history
    // RequestInfo prevRequest =
    // requestHistory.containsAndReplace(requestInformation);
    // if (prevRequest != null) {
    // System.out.println("Request already processed: " +
    // requestMessage.getRequestID());
    // // response for already done is the same as the previous request
    // responseMessage = prevRequest.responseMessage;
    // return responseMessage;
    // }

    // // Process request

    // // Check if request changes monitored resources
    // // TODO: the list of things to monitor is not confirmed currently, to add on
    // if
    // // needed
    // if (requestMessage.getOperation() == Operation.UPDATE) {
    // facultyMonitorService.notifyAll(aSocket);
    // }
    // return responseMessage;
  }

  private static void printSupposedData(int requestType, int requestID, String data) {
    RequestMessage x = new RequestMessage(requestType, requestID, data);
    byte[] buffer = null;

    try {
      buffer = Serializer.serialize(x);

      System.out.println("Supposed data size: " + buffer.length + " bytes");
      for (byte b : buffer) {
        System.out.print(String.format("%02X ", b & 0xFF));
      }
      System.out.println();
    } catch (Exception e) {
      System.err.println("Error serializing request: " + e.getMessage());
      e.printStackTrace();
    }
  }

  private static void testSerialisation() {
    RequestMessage responseMessage = new RequestMessage(0, 0, "Hello from Server!");
    try {
      byte[] replybuff = Serializer.serialize(responseMessage);
      RequestMessage requestMessage = (RequestMessage) Serializer.deserialize(replybuff);

      System.out.println("requestMessage id: " + requestMessage.getRequestID());
      System.out.println("Request message type: " + requestMessage.getRequestType());
      System.out.println("Request Message Data: " + requestMessage.getData());
    } catch (Exception e) {
      System.err.println("Error serializing request: " + e.getMessage());
      e.printStackTrace();
    }

  }

}
