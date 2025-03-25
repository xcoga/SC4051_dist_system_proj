package Server.models;

import java.time.DayOfWeek;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

public class BookingDetails{
    String facilityName;
    private DayOfWeek day;
    private int startHour;
    private int startMinute;
    private int endHour;
    private int endMinute;
    /**
     * Default constructor
     */
    public BookingDetails() {
    }


    public BookingDetails(String facilityName, DayOfWeek day, int startHour, int startMinute, int endHour, int endMinute) {
        this.facilityName = facilityName;
        this.day = day;
        this.startHour = startHour;
        this.startMinute = startMinute;
        this.endHour = endHour;
        this.endMinute = endMinute;
    }

    public String getFacilityName() {
        return facilityName;
    }

    public void setFacilityName(String facilityName) {
        this.facilityName = facilityName;
    }

    public DayOfWeek getDay() {
        return day;
    }

    public void setDay(DayOfWeek day) {
        this.day = day;
    }

    public int getStartHour() {
        return startHour;
    }

    public void setStartHour(int startHour) {
        this.startHour = startHour;
    }

    public int getStartMinute() {
        return startMinute;
    }

    public void setStartMinute(int startMinute) {
        this.startMinute = startMinute;
    }

    public int getEndHour() {
        return endHour;
    }

    public void setEndHour(int endHour) {
        this.endHour = endHour;
    }

    public int getEndMinute() {
        return endMinute;
    }

    public void setEndMinute(int endMinute) {
        this.endMinute = endMinute;
    }
    @Override
    public String toString() {
        return String.format("%s %s %02d:%02d-%02d:%02d", 
            facilityName, day.toString(), startHour, startMinute, endHour, endMinute);
    }
}