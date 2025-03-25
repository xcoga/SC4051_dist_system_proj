package Server;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.time.DayOfWeek;
import java.util.List;
import java.util.UUID;

import Server.models.Availability;
import Server.models.Facility;
import Server.models.BookingDetails;
import Server.services.FacilityFactory;
import Server.services.MonitorService;
import Server.services.RequestInfo;
import Server.services.RequestHistory;
import Server.utils.Serializer;
import Server.utils.Parser;


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
      byte[] receivedData = new byte[request.getLength()];
      System.arraycopy(request.getData(), request.getOffset(), receivedData, 0, request.getLength());
      requestMessage = (RequestMessage) Serializer.deserialize(receivedData);
      System.out.println("Request: " + requestMessage.toString());
    } catch (Exception e) {
      System.err.println("Error deserializing request: " + e.getMessage());

      responseMessage = new RequestMessage(Operation.READ.getOpCode(), 0, "status: ERROR\nmessage: bad request");
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
        // example requests format: facility,Weekday1
        String[] requestString = requestMessage.getData().split(",");
        if (requestString == null || requestString.length == 1) {
          responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
              "status: ERROR\nmessage: invalid request format");
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
                  "status: SUCCESS\n" + facilityNames);
              break;
            default:
              // return specified facility information
              System.out.println("handleRequest: Requested facility: " + requestString[1]);
              Facility requestedFacility = facilityFactory.getFacility(requestString[1]);
              if (requestedFacility != null) {
                // Get available timeslots for the requested facility
                Availability availability = requestedFacility.getAvailability();
                String availableTimeslots = "Available timeslots: ";
                for (DayOfWeek day : DayOfWeek.values()) {
                  List<Availability.TimeSlot> timeslots = availability.getAvailableTimeSlots(day);
                  if (!timeslots.isEmpty()) {
                    availableTimeslots += day.toString() + ": ";
                    for (Availability.TimeSlot slot : timeslots) {
                      availableTimeslots += slot.toString() + ", ";
                    }
                  }
                }
                responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                    "status: SUCCESS\n" + availableTimeslots);
              } else {
                responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                    "status: ERROR\nmessage: facility not found");
              }
              break;
          }
        } else if (requestString[0].equals("booking")) {
          // read booking information based on booking id string in request
          // example requests format: booking,booking_id
          String bookingID = requestString[1];
          for (Facility facility : facilityFactory.getFacilities()) {
            Availability availability = facility.getAvailability();
            if (availability != null) {
              String bookingInfo = availability.getBookingInfo(bookingID).toString();
              if (bookingInfo != null) {
                responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                    "status: SUCCESS\n" + bookingInfo);
                break;
              }
            }
          }
          if (responseMessage == null) {
            responseMessage = new RequestMessage(Operation.READ.getOpCode(), requestMessage.getRequestID(),
                "status: ERROR\nmessage: booking not found");
          }
        }
        break;


      case WRITE:
        try {
          // Parse booking details using the Parser class
          BookingDetails booking = Parser.parseBookingDetails(requestMessage.getData());
          
          // Book the facility using the parsed details
          String bookingResult = bookFacility(
              booking.getFacilityName(), 
              booking.getDay(), 
              booking.getStartHour(), 
              booking.getStartMinute(), 
              booking.getEndHour(), 
              booking.getEndMinute(),
              request.getAddress(), 
              request.getPort()
          );
          
          responseMessage = new RequestMessage(
              Operation.WRITE.getOpCode(), 
              requestMessage.getRequestID(), 
              bookingResult
          );
          } catch (Exception e) {
          responseMessage = new RequestMessage(
              Operation.WRITE.getOpCode(), 
              requestMessage.getRequestID(),
              "status: ERROR\nmessage: " + e.getMessage()
          );
        }
        break;


      case UPDATE:

        // Extract booking details from the request message.
        // 1. Booking details to delete
        // 2. Booking details to add
        try {
            // Parse the booking details string into previous and new BookingDetails objects
            BookingDetails[] bookings = Parser.parseUpdateBookingDetails(requestMessage.getData());
            BookingDetails previousBooking = bookings[0];
            BookingDetails newBooking = bookings[1];
            
            // Update the booking for the facility
            String bookingResult = updateBooking(
                previousBooking.getFacilityName(),
                previousBooking.getDay(), 
                previousBooking.getStartHour(), 
                previousBooking.getStartMinute(), 
                previousBooking.getEndHour(), 
                previousBooking.getEndMinute(),
                newBooking.getDay(), 
                newBooking.getStartHour(), 
                newBooking.getStartMinute(), 
                newBooking.getEndHour(), 
                newBooking.getEndMinute(),
                request.getAddress(),
                request.getPort()
            );
            
            responseMessage = new RequestMessage(
                Operation.UPDATE.getOpCode(),
                requestMessage.getRequestID(),
                bookingResult
            );
        } catch (IllegalArgumentException e) {
            responseMessage = new RequestMessage(
                Operation.UPDATE.getOpCode(),
                requestMessage.getRequestID(),
                "status: ERROR\nmessage: " + e.getMessage()
            );
        }
        break;

      case DELETE:
        responseMessage = new RequestMessage(Operation.DELETE.getOpCode(), requestMessage.getRequestID(),
            "status: ERROR\nmessage: unimplemented operation");
        break;
      case MONITOR:
        responseMessage = new RequestMessage(Operation.MONITOR.getOpCode(), requestMessage.getRequestID(),
            "status: ERROR\nmessage: unimplemented operation");
        break;
      default:
        responseMessage = new RequestMessage(Operation.NONE.getOpCode(), requestMessage.getRequestID(),
            "status: ERROR\nmessage: unknown operation");
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




  private static String bookFacility(String facilityName, DayOfWeek day, int startHour, int startMinute, int endHour,
      int endMinute, InetAddress userAddress, int userPort) {
    Facility facility = facilityFactory.getFacility(facilityName);
    if (facility == null) {
      return "status: ERROR\nmessage: facility not found";
    }

    Availability availability = facility.getAvailability();
    if (availability == null || !availability.isAvailable(day, startHour, startMinute, endHour, endMinute)) {
      return "status: ERROR\nmessage: facility not available at the requested time";
    }

    // Capture user information
    String userInfo = userAddress.toString() + ":" + userPort;

    // Book the facility and get the confirmation ID
    String confirmationID = availability.bookTimeSlot(day, startHour, startMinute, endHour, endMinute, userInfo);

    return "status: SUCCESS\nBooking_ID: " + confirmationID + " by " + userInfo;
  }





  //Update booking. 1. Remove the previous booking 2. Add the new booking
  private static String updateBooking(String facilityName, DayOfWeek prev_day, int prev_startHour, int prev_startMinute, int prev_endHour,
      int prev_endMinute, DayOfWeek new_day, int new_startHour, int new_startMinute, int new_endHour,
      int new_endMinute, InetAddress userAddress, int userPort){

    Facility facility = facilityFactory.getFacility(facilityName);
    if (facility == null) {
      return "status: ERROR\nmessage: facility not found";
    }

    Availability availability = facility.getAvailability();

    // Capture user information
    String userInfo = userAddress.toString() + ":" + userPort;

    // Book the facility and get the confirmation ID
    try{
      String delete_confirmationID = availability.removeTimeSlot(prev_day, prev_startHour, prev_startMinute, prev_endHour, prev_endMinute, userInfo);
      String booking_confirmationID = availability.bookTimeSlot(new_day, new_startHour, new_startMinute, new_endHour, new_endMinute, userInfo);

      return "status: SUCCESS\nBooking_ID: " + booking_confirmationID + " by " + userInfo;
    } catch (Exception e) {
      return "status: ERROR\nmessage: booking not successful or booking not found"  + " by " + userInfo; 
    }
  }





  private static String deleteBooking(String facilityName, DayOfWeek day, int startHour, int startMinute, int endHour,
      int endMinute, InetAddress userAddress, int userPort) {

    Facility facility = facilityFactory.getFacility(facilityName);
    if (facility == null) {
      return "status: ERROR\nmessage: facility not found";
    }

    Availability availability = facility.getAvailability();

    // Capture user information
    String userInfo = userAddress.toString() + ":" + userPort;


    try{
      //Remove the booking for that facility and get the confirmation ID
      String confirmationID = availability.removeTimeSlot(day, startHour, startMinute, endHour, endMinute, userInfo);
      return "status: SUCCESS\nBooking_ID: " + confirmationID + " by " + userInfo;
    } catch (Exception e) {
      return "status: ERROR\nmessage: booking not found";
    }
    
  }





}
