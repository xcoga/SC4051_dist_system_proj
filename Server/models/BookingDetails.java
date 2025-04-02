package Server.models;

import java.time.DayOfWeek;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

/**
 * BookingDetails class represents the details of a booking request.
 * It includes the facility name, day, time range, and optionally a previous
 * booking ID for updates.
 */
public class BookingDetails {
    String facilityName; // Name of the facility
    private DayOfWeek day; // Day of the booking
    private int startHour; // Start hour of the booking
    private int startMinute; // Start minute of the booking
    private int endHour; // End hour of the booking
    private int endMinute; // End minute of the booking
    private String prev_bookingId; // Previous booking ID (used for updates)

    /**
     * Default constructor.
     */
    public BookingDetails() {
    }

    /**
     * Constructor to initialize a BookingDetails object.
     * 
     * @param facilityName The name of the facility.
     * @param day          The day of the booking.
     * @param startHour    The start hour of the booking.
     * @param startMinute  The start minute of the booking.
     * @param endHour      The end hour of the booking.
     * @param endMinute    The end minute of the booking.
     */
    public BookingDetails(String facilityName, DayOfWeek day, int startHour, int startMinute, int endHour,
            int endMinute) {
        this.facilityName = facilityName;
        this.day = day;
        this.startHour = startHour;
        this.startMinute = startMinute;
        this.endHour = endHour;
        this.endMinute = endMinute;
    }

    /**
     * Constructor for update bookings with a previous booking ID.
     * 
     * @param prev_bookingId The previous booking ID.
     * @param facilityName   The name of the facility.
     * @param day            The day of the booking.
     * @param startHour      The start hour of the booking.
     * @param startMinute    The start minute of the booking.
     * @param endHour        The end hour of the booking.
     * @param endMinute      The end minute of the booking.
     */
    public BookingDetails(String prev_bookingId, String facilityName, DayOfWeek day, int startHour, int startMinute,
            int endHour, int endMinute) {
        this.prev_bookingId = prev_bookingId;
        this.facilityName = facilityName;
        this.day = day;
        this.startHour = startHour;
        this.startMinute = startMinute;
        this.endHour = endHour;
        this.endMinute = endMinute;
    }

    /**
     * Get the name of the facility.
     * 
     * @return The facility name.
     */
    public String getFacilityName() {
        return facilityName;
    }

    public void setFacilityName(String facilityName) {
        this.facilityName = facilityName;
    }

    /**
     * Get the day of the booking.
     * 
     * @return The day of the booking.
     */
    public DayOfWeek getDay() {
        return day;
    }

    public void setDay(DayOfWeek day) {
        this.day = day;
    }

    /**
     * Get the start hour of the booking.
     * 
     * @return The start hour of the booking.
     */
    public int getStartHour() {
        return startHour;
    }

    public void setStartHour(int startHour) {
        this.startHour = startHour;
    }

    /**
     * Get the start minute of the booking.
     * 
     * @return The start minute of the booking.
     */
    public int getStartMinute() {
        return startMinute;
    }

    public void setStartMinute(int startMinute) {
        this.startMinute = startMinute;
    }

    /**
     * Get the end hour of the booking.
     * 
     * @return The end hour of the booking.
     */
    public int getEndHour() {
        return endHour;
    }

    public void setEndHour(int endHour) {
        this.endHour = endHour;
    }

    /**
     * Get the end minute of the booking.
     * 
     * @return The end minute of the booking.
     */
    public int getEndMinute() {
        return endMinute;
    }

    public void setEndMinute(int endMinute) {
        this.endMinute = endMinute;
    }

    /**
     * Convert the BookingDetails object to a string representation.
     * 
     * @return A string containing the facility name, day, and time range.
     */
    @Override
    public String toString() {
        return String.format("%s %s %02d:%02d-%02d:%02d",
                facilityName, day.toString(), startHour, startMinute, endHour, endMinute);
    }

    /**
     * Get the previous booking ID.
     * 
     * @return The previous booking ID.
     */
    public String getPrev_bookingId() {
        return prev_bookingId;
    }

    public void setPrev_bookingId(String prev_bookingId) {
        this.prev_bookingId = prev_bookingId;
    }
}