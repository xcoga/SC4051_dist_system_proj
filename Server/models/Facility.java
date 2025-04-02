package Server.models;

/**
 * Facility class represents a facility with a name, availability, and rating.
 * It provides methods to manage availability and ratings for the facility.
 */
public class Facility {
  private String name; // Name of the facility
  private Availability availability; // Availability of the facility
  private Rating rating; // Rating of the facility

  /**
   * Constructor to initialize a Facility object with a name.
   * 
   * @param name The name of the facility.
   */
  public Facility(String name) {
    this.name = name;
    this.availability = new Availability();
    this.rating = new Rating();
  }

  /**
   * Convert the Facility object to a string representation.
   * 
   * @return A string containing the facility name, availability, and rating.
   */
  public String toString() {
    return "Facility: " + this.name + ", " + this.availability.toString() + ", " + this.rating.toString();
  }

  /**
   * Get the name of the facility.
   * 
   * @return The facility name.
   */
  public String getName() {
    return this.name;
  }

  /**
   * Set the availability of the facility.
   * 
   * @param availability The Availability object to set.
   */
  public void setAvailability(Availability availability) {
    this.availability = availability;
  }

  /**
   * Get the availability of the facility.
   * 
   * @return The Availability object.
   */
  public Availability getAvailability() {
    return this.availability;
  }

  /**
   * Get the rating of the facility.
   * 
   * @return The Rating object.
   */
  public Rating getRating() {
    return this.rating;
  }

  /**
   * Set the rating of the facility.
   * 
   * @param rating The Rating object to set.
   */
  public void setRating(Rating rating) {
    this.rating = rating;
  }

  /**
   * Add a new rating to the facility.
   * 
   * @param rating The rating to add.
   */
  public void addRating(double rating) {
    this.rating.addRating(rating);
  }
}