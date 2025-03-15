package Server;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.time.DayOfWeek;
import java.util.UUID;

import Server.models.Availability;
import Server.models.Facility;
import Server.services.FacilityFactory;
import Server.services.MonitorService;
import Server.services.RequestInfo;
import Server.services.RequestHistory;
import Server.utils.Serializer;

import java.io.IOException;

public class Server {
  private static MonitorService facilityMonitorService;
  private static RequestHistory requestHistory;
  private static FacilityFactory facilityFactory;

  private static DatagramSocket aSocket = null;

  public static void main(String[] args) {
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

        System.out.println("Received request from: " + request.getAddress() + ":" + request.getPort());
        RequestMessage responseMessage = handleRequest(request);
        System.out.println("Current request history: " + requestHistory.toString());

        // send response
        try {
          byte[] replybuff = Serializer.serialize(responseMessage);
          System.out.println("serialized");
          // Send response (echo the received message)
          DatagramPacket reply = new DatagramPacket(replybuff,
              replybuff.length,
              request.getAddress(),
              request.getPort());
          aSocket.send(reply);

          System.out.println(
              "Replied to " + request.getAddress() + ":" + request.getPort() + " with: " + responseMessage.toString());
        } catch (Exception e) {
          System.out.println("Error serializing response: " + e.getMessage());
        }

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
    facilityMonitorService = new MonitorService();
    requestHistory = new RequestHistory();
    facilityFactory = new FacilityFactory();

    // Create some facilities at the start
    Facility f1 = facilityFactory.newFacility("Weekday1");
    Facility f2 = facilityFactory.newFacility("Weekday2");
    Facility f3 = facilityFactory.newFacility("Weekends");

    // Set availability for each facility
    Availability a1 = new Availability();
    a1.setAvaOfDay(DayOfWeek.MONDAY, true, 8, 0, 17, 0);
    a1.setAvaOfDay(DayOfWeek.TUESDAY, true, 8, 0, 17, 0);
    a1.setAvaOfDay(DayOfWeek.WEDNESDAY, true, 8, 0, 17, 0);
    f1.setAvailability(a1);

    Availability a2 = new Availability();
    a2.setAvaOfDay(DayOfWeek.WEDNESDAY, true, 8, 30, 23, 30);
    a2.setAvaOfDay(DayOfWeek.THURSDAY, true, 8, 30, 23, 30);
    a2.setAvaOfDay(DayOfWeek.FRIDAY, true, 8, 30, 23, 30);
    f2.setAvailability(a2);

    Availability a3 = new Availability();
    a3.setAvaOfDay(DayOfWeek.SATURDAY, true, 9, 0, 18, 0);
    a3.setAvaOfDay(DayOfWeek.SUNDAY, true, 9, 0, 18, 0);
    f3.setAvailability(a3);
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
      System.out.println("Request: " + requestMessage.toString());
    } catch (Exception e) {
      System.err.println("Error deserializing request: " + e.getMessage());

      responseMessage = new RequestMessage(Operation.READ.getOpCode(), 0, "ERROR: bad request");
      return responseMessage;
    }
    // do nothing if requested server to echo 
    if (requestMessage.getOperation() == Operation.ECHO) {
      System.out.println("Echo request received");
      return requestMessage;
    }


    // TODO uncomment
    // responseMessage = new RequestMessage(0, 0, "Good: good request");
    // return responseMessage;

    // Check if request is in history, return previous response if it is
    RequestInfo requestInformation = new RequestInfo(
        requestMessage,
        request.getAddress(),
        request.getPort(),
        null);
    RequestInfo prevRequest = requestHistory.containsRequest(requestInformation);
    if (prevRequest != null) {
      System.out.println("Duplicate request id detected: " +
          requestMessage.getRequestID());
      // response for already done is the same as the previous request
      responseMessage = prevRequest.responseMessage;
      return responseMessage;
    }

