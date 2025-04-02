package Server.models;

import java.time.DayOfWeek;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

/**
 * Availability class represents the availability of a facility for each day of
 * the week.
 * It also manages bookings and checks for conflicts.
 */
public class Availability {
  private boolean[] days; // Array to indicate if the facility is open on each day
  private int[] startHour; // Start hour for each day
  private int[] startMinute; // Start minute for each day
  private int[] endHour; // End hour for each day
  private int[] endMinute; // End minute for each day
  private List<Booking>[] bookedSlots; // List of booked slots for each day
  private String facilityName; // Name of the facility

  /**
   * Constructor to initialize the Availability object.
   */
  @SuppressWarnings("unchecked")
  public Availability() {
    this.days = new boolean[7];
    this.startHour = new int[7];
    this.startMinute = new int[7];
    this.endHour = new int[7];
    this.endMinute = new int[7];
    this.bookedSlots = (List<Booking>[]) new ArrayList[7];
    for (int i = 0; i < 7; i++) {
      this.bookedSlots[i] = new ArrayList<>();
    }
  }

  /**
   * Convert the Availability object to a string representation.
   * 
   * @return A string containing the availability details for each day.
   */
  public String toString() {
    String str = "Availability: ";
    for (int i = 0; i < 7; i++) {
      str += "Day " + DayOfWeek.of(i + 1).toString() + ": " + (this.days[i] ? "Open" : "Closed") + " ";
      if (this.days[i]) {
        // this is to format the time to hh:mm nicely
        str += "Start: " + (this.startHour[i] < 10 ? "0" + this.startHour[i] : this.startHour[i]) + ":" +
            (this.startMinute[i] < 10 ? "0" + this.startMinute[i] : this.startMinute[i]) + "h ";
        str += "End: " + (this.endHour[i] < 10 ? "0" + this.endHour[i] : this.endHour[i]) + ":" +
            (this.endMinute[i] < 10 ? "0" + this.endMinute[i] : this.endMinute[i]) + "h ";
      }
    }
    return str;
  }

  /**
   * Set the availability for a specific day.
   * 
   * @param day         The day of the week.
   * @param open        Whether the facility is open on this day.
   * @param startHour   Start hour of availability.
   * @param startMinute Start minute of availability.
   * @param endHour     End hour of availability.
   * @param endMinute   End minute of availability.
   */
  public void setAvaOfDay(DayOfWeek day, boolean open, int startHour, int startMinute, int endHour, int endMinute) {
    this.days[day.getValue() - 1] = open;
    this.startHour[day.getValue() - 1] = startHour;
    this.startMinute[day.getValue() - 1] = startMinute;
    this.endHour[day.getValue() - 1] = endHour;
    this.endMinute[day.getValue() - 1] = endMinute;
  }

  public boolean getAvaOfDay(DayOfWeek day) {
    return this.days[day.getValue() - 1];
  }

  public int getStartHour(DayOfWeek day) {
    return this.startHour[day.getValue() - 1];
  }

  public int getStartMinute(DayOfWeek day) {
    return this.startMinute[day.getValue() - 1];
  }

  public int getEndHour(DayOfWeek day) {
    return this.endHour[day.getValue() - 1];
  }

  public int getEndMinute(DayOfWeek day) {
    return this.endMinute[day.getValue() - 1];
  }

  /**
   * Check if a time slot is available for booking.
   * 
   * @param timeSlot The time slot to check.
   * @return True if the time slot is available, false otherwise.
   */
  public boolean isAvailable(TimeSlot timeSlot) {
    int dayIndex = timeSlot.day.getValue() - 1;
    if (!this.days[dayIndex]) {
      return false;
    }

    for (Booking slot : this.bookedSlots[dayIndex]) {
      if (slot.conflictsWith(timeSlot)) {
        return false;
      }
    }

    return true;
  }

