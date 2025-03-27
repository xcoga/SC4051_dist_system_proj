package Server.models;

import java.time.DayOfWeek;

public class Booking {
  public String confirmationID;
  public String userInfo;
  public String facilityName;
  public TimeSlot timeSlot;

  Booking(String confirmationID, String userInfo, String facilityName, TimeSlot timeSlot) {
    this.confirmationID = confirmationID;
    this.userInfo = userInfo;
    this.facilityName = facilityName;
    this.timeSlot = timeSlot;
  }

  public DayOfWeek getDay() {
    return this.timeSlot.getDay();
  }

  public boolean conflictsWith(TimeSlot timeSlot) {
    return this.timeSlot.conflictsWith(timeSlot);
  }

  @Override
  public String toString() {
    return String.format("bookingID:%s\nuser:%s\nfacility:%s\n%s\n", this.confirmationID, this.userInfo,
        this.facilityName, this.timeSlot.getDayTimeString());
  }
}