    // Process request
    switch (requestMessage.getOperation()) {
      case READ:
        // read facility infomation
        String requestString = requestMessage.getData();
        // different behaviour based on string in request
        switch (requestString) {
          case "ALL":
            // return all facility names
            String facilityNames = "";
            for (Facility facility : facilityFactory.getFacilities()) {
              facilityNames += "Facility: " + facility.getName() + " ";
            }
            responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                facilityNames);
            break;
          default:
            // return specified facility information
            System.out.println("handleRequest: Requested facility: " + requestString);
            Facility requestedFacility = facilityFactory.getFacility(requestString);
            if (requestedFacility != null) {
              responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                  requestedFacility.toString());
            } else {
              responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                  "ERROR: facility not found");
            }
            break;
        }

        break;
      case WRITE:
        // Extract booking details from the request message
        String[] bookingDetails = requestMessage.getData().split(",");
        if (bookingDetails.length != 6) {
            responseMessage = new RequestMessage(Operation.WRITE.getOpCode(), requestMessage.getRequestID(),
                "ERROR: invalid booking request format");
            break;
        }

        String facilityName = bookingDetails[0];
        DayOfWeek day;
        int startHour, startMinute, endHour, endMinute;

        try {
            day = DayOfWeek.valueOf(bookingDetails[1].toUpperCase());
            startHour = Integer.parseInt(bookingDetails[2]);
            startMinute = Integer.parseInt(bookingDetails[3]);
            endHour = Integer.parseInt(bookingDetails[4]);
            endMinute = Integer.parseInt(bookingDetails[5]);
        } catch (Exception e) {
            responseMessage = new RequestMessage(Operation.WRITE.getOpCode(), requestMessage.getRequestID(),
                "ERROR: invalid booking request parameters");
            break;
        }

        // Book the facility
        String bookingResult = bookFacility(facilityName, day, startHour, startMinute, endHour, endMinute, request.getAddress(), request.getPort());
        responseMessage = new RequestMessage(Operation.WRITE.getOpCode(), requestMessage.getRequestID(), bookingResult);
        break;
      case UPDATE:
        responseMessage = new RequestMessage(Operation.READ.getOpCode(), 0, "ERROR: unimplemented operation");
        break;
      case DELETE:
        responseMessage = new RequestMessage(Operation.READ.getOpCode(), 0, "ERROR: unimplemented operation");
        break;
      case MONITOR:
        responseMessage = new RequestMessage(Operation.READ.getOpCode(), 0, "ERROR: unimplemented operation");
        break;
      default:
        responseMessage = new RequestMessage(Operation.READ.getOpCode(), 0, "ERROR: unknown operation");
        break;
    }

    // save request and response to history
    RequestInfo currentRequestInformation = new RequestInfo(
        requestMessage,
        request.getAddress(),
        request.getPort(),
        responseMessage);
    requestHistory.addRequest(currentRequestInformation);

    // Check if request changes monitored resources
    // TODO: the list of things to monitor is not confirmed currently, to add on if
    // needed
    // if (requestMessage.getOperation() == Operation.UPDATE) {
    // facilityMonitorService.notifyAll(aSocket);
    // }
    return responseMessage;
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

  private static String bookFacility(String facilityName, DayOfWeek day, int startHour, int startMinute, int endHour, int endMinute, InetAddress userAddress, int userPort) {
    Facility facility = facilityFactory.getFacility(facilityName);
    if (facility == null) {
        return "ERROR: facility not found";
    }

    Availability availability = facility.getAvailability();
    if (availability == null || !availability.isAvailable(day, startHour, startMinute, endHour, endMinute)) {
        return "ERROR: facility not available at the requested time";
    }

    // Capture user information
    String userInfo = userAddress.toString() + ":" + userPort;

    // Book the facility and get the confirmation ID
    String confirmationID = availability.bookTimeSlot(day, startHour, startMinute, endHour, endMinute, userInfo);

    return "Booking_ID: " + confirmationID + " by " + userInfo;
  }

}
