package Server;

import java.net.*;
import java.util.Scanner;
import Server.utils.Serializer;
import java.io.*;

/**
 * UDPClient class for testing the server.
 * Allows users to send requests to the server and receive responses.
 */
public class UDPClient {
  public static void main(String args[]) {
    // Default server and client configuration
    String hostname = "localhost"; // Server hostname
    int serverPort = 6789;         // Server port
    int clientPort = 62766;        // Client port

    // Create a scanner for user input
    Scanner scanner = new Scanner(System.in);
    DatagramSocket aSocket = null;

    try {
      // Create a DatagramSocket bound to the client port
      aSocket = new DatagramSocket(clientPort);
      InetAddress aHost = InetAddress.getByName(hostname); // Resolve server hostname

      boolean continueRunning = true; // Flag to keep the client running

      while (continueRunning) {
        // Prompt user for operation code
        System.out.println("\nEnter operation code (integer) (-1=NONE, 0=READ, 1=WRITE, 2=UPDATE, 3=DELETE, 4=MONITOR, 5=ECHO, 7=EXIT): ");
        int opCode;
        try {
          opCode = Integer.parseInt(scanner.nextLine());
          // Check for exit condition
          if (opCode == 7) {
            System.out.println("Exiting client...");
            continueRunning = false;
            continue;
          }
          // Validate operation code is within the valid range
          if (opCode < -1 || opCode > 7) {
            System.err.println("Invalid operation code. Using READ (2) as default.");
            opCode = Operation.READ.getOpCode();
          }
        } catch (NumberFormatException e) {
          // Handle invalid input for operation code
          System.err.println("Invalid operation code. Using READ (2) as default.");
          opCode = Operation.READ.getOpCode();
        }

        // Prompt user for request ID
        System.out.println("Enter request ID (integer): ");
        int recordId;
        try {
          recordId = Integer.parseInt(scanner.nextLine());
        } catch (NumberFormatException e) {
          // Handle invalid input for request ID
          System.err.println("Invalid record ID. Using 0 as default.");
          recordId = 0;
        }

        // Prompt user for location
        System.out.println("Enter location: ");
        String location = scanner.nextLine();
        if (location.trim().isEmpty()) {
          // Default to "ALL" if no location is provided
          location = "ALL";
          System.out.println("Empty location. Using 'ALL' as default.");
        }

        System.out.println("");
        try {
          // Create a RequestMessage object with user input
          RequestMessage requestMessage = new RequestMessage(opCode, recordId, location);
          System.out.println("Sending request: " + requestMessage.toString());

          // Serialize the request message into a byte array
          byte[] m = Serializer.serialize(requestMessage);

          // Create a DatagramPacket to send the request to the server
          DatagramPacket request = new DatagramPacket(m, m.length, aHost, serverPort);
          aSocket.send(request); // Send the request

          // Prepare a buffer to receive the server's response
          byte[] buffer = new byte[1000];
          DatagramPacket reply = new DatagramPacket(buffer, buffer.length);
          aSocket.receive(reply); // Receive the response

          // Deserialize the response message
          RequestMessage deserialized_data = (RequestMessage) Serializer.deserialize(reply.getData());
          System.out.println("Reply from server: " + deserialized_data.toString());
        } catch (IOException e) {
          // Handle IO exceptions
          System.err.println("IO error: " + e.getMessage());
        } catch (Exception e) {
          // Handle other exceptions
          e.printStackTrace();
        }
      }
    } catch (SocketException e) {
      // Handle socket-related exceptions
      System.err.println("Socket error: " + e.getMessage());
    } catch (IOException e) {
      // Handle IO exceptions during socket creation
      System.err.println("IO error: " + e.getMessage());
    } finally {
      // Ensure the socket is closed and scanner is released
      if (aSocket != null)
        aSocket.close();
      scanner.close();
    }
  }
}