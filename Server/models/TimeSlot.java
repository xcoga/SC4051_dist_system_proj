package Server.models;

import java.time.DayOfWeek;

/**
 * TimeSlot class represents a specific time interval on a particular day.
 * It is used to define availability or booking times for facilities.
 */
public class TimeSlot {
  int startHour; // Start hour of the time slot
  int startMinute; // Start minute of the time slot
  int endHour; // End hour of the time slot
  int endMinute; // End minute of the time slot
  DayOfWeek day; // Day of the week for the time slot

  /**
   * Constructor to initialize a TimeSlot object.
   * 
   * @param startHour   Start hour of the time slot.
   * @param startMinute Start minute of the time slot.
   * @param endHour     End hour of the time slot.
   * @param endMinute   End minute of the time slot.
   * @param day         Day of the week for the time slot.
   */
  public TimeSlot(int startHour, int startMinute, int endHour, int endMinute, DayOfWeek day) {
    this.startHour = startHour;
    this.startMinute = startMinute;
    this.endHour = endHour;
    this.endMinute = endMinute;
    this.day = day;
  }

  /**
   * Get the day of the week for the time slot.
   * 
   * @return The day of the week.
   */
  public DayOfWeek getDay() {
    return this.day;
  }

  /**
   * Check if this time slot conflicts with another time slot.
   * 
   * @param timeSlot The other time slot to check against.
   * @return True if the time slots conflict, false otherwise.
   */
  boolean conflictsWith(TimeSlot timeSlot) {
    return this.conflictsWith(timeSlot.startHour, timeSlot.startMinute, timeSlot.endHour, timeSlot.endMinute);
  }

  /**
   * Check if this time slot conflicts with a specified time range.
   * 
   * @param startHour   Start hour of the other time range.
   * @param startMinute Start minute of the other time range.
   * @param endHour     End hour of the other time range.
   * @param endMinute   End minute of the other time range.
   * @return True if the time ranges conflict, false otherwise.
   */
  boolean conflictsWith(int startHour, int startMinute, int endHour, int endMinute) {
    if (this.endHour < startHour || (this.endHour == startHour && this.endMinute <= startMinute)) {
      return false;
    }
    if (this.startHour > endHour || (this.startHour == endHour && this.startMinute >= endMinute)) {
      return false;
    }
    return true;
  }

  /**
   * Convert the TimeSlot object to a string representation.
   * 
   * @return A string in the format "HHMM - HHMM".
   */
  @Override
  public String toString() {
    return String.format("%02d%02d - %02d%02d", this.startHour, this.startMinute, this.endHour, this.endMinute);
  }

  /**
   * Get a string representation of the day and time range.
   * 
   * @return A string in the format "day:DAY\nHHMM - HHMM".
   */
  public String getDayTimeString() {
    return String.format("day:%s\n%02d%02d - %02d%02d", this.day.toString(), this.startHour, this.startMinute,
        this.endHour, this.endMinute);
  }

  /**
   * Get the start time of the time slot as a string.
   * 
   * @return The start time in the format "HHMM".
   */
  public String getStartTime() {
    return String.format("%02d%02d", this.startHour, this.startMinute);
  }

  /**
   * Get the end time of the time slot as a string.
   * 
   * @return The end time in the format "HHMM".
   */
  public String getEndTime() {
    return String.format("%02d%02d", this.endHour, this.endMinute);
  }
}