  /**
   * Book a time slot for a user.
   * 
   * @param userInfo Information about the user.
   * @param timeSlot The time slot to book.
   * @return A unique confirmation ID for the booking.
   */
  public String bookTimeSlot(String userInfo, TimeSlot timeSlot) {
    int dayIndex = timeSlot.day.getValue() - 1;
    String confirmationID = UUID.randomUUID().toString();
    this.bookedSlots[dayIndex]
        .add(new Booking(confirmationID, userInfo, this.facilityName, timeSlot));
    return confirmationID;
  }

  /**
   * Remove a booked time slot.
   * 
   * @param prev_bookingId The confirmation ID of the booking to remove.
   * @param userInfo       Information about the user.
   * @return The confirmation ID of the removed booking, or null if not found.
   */
  public String removeTimeSlot(String prev_bookingId, String userInfo) {
    // Loop through each day
    for (int dayIndex = 0; dayIndex < this.bookedSlots.length; dayIndex++) {
      // Loop through each slot in the current day
      for (int slotIndex = 0; slotIndex < this.bookedSlots[dayIndex].size(); slotIndex++) {
        Booking slot = this.bookedSlots[dayIndex].get(slotIndex);
        if (slot.confirmationID.equals(prev_bookingId) && slot.userInfo.equals(userInfo)) {
          this.bookedSlots[dayIndex].remove(slotIndex);
          return slot.confirmationID;
        }
      }
    }
    return null;
  }

  /**
   * Get booking information for a specific confirmation ID.
   * 
   * @param confirmationID The confirmation ID of the booking.
   * @return The Booking object if found, null otherwise.
   */
  public Booking getBookingInfo(String confirmationID) {
    for (int i = 0; i < 7; i++) {
      for (Booking slot : this.bookedSlots[i]) {
        if (slot.confirmationID.equals(confirmationID)) {
          return slot;
        }
      }
    }
    return null;
  }

  /**
   * Get a list of available time slots for a specific day.
   * 
   * @param day The day of the week.
   * @return A list of available TimeSlot objects.
   */
  public List<TimeSlot> getAvailableTimeSlots(DayOfWeek day) {
    int dayIndex = day.getValue() - 1;
    List<TimeSlot> availableSlots = new ArrayList<>();

    if (!this.days[dayIndex]) {
      return availableSlots; // Return empty list if the facility is closed on this day
    }

    // Sort booked slots by start time
    List<Booking> sortedBookings = new ArrayList<>(this.bookedSlots[dayIndex]);
    sortedBookings.sort((a, b) -> {
      if (a.timeSlot.startHour != b.timeSlot.startHour) {
        return Integer.compare(a.timeSlot.startHour, b.timeSlot.startHour);
      }
      return Integer.compare(a.timeSlot.startMinute, b.timeSlot.startMinute);
    });

    int currentStartHour = this.startHour[dayIndex];
    int currentStartMinute = this.startMinute[dayIndex];

    for (Booking bookedSlot : sortedBookings) {
      // Add available slot before the booked slot
      if (currentStartHour < bookedSlot.timeSlot.startHour ||
          (currentStartHour == bookedSlot.timeSlot.startHour && currentStartMinute < bookedSlot.timeSlot.startMinute)) {
        availableSlots.add(new TimeSlot(currentStartHour, currentStartMinute,
            bookedSlot.timeSlot.startHour, bookedSlot.timeSlot.startMinute, day));
      }
      // Update the start time to the end of the booked slot
      currentStartHour = bookedSlot.timeSlot.endHour;
      currentStartMinute = bookedSlot.timeSlot.endMinute;
    }

    // Add the remaining available slot after the last booked slot
    if (currentStartHour < this.endHour[dayIndex] ||
        (currentStartHour == this.endHour[dayIndex] && currentStartMinute < this.endMinute[dayIndex])) {
      availableSlots.add(new TimeSlot(currentStartHour, currentStartMinute,
          this.endHour[dayIndex], this.endMinute[dayIndex], day));
    }

    return availableSlots;
  }
}
