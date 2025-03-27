package Server.models;

import java.time.DayOfWeek;

public class TimeSlot {
  int startHour;
  int startMinute;
  int endHour;
  int endMinute;
  DayOfWeek day;

  public TimeSlot(int startHour, int startMinute, int endHour, int endMinute, DayOfWeek day) {
    this.startHour = startHour;
    this.startMinute = startMinute;
    this.endHour = endHour;
    this.endMinute = endMinute;
    this.day = day;
  }

  public DayOfWeek getDay() {
    return this.day;
  }

  boolean conflictsWith(TimeSlot timeSlot) {
    return this.conflictsWith(timeSlot.startHour, timeSlot.startMinute, timeSlot.endHour, timeSlot.endMinute);
  }

  boolean conflictsWith(int startHour, int startMinute, int endHour, int endMinute) {
    if (this.endHour < startHour || (this.endHour == startHour && this.endMinute <= startMinute)) {
      return false;
    }
    if (this.startHour > endHour || (this.startHour == endHour && this.startMinute >= endMinute)) {
      return false;
    }
    return true;
  }

  @Override
  public String toString() {
    return String.format("%02d%02d - %02d%02d", this.startHour, this.startMinute, this.endHour, this.endMinute);
  }

  public String getDayTimeString() {
    return String.format("day:%s\n%02d%02d - %02d%02d", this.day.toString(), this.startHour, this.startMinute,
        this.endHour, this.endMinute);
  }

  public String getStartTime(){
    return String.format("%02d%02d", this.startHour, this.startMinute);
  }

  public String getEndTime(){
    return String.format("%02d%02d", this.endHour, this.endMinute);
  }
}
