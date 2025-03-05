package Server;

import java.net.*;

import Server.utils.Serializer;

import java.io.*;

// this class is just for testing the server
public class UDPClient {
  public static void main(String args[]) {
    // args give message contents and server hostname
    String message = "Hello from UDP client!";
    String hostname = "localhost";

    int clientPort = 62766;
    DatagramSocket aSocket = null;
    System.out.println("");
    try {
      aSocket = new DatagramSocket(clientPort); // use a free local port
      RequestMessage requestMessage = new RequestMessage(Operation.READ.getOpCode(), 2, "ALL");
      byte[] m = Serializer.serialize(requestMessage);

      // translate user-specified hostname to Internet address
      InetAddress aHost = InetAddress.getByName(hostname); // args[1]
      int serverPort = 6789;
      DatagramPacket request = new DatagramPacket(m,
          m.length, aHost, serverPort);
      aSocket.send(request);

      // send packet using socket method
      byte[] buffer = new byte[1000]; // a buffer for receive
      DatagramPacket reply = new DatagramPacket(buffer,
          buffer.length);
      aSocket.receive(reply);

      RequestMessage deserialised_data = (RequestMessage) Serializer.deserialize(reply.getData());

      System.out.println("Reply from server: " + deserialised_data.toString());
    } catch (SocketException e) {
      System.err.println("Socket error: " + e.getMessage());
    } catch (IOException e) {
      System.err.println("IO error: " + e.getMessage());
    } catch (Exception e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    } finally {
      if (aSocket != null)
        aSocket.close();
    }
  }
}