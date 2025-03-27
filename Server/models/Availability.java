package Server.models;

import java.time.DayOfWeek;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

public class Availability {
  private boolean[] days;
  private int[] startHour;
  private int[] startMinute;
  private int[] endHour;
  private int[] endMinute;
  private List<Booking>[] bookedSlots;
  private String facilityName;

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

  public String bookTimeSlot(String userInfo, TimeSlot timeSlot) {
    int dayIndex = timeSlot.day.getValue() - 1;
    String confirmationID = UUID.randomUUID().toString();
    this.bookedSlots[dayIndex]
        .add(new Booking(confirmationID, userInfo, this.facilityName, timeSlot));
    return confirmationID;
  }

  // public String removeTimeSlot(String prev_bookingId, String userInfo){
  // for (TimeSlot slot : this.bookedSlots[dayIndex]) {
  // if (slot.confirmationID.equals(prev_bookingId) &&
  // slot.userInfo.equals(userInfo)) {
  // this.bookedSlots[dayIndex].remove(slot);
  // return slot.confirmationID;
  // }
  // }
  // return null;
  // }

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

  public List<TimeSlot> getAvailableTimeSlots(DayOfWeek day) {
    int dayIndex = day.getValue() - 1;
    List<TimeSlot> availableSlots = new ArrayList<>();

    if (!this.days[dayIndex]) {
      return availableSlots; // Return empty list if the facility is closed on this day
    }

    int currentStartHour = this.startHour[dayIndex];
    int currentStartMinute = this.startMinute[dayIndex];

    for (Booking bookedSlot : this.bookedSlots[dayIndex]) {
      // Add available slot before the booked slot
      if (currentStartHour < bookedSlot.timeSlot.startHour ||
          (currentStartHour == bookedSlot.timeSlot.startHour && currentStartMinute < bookedSlot.timeSlot.startMinute)) {
        availableSlots
            .add(new TimeSlot(currentStartHour, currentStartMinute, bookedSlot.timeSlot.startHour,
                bookedSlot.timeSlot.startMinute, day));
      }
      // Update the start time to the end of the booked slot
      currentStartHour = bookedSlot.timeSlot.endHour;
      currentStartMinute = bookedSlot.timeSlot.endMinute;
    }

    // Add the remaining available slot after the last booked slot
    if (currentStartHour < this.endHour[dayIndex] ||
        (currentStartHour == this.endHour[dayIndex] && currentStartMinute < this.endMinute[dayIndex])) {
      availableSlots
          .add(new TimeSlot(currentStartHour, currentStartMinute, this.endHour[dayIndex], this.endMinute[dayIndex],
              day));
    }

    return availableSlots;
  }
}
