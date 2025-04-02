package Server;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.time.DayOfWeek;
import java.util.HashMap;
import java.util.List;

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

/**
 * Server class to handle client requests and manage facility bookings.
 * Implements a UDP server with at-most-once and at-least-once invocation
 * semantics.
 */
public class Server {
  // Probability of dropping a request (used for testing reliability)
  private static final double DROP_CHANCE = 0.4;

  // Service classes for managing facilities, request history, and monitoring
  private static MonitorService facilityMonitorService;
  private static RequestHistory requestHistory;
  private static FacilityFactory facilityFactory;

  // Flag to determine invocation semantics: true for at-most-once, false for
  // at-least-once
  private static boolean checkHistory = true;

  // UDP socket for communication
  private static DatagramSocket aSocket = null;

  // Tracks the last updated facility for monitoring purposes
  private static String lastUpdatedFacility = null;

  public static void main(String[] args) {
    // Determine invocation semantics based on command-line arguments
    String invocationScematic = args.length > 0 ? args[0] : "default";

    if (invocationScematic.equals("at-least-once")) {
      System.out.println("Server started with at-least-once invocation semantics.");
      checkHistory = false;
    } else {
      System.out.println("Server started with at-most-once invocation semantics.");
    }

    // Initialize service classes and facilities
    init();

    try {
      // Bind the UDP socket to port 6789
      aSocket = new DatagramSocket(6789);
      byte[] buffer = new byte[1000]; // Buffer for receiving data

      System.out.println("UDP Server is running on port 6789...");
      while (true) {
        // Receive a request from a client
        DatagramPacket request = new DatagramPacket(buffer, buffer.length);
        aSocket.receive(request); // Blocks until a packet is received

        System.out.println("\n\nReceived request from: " + request.getAddress() + ":" + request.getPort());

        // Simulate dropping requests based on DROP_CHANCE
        boolean dropRequest = Math.random() < DROP_CHANCE;
        if (dropRequest) {
          System.out.println("Dropping request from " + request.getAddress() + ":" + request.getPort());
          continue; // Skip processing this request
        }

        // Handle the request and generate a response
        RequestMessage responseMessage = handleRequest(request);
        System.out.println("Current request history: " + requestHistory.toString());

        // Send the response back to the client
        try {
          byte[] replybuff = Serializer.serialize(responseMessage);

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
      // Ensure the socket is closed when the server shuts down
      if (aSocket != null && !aSocket.isClosed()) {
        aSocket.close();
        System.out.println("Server socket closed.");
      }
    }
  }

  /**
   * Initializes the service classes and creates initial facilities with
   * availability.
   */
  private static void init() {
    facilityMonitorService = new MonitorService();
    requestHistory = new RequestHistory();
    facilityFactory = new FacilityFactory();

    // Create some facilities by default at the start
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
   * Handles incoming client requests and processes them based on the operation
   * type.
   * 
   * @param request DatagramPacket containing the client's request.
   * @return RequestMessage response to be sent back to the client.
   */
  public static RequestMessage handleRequest(DatagramPacket request) {
    RequestMessage requestMessage = null;
    RequestMessage responseMessage = null;
    lastUpdatedFacility = null;

    // Simulate processing delay to mimic real-world server behavior
    try {
      int processingDelay = 3000 + (int) (Math.random() * 5000);
      System.out.println("Simulating slow processing for " + processingDelay + "ms");
      Thread.sleep(processingDelay);
    } catch (InterruptedException e) {
      System.err.println("Sleep interrupted: " + e.getMessage());
    }

    // Deserialize the request message
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

    // Handle ECHO operation (no processing required)
    if (requestMessage.getOperation() == Operation.ECHO) {
      System.out.println("Echo request received");
      return requestMessage;
    }

    // Check request history for at-most-once semantics
    if (checkHistory) {
      System.out.println("Checking request history...");
      RequestInfo requestInformation = new RequestInfo(
          requestMessage,
          request.getAddress(),
          request.getPort(),
          null);
      RequestInfo prevRequest = requestHistory.containsRequest(requestInformation);
      if (prevRequest != null) {
        System.out.printf("Duplicate %s request id %d detected\n", requestMessage.getOperation(),
            requestMessage.getRequestID());
        responseMessage = prevRequest.responseMessage;
        return responseMessage;
      }
    }

    // Process the request based on its operation type
    switch (requestMessage.getOperation()) {
      case READ:
        // Handle READ operations (e.g., facility info, booking info, ratings)
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
              // requestString[1] is the facility name
              // if have more, can use a hashmap to store the days requested
              // example requests format: facility,facilityName,day1,day2,...
              // if no days specified, return all days
              // else return only those days
              HashMap<DayOfWeek, String> days = new HashMap<>();
              for (int i = 2; i < requestString.length; i++) {
                days.put(DayOfWeek.valueOf(requestString[i]), "true");
              }
              System.out.println("Days: " + days.toString());

              responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                  formatFacilityAvailability(requestString[1], days));
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
        // Handle WRITE operations (e.g., book a facility)
        // For book facility request, format:
        // facilityName,day,startHour,startMinute,endHour,endMinute
        try {
          responseMessage = bookFacility(requestMessage, request.getAddress(), request.getPort());
        } catch (Exception e) {
          responseMessage = new RequestMessage(Operation.WRITE.getOpCode(), requestMessage.getRequestID(),
              "status:ERROR\nmessage:" + e.getMessage());
        }

        break;

      case UPDATE:
        // Handle UPDATE operations (e.g., update booking, add rating)
        // UPDATE has 2 cases: 1. Update booking 2. Add a rating
        // 1. Update Booking, Expected format:
        // "book,<prevBookingID>,<facilityName>,<Day>,<startHour>,<startMinute>,<endHour>,<endMinute>"
        // 2. Add Rating, Expected format: "rating",facilityName,rating
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
        // Handle DELETE operations (e.g., delete booking)
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
        // Handle MONITOR operations (e.g., register for monitoring updates)
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

            // CHECK if the facilityName provided by user is correct.
            if (facilityFactory.getFacility(facilityName) == null) {
              responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                  "status:ERROR\nmessage:Invalid facilityName provided");
              break;
            }

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
                    facilityName, monitorInterval));
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

    // Save the request and response to the history for at-most-once semantics
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

        String notification = formatFacilityAvailability(lastUpdatedFacility, null);
        if (!(notification == null || notification.isEmpty() || notification.contains("status:ERROR"))) {
          facilityMonitorService.notifyAll(aSocket, lastUpdatedFacility, notification);
        }
      }
    } catch (Exception e) {
      System.err.println("Error notifying clients: " + e.getMessage());
    }

    return responseMessage;
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

    // Check if the timeslot to update is available
    Availability availability = facility.getAvailability();
    if (availability == null || !availability.isAvailable(newTimeSlot)) {
      responseMessage = new RequestMessage(
          Operation.WRITE.getOpCode(),
          requestMessage.getRequestID(),
          "status:ERROR\nmessage:Facility not available at the requested time");
      return responseMessage;
    }

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
              newTimeSlot.getEndTime()));

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
  private static String formatFacilityAvailability(String facilityName, HashMap<DayOfWeek, String> days) {
    // return specified facility information
    System.out.println("handleRequest: Requested facility: " + facilityName);
    Facility requestedFacility = facilityFactory.getFacility(facilityName);
    if (requestedFacility != null) {
      // Get available timeslots for the requested facility
      Availability availability = requestedFacility.getAvailability();
      String availableTimeslots = "";
      for (DayOfWeek day : DayOfWeek.values()) {
        // if days not specified, get all days
        // else if days specified, only get those days
        if (!(days == null || days.isEmpty()) && days.get(day) == null) {
          continue;
        }

        // Debug message
        // System.out.println("Checking availability for day: " + day);

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
