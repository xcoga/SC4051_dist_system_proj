package Server.utils;
import java.time.DayOfWeek;
import Server.models.BookingDetails;

public class Parser{

    public static BookingDetails parseBookingDetails(String bookingDetails){
        String[] details = bookingDetails.split(",");
        return new BookingDetails(details[0], DayOfWeek.valueOf(details[1]), Integer.parseInt(details[2]), Integer.parseInt(details[3]), Integer.parseInt(details[4]), Integer.parseInt(details[5]));
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
    public static BookingDetails[] parseUpdateBookingDetails(String bookingDetailsStr) {
        if (bookingDetailsStr == null || bookingDetailsStr.trim().isEmpty()) {
            throw new IllegalArgumentException("Booking details string cannot be null or empty");
        }
        
        // Split the input string by commas
        String[] parts = bookingDetailsStr.split(",");
        
        // Check if we have the correct number of elements
        if (parts.length != 11) {
            throw new IllegalArgumentException(
                "Invalid booking details format. Expected 11 comma-separated values, found: " + parts.length);
        }
        
        // Extract the facility name
        String facilityName = parts[0];
        
        try {
            // Create array to hold both booking details objects
            BookingDetails[] bookings = new BookingDetails[2];
            
            // Parse the previous booking details (indices 1-5)
            bookings[0] = new BookingDetails(
                facilityName,
                DayOfWeek.valueOf(parts[1].toUpperCase()),
                Integer.parseInt(parts[2]),
                Integer.parseInt(parts[3]),
                Integer.parseInt(parts[4]),
                Integer.parseInt(parts[5])
            );
            
            // Parse the new booking details (indices 6-10)
            bookings[1] = new BookingDetails(
                facilityName,
                DayOfWeek.valueOf(parts[6].toUpperCase()),
                Integer.parseInt(parts[7]),
                Integer.parseInt(parts[8]),
                Integer.parseInt(parts[9]),
                Integer.parseInt(parts[10])
            );
            
            return bookings;
        } catch (Exception e) {
            throw new IllegalArgumentException("Error parsing booking details: " + e.getMessage(), e);
        }
    }
} 