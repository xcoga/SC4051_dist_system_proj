package Server;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.time.DayOfWeek;
import java.util.List;
import java.util.UUID;

import Server.models.Availability;
import Server.models.Booking;
import Server.models.Facility;
import Server.models.TimeSlot;
import Server.models.BookingDetails;
import Server.services.FacilityFactory;
import Server.services.Monitor;
import Server.services.MonitorService;
import Server.services.RequestInfo;
import Server.services.RequestHistory;
import Server.utils.Serializer;
import Server.utils.Parser;

import java.io.IOException;

// Server class to handle requests from clients and manage facility bookings
public class Server {
  private static final double DROP_CHANCE = 0.1; // % chance to drop a request

  private static MonitorService facilityMonitorService;
  private static RequestHistory requestHistory;
  private static FacilityFactory facilityFactory;

  private static boolean checkHistory = true; // true for at-most-once, false for at-least-once

  private static DatagramSocket aSocket = null;
  private static String lastUpdatedFacility = null;

  public static void main(String[] args) {
    String invocationScematic = args.length > 0 ? args[0] : "default";

    if (invocationScematic.equals("at-least-once")) {
      System.out.println("Server started with at-least-once invocation semantics.");
      checkHistory = false;
    } else {
      System.out.println("Server started with at-most-once invocation semantics.");
    }

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

        boolean dropRequest = Math.random() < DROP_CHANCE;
        if (dropRequest) {
          System.out.println("Dropping request from " + request.getAddress() + ":" + request.getPort());
          continue; // Skip processing this request
        }
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
    lastUpdatedFacility = null;

    // Deserialize request
    try {
      byte[] receivedData = new byte[request.getLength()];
      System.arraycopy(request.getData(), request.getOffset(), receivedData, 0, request.getLength());
      requestMessage = (RequestMessage) Serializer.deserialize(receivedData);
      System.out.println("Request: " + requestMessage.toString());
    } catch (Exception e) {
      System.err.println("Error deserializing request: " + e.getMessage());

      responseMessage = new RequestMessage(Operation.READ.getOpCode(), 0, "status:ERROR\nmessage:Bad request");
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

    // if invocation semantics is at-least-once, check if request is in history
    if (checkHistory) {
      System.out.println("Checking request history...");
      // Check if request is in history, return previous response if it is
      RequestInfo requestInformation = new RequestInfo(
          requestMessage,
          request.getAddress(),
          request.getPort(),
          null);
      RequestInfo prevRequest = requestHistory.containsRequest(requestInformation);
      if (prevRequest != null) {
        System.out.printf("Duplicate %s request id %d detected\n",requestMessage.getOperation(),requestMessage.getRequestID());
        // response for already done is the same as the previous request
        responseMessage = prevRequest.responseMessage;
        return responseMessage;
      }
    }

    // Process request
    switch (requestMessage.getOperation()) {
      case READ:
        // example requests format: facility,Weekday1
        String[] requestString = requestMessage.getData().split(",");
        if (requestString == null || requestString.length == 1) {
          responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
              "status:ERROR\nmessage:Invalid request format");
          break;
        }

        if (requestString[0].equals("facility")) {
          // read facility information
          // different behaviour based on string in request
          switch (requestString[1]) {
            case "ALL":
              // return all facility names
              String facilityNames = "";
              for (Facility facility : facilityFactory.getFacilities()) {
                facilityNames += facility.getName() + ",";
              }
              responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                  String.format("status:SUCCESS\nfacilityNames:%s", facilityNames));
              break;
            default:
              responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                  formatFacilityAvailability(requestString[1]));
              break;
          }
        } else if (requestString[0].equals("booking")) {
          // read booking information based on booking id string in request
          // example requests format: booking,bookingID
          String bookingID = requestString[1];
          for (Facility facility : facilityFactory.getFacilities()) {
            Availability availability = facility.getAvailability();
            if (availability != null) {
              Booking bookingInfo = availability.getBookingInfo(bookingID);
              if (bookingInfo != null) {
                responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                    "status:SUCCESS\n" +
                        "bookingID:" + bookingInfo.confirmationID + "\n" +
                        "user:" + bookingInfo.userInfo + "\n" +
                        "facility:" + facility.getName() + "\n" +
                        "day:" + bookingInfo.getDay() + "\n" +
                        "startTime:" + bookingInfo.timeSlot.getStartTime() + "\n" +
                        "endTime:" + bookingInfo.timeSlot.getEndTime() + "\n");
                break;
              }
            }
          }
          if (responseMessage == null) {
            responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                "status:ERROR\nmessage:Booking not found");
          }
        } else if (requestString[0].equals("rating")) {
          // read rating information based on facility name string in request
          // Expected request format: "rating",facility_name

          Facility facility = facilityFactory.getFacility(requestString[1]);
          if (facility != null) {
            String rating = getRating(facility);
            responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(), rating);
          } else {
            responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                "status:ERROR\nmessage:Facility not found");
          }
        } else {
          responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
              "status:ERROR\nmessage:Invalid request format");
        }
        break;

      case WRITE:
        // For book facility request, format:
        // facilityName,day,startHour,startMinute,endHour,endMinute
        try {
          responseMessage = bookFacility(requestMessage, request.getAddress(), request.getPort());
        } catch (Exception e) {
          responseMessage = new RequestMessage(Operation.WRITE.getOpCode(), requestMessage.getRequestID(),
              "status:ERROR\nmessage:" + e.getMessage());
        }

        break;

      // UPDATE has 2 cases: 1. Update booking 2. Add a rating
      // 1. Update Booking, Expected format:
      // "book,<prevBookingID>,<facilityName>,<Day>,<startHour>,<startMinute>,<endHour>,<endMinute>"
      // 2. Add Rating, Expected format: "rating",facilityName,rating
      case UPDATE:

        try {
          // Parse the request to get the type of update
          String updateType = Parser.parseUpdateType(requestMessage.getData());

          // Updating an existing booking
          if (updateType.equals("booking")) {
            responseMessage = updateBooking(requestMessage, request.getAddress(), request.getPort());
          }

          // Updating a Facility rating
          else if (updateType.equals("rating")) {
            responseMessage = addRating(requestMessage, request.getAddress(), request.getPort());
          }

          // Invalid update type
          else {
            responseMessage = new RequestMessage(
                Operation.UPDATE.getOpCode(),
                requestMessage.getRequestID(),
                "status:ERROR\nmessage:Invalid update type");
          }
        } catch (Exception e) {
          responseMessage = new RequestMessage(
              Operation.UPDATE.getOpCode(),
              requestMessage.getRequestID(),
              "status:ERROR\nmessage:" + e.getMessage());
        }

        break;

      case DELETE:

        try {
          // Delete booking. Expected format: "<bookingId>,<facilityName>"
          responseMessage = deleteBooking(requestMessage, request.getAddress(), request.getPort());

        } catch (Exception e) {
          responseMessage = new RequestMessage(
              Operation.DELETE.getOpCode(),
              requestMessage.getRequestID(),
              "status:ERROR\nmessage:" + e.getMessage());
        }
        break;

      case MONITOR:
        // Monitor request format: "<register>,<facilityName>,<monitor interval>"
        try {
          String[] monitorRequestString = requestMessage.getData().split(",");
          if (monitorRequestString == null || monitorRequestString.length == 1) {
            responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                "status:ERROR\nmessage:Invalid request format");
            break;
          }

          if (monitorRequestString[0].equals("register")) {
            // Register facility for monitoring
            String facilityName = monitorRequestString[1];
            int monitorInterval = Integer.parseInt(monitorRequestString[2]);

            Monitor clientMonitor = new Monitor(facilityName, requestMessage.getRequestID(), request.getAddress(),
                request.getPort(), monitorInterval);

            facilityMonitorService.registerMonitor(clientMonitor);
            responseMessage = new RequestMessage(
              Operation.MONITOR.getOpCode(),
              requestMessage.getRequestID(),
              String.format(
                "status:SUCCESS\n" +
                "facility:%s\n" +
                "interval:%d",
                facilityName, monitorInterval
              )
            );
          } else {
            responseMessage = new RequestMessage(Operation.MONITOR.getOpCode(), requestMessage.getRequestID(),
                "status:ERROR\nmessage:Invalid request format");
          }
        } catch (Exception e) {
          responseMessage = new RequestMessage(Operation.MONITOR.getOpCode(), requestMessage.getRequestID(),
              "status:ERROR\nmessage:" + e.getMessage());
        }

        break;
      default:
        responseMessage = new RequestMessage(Operation.NONE.getOpCode(), requestMessage.getRequestID(),
            "status:ERROR\nmessage:Unknown operation");
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
    // Only notify if write or update or delete operation and is booking related
    try {
      if (lastUpdatedFacility != null && (requestMessage.getOperation() == Operation.WRITE
          || requestMessage.getOperation() == Operation.UPDATE || requestMessage.getOperation() == Operation.DELETE)) {
        facilityMonitorService.removeExpiredMonitors();

        String notification = formatFacilityAvailability(lastUpdatedFacility);
        if (!(notification == null || notification.isEmpty() || notification.contains("status:ERROR"))) {
          facilityMonitorService.notifyAll(aSocket, lastUpdatedFacility, notification);
        }
      }
    } catch (Exception e) {
      System.err.println("Error notifying clients: " + e.getMessage());
    }

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

  private static RequestMessage bookFacility(RequestMessage request, InetAddress userAddress, int userPort) {

    BookingDetails booking = Parser.parseBookingDetails(request.getData());

    String facilityName = booking.getFacilityName();
    DayOfWeek day = booking.getDay();
    int startHour = booking.getStartHour();
    int startMinute = booking.getStartMinute();
    int endHour = booking.getEndHour();
    int endMinute = booking.getEndMinute();

    RequestMessage responseMessage;

    // Check if facility is available for booking
    Facility facility = facilityFactory.getFacility(facilityName);
    if (facility == null) {
      responseMessage = new RequestMessage(
          Operation.WRITE.getOpCode(),
          request.getRequestID(),
          "status:ERROR\nmessage:Facility not found");
      return responseMessage;
    }
    lastUpdatedFacility = facilityName;

    Availability availability = facility.getAvailability();
    TimeSlot bookTimeSlot = new TimeSlot(startHour, startMinute, endHour, endMinute, day);
    if (availability == null || !availability.isAvailable(bookTimeSlot)) {
      responseMessage = new RequestMessage(
          Operation.WRITE.getOpCode(),
          request.getRequestID(),
          "status:ERROR\nmessage:Facility not available at the requested time");
      return responseMessage;
    }

    // Capture user information
    String userInfo = userAddress.toString() + ":" + userPort;

    // Book the facility and get the confirmation ID
    String confirmationID = availability.bookTimeSlot(userInfo, bookTimeSlot);

    // Craft response message to send back to client
    responseMessage = new RequestMessage(
        Operation.WRITE.getOpCode(),
        request.getRequestID(),
        String.format(
            "status:SUCCESS\n" +
              "bookingID:%s\n" +
              "user:%s\n" +
              "facility:%s\n" +
              "day:%s\n" +
              "startTime:%s\n" +
              "endTime:%s",
            confirmationID, userInfo, facilityName, bookTimeSlot.getDay(), bookTimeSlot.getStartTime(),
            bookTimeSlot.getEndTime()));

    return responseMessage;
  }

  // Update booking. Expected format:
  // "book,<prevBookingID>,<facilityName>,<Day>,<startHour>,<startMinute>,<endHour>,<endMinute>"
  private static RequestMessage updateBooking(RequestMessage requestMessage, InetAddress userAddress, int userPort) {

    // Extract booking details from the request message.
    // 1. BookingID to delete
    // 2. New booking details to book

    BookingDetails booking = Parser.parseUpdateBookingDetails(requestMessage.getData());

    String prev_bookingId = booking.getPrev_bookingId();
    String facilityName = booking.getFacilityName();
    DayOfWeek day = booking.getDay();
    int startHour = booking.getStartHour();
    int startMinute = booking.getStartMinute();
    int endHour = booking.getEndHour();
    int endMinute = booking.getEndMinute();

    TimeSlot newTimeSlot = new TimeSlot(startHour, startMinute, endHour, endMinute, day);

    RequestMessage responseMessage;

    Facility facility = facilityFactory.getFacility(facilityName);
    if (facility == null) {
      responseMessage = new RequestMessage(
          Operation.UPDATE.getOpCode(),
          requestMessage.getRequestID(),
          "status:ERROR\nmessage:Facility not found");
      return responseMessage;
    }

    lastUpdatedFacility = facilityName;

    Availability availability = facility.getAvailability();

    // Capture user information
    String userInfo = userAddress.toString() + ":" + userPort;

    // Book the facility and get the confirmation ID
    try {
      String delete_confirmationID = availability.removeTimeSlot(prev_bookingId, userInfo);

      String booking_confirmationID = availability.bookTimeSlot(userInfo, newTimeSlot);

      String dayStr = day.name();

      responseMessage = new RequestMessage(
          Operation.UPDATE.getOpCode(),
          requestMessage.getRequestID(),
          String.format(
              "status:SUCCESS\n" +
                  "oldBookingID:%s\n" +
                  "newBookingID:%s\n" +
                  "user:%s\n" +
                  "facility:%s\n" +
                  "day:%s\n" +
                  "startTime:%s\n" +
                  "endTime:%s\n",
              delete_confirmationID, booking_confirmationID, userInfo, facilityName, dayStr, newTimeSlot.getStartTime(),
              newTimeSlot.getEndTime()
          )
      );

      return responseMessage;

    } catch (Exception e) {
      responseMessage = new RequestMessage(
          Operation.UPDATE.getOpCode(),
          requestMessage.getRequestID(),
          String.format("status:ERROR\nmessage:Booking not successful or booking by %s is not found", userInfo));
      return responseMessage;
    }
  }

  // Delete booking. Expected format: "<bookingId>,<facilityName>"
  private static RequestMessage deleteBooking(RequestMessage requestMessage, InetAddress userAddress, int userPort) {

    String[] parts = requestMessage.getData().split(",");
    String bookingId = parts[0];
    String facilityName = parts[1];

    Facility facility = facilityFactory.getFacility(facilityName);
    if (facility == null) {
      return new RequestMessage(
          Operation.DELETE.getOpCode(),
          requestMessage.getRequestID(),
          "status:ERROR\nmessage:Facility not found");
    }
    lastUpdatedFacility = facilityName;

    Availability availability = facility.getAvailability();

    // Capture user information
    String userInfo = userAddress.toString() + ":" + userPort;

    try {
      // Remove the booking for that facility and get the confirmation ID
      String confirmationID = availability.removeTimeSlot(bookingId, userInfo);
      return new RequestMessage(
          Operation.DELETE.getOpCode(),
          requestMessage.getRequestID(),
          String.format("status:SUCCESS\nbookingID:%s\nuser:%s", confirmationID, userInfo));
    } catch (Exception e) {
      return new RequestMessage(
          Operation.DELETE.getOpCode(),
          requestMessage.getRequestID(),
          "status:ERROR\nmessage:Booking not found");
    }

  }

  private static String getRating(Facility facility) {
    String facilityName = facility.getName();
    double avgRating = facility.getRating().getAverageRating();

    String response = String.format(
        "status:SUCCESS\n" +
            "facility:%s\n" +
            "rating:%.1f",
        facilityName, avgRating);

    return response;
  }

  private static RequestMessage addRating(RequestMessage requestMessage, InetAddress userAddress, int userPort) {
    String[] requestString = requestMessage.getData().split(",");
    String facilityName = requestString[1];
    Facility facility = facilityFactory.getFacility(facilityName);
    double rating = Double.parseDouble(requestString[2]);

    facility.addRating(rating);
    String userInfo = userAddress.toString() + ":" + userPort;

    RequestMessage responseMessage = new RequestMessage(
        Operation.UPDATE.getOpCode(),
        requestMessage.getRequestID(),
        String.format(
            "status:SUCCESS\n" +
                "user:%s\n" +
                "facility:%s\n" +
                "rating:%.1f",
            userInfo, facilityName, rating));
    return responseMessage;
  }

  // facilityName cannot be null
  private static String formatFacilityAvailability(String facilityName) {
    // return specified facility information
    System.out.println("handleRequest: Requested facility: " + facilityName);
    Facility requestedFacility = facilityFactory.getFacility(facilityName);
    if (requestedFacility != null) {
      // Get available timeslots for the requested facility
      Availability availability = requestedFacility.getAvailability();
      String availableTimeslots = "";
      for (DayOfWeek day : DayOfWeek.values()) {
        List<TimeSlot> timeslots = availability.getAvailableTimeSlots(day);
        if (!timeslots.isEmpty()) {
          availableTimeslots += day.toString() + ":";
          for (TimeSlot slot : timeslots) {
            availableTimeslots += slot.toString() + ",";
          }
          availableTimeslots += "\n";
        }
      }
      return String.format(
          "status:SUCCESS\n" +
              "facility:%s\n" +
              "availableTimeslots:\n%s",
          facilityName, availableTimeslots);
    } else {
      return "status:ERROR\nmessage:Facility not found";
    }
  }
}
