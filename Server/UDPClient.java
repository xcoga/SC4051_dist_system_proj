package Server;

import java.net.*;
import java.util.Scanner;
import Server.utils.Serializer;
import java.io.*;

// this class is just for testing the server
public class UDPClient {
  public static void main(String args[]) {
    // Default values
    String hostname = "localhost";
    int serverPort = 6789;
    int clientPort = 62766;
    
    // Create scanner for user input
    Scanner scanner = new Scanner(System.in);
    DatagramSocket aSocket = null;
    
    try {
      aSocket = new DatagramSocket(clientPort);
      InetAddress aHost = InetAddress.getByName(hostname);
      
      boolean continueRunning = true;
      
      while (continueRunning) {
        // Get operation input as integer
        System.out.println("\nEnter operation code (integer) (1=WRITE, 2=UPDATE, 3=DELETE, 4=MONITOR, 0=EXIT): ");
        int opCode;
        try {
          opCode = Integer.parseInt(scanner.nextLine());
          // Check for exit condition
          if (opCode == 0) {
            System.out.println("Exiting client...");
            continueRunning = false;
            continue;
          }
          // Validate operation code is in range
          if (opCode < 1 || opCode > 4) {
            System.err.println("Invalid operation code. Using READ (2) as default.");
            opCode = Operation.READ.getOpCode();
          }
        } catch (NumberFormatException e) {
          System.err.println("Invalid operation code. Using READ (2) as default.");
          opCode = Operation.READ.getOpCode();
        }
        
        // Get record ID input
        System.out.println("Enter request ID (integer): ");
        int recordId;
        try {
          recordId = Integer.parseInt(scanner.nextLine());
        } catch (NumberFormatException e) {
          System.err.println("Invalid record ID. Using 0 as default.");
          recordId = 0;
        }
        
        // Get location input
        System.out.println("Enter location: ");
        String location = scanner.nextLine();
        if (location.trim().isEmpty()) {
          location = "ALL";
          System.out.println("Empty location. Using 'ALL' as default.");
        }
        
        System.out.println("");
        try {
          // Create RequestMessage with user input
          RequestMessage requestMessage = new RequestMessage(opCode, recordId, location);
          System.out.println("Sending request: " + requestMessage.toString());
          
          byte[] m = Serializer.serialize(requestMessage);
          
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
        } catch (IOException e) {
          System.err.println("IO error: " + e.getMessage());
        } catch (Exception e) {
          e.printStackTrace();
        }
      }
    } catch (SocketException e) {
      System.err.println("Socket error: " + e.getMessage());
    } catch (IOException e) {
      System.err.println("IO error: " + e.getMessage());
    } finally {
      if (aSocket != null)
        aSocket.close();
      scanner.close();
    }
  }
}