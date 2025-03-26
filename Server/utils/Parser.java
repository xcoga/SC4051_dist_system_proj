package Server.utils;
import java.time.DayOfWeek;
import Server.models.BookingDetails;

public class Parser{

    public static BookingDetails parseBookingDetails(String bookingDetails){
        String[] details = bookingDetails.split(",");
        return new BookingDetails(details[0], DayOfWeek.valueOf(details[1]), Integer.parseInt(details[2]), Integer.parseInt(details[3]), Integer.parseInt(details[4]), Integer.parseInt(details[5]));
    }


    public static String parseUpdateType(String updateType){
        String[] parts = updateType.split(",");

        return parts[0];
    }



    /**
     * Parses a comma-separated string of booking details into two BookingDetails objects:
     * one for the previous booking and one for the new booking.
     * 
     * Expected format: facilityName,day1,startHour1,startMin1,endHour1,endMin1,day2,startHour2,startMin2,endHour2,endMin2
     * 
     * @param bookingDetailsStr The comma-separated string containing booking details
     * @return An array of BookingDetails objects: [previousBooking, newBooking]
     * @throws IllegalArgumentException If the input string is invalid or cannot be parsed
     */
    public static BookingDetails parseUpdateBookingDetails(String bookingDetailsStr) {
        if (bookingDetailsStr == null || bookingDetailsStr.trim().isEmpty()) {
            throw new IllegalArgumentException("Booking details string cannot be null or empty");
        }
        
        // Split the input string by commas
        String[] parts = bookingDetailsStr.split(",");
        
        // Check if we have the correct number of elements
        if (parts.length != 8) {
            throw new IllegalArgumentException(
                "Invalid booking details format. Expected 12 comma-separated values, found: " + parts.length);
        }
        

        
        try {
            
            // Parse the previous booking details (indices 1-5)
            BookingDetails booking = new BookingDetails(
                parts[1], //Previous bookingID
                parts[2], //Facility name
                DayOfWeek.valueOf(parts[3].toUpperCase()),
                Integer.parseInt(parts[4]),
                Integer.parseInt(parts[5]),
                Integer.parseInt(parts[6]),
                Integer.parseInt(parts[7])
            );
            
            return booking;
        } catch (Exception e) {
            throw new IllegalArgumentException("Error parsing booking details: " + e.getMessage(), e);
        }
    }
} 