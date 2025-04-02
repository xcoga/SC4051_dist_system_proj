package Server.models;

import java.time.DayOfWeek;

/**
 * Booking class represents a confirmed booking for a facility.
 * It includes the confirmation ID, user information, facility name, and time
 * slot.
 */
public class Booking {
  public String confirmationID; // Unique confirmation ID for the booking
  public String userInfo; // Information about the user who made the booking
  public String facilityName; // Name of the booked facility
  public TimeSlot timeSlot; // Time slot for the booking

  /**
   * Constructor to initialize a Booking object.
   * 
   * @param confirmationID The unique confirmation ID for the booking.
   * @param userInfo       Information about the user.
   * @param facilityName   The name of the booked facility.
   * @param timeSlot       The time slot for the booking.
   */
  Booking(String confirmationID, String userInfo, String facilityName, TimeSlot timeSlot) {
    this.confirmationID = confirmationID;
    this.userInfo = userInfo;
    this.facilityName = facilityName;
    this.timeSlot = timeSlot;
  }

  /**
   * Get the day of the week for the booking.
   * 
   * @return The day of the week.
   */
  public DayOfWeek getDay() {
    return this.timeSlot.getDay();
  }

  /**
   * Check if this booking conflicts with a given time slot.
   * 
   * @param timeSlot The time slot to check against.
   * @return True if the booking conflicts, false otherwise.
   */
  public boolean conflictsWith(TimeSlot timeSlot) {
    return this.timeSlot.conflictsWith(timeSlot);
  }

  /**
   * Convert the Booking object to a string representation.
   * 
   * @return A string containing the booking details.
   */
  @Override
  public String toString() {
    return String.format("bookingID:%s\nuser:%s\nfacility:%s\n%s\n", this.confirmationID, this.userInfo,
        this.facilityName, this.timeSlot.getDayTimeString());
  }
}
