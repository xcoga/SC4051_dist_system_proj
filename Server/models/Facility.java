package Server.models;

public class Facility {
  private String name;
  private Availability availability;
  private Rating rating;

  public Facility(String name) {
    this.name = name;
    this.availability = new Availability();
    this.rating = new Rating();
  }

  public String toString() {
    return "Facility: " + this.name + ", " + this.availability.toString() + ", " + this.rating.toString();
  }
  
  public String getName() {
    return this.name;
  }

  public void setAvailability(Availability availability) {
    this.availability = availability;
  }

  public Availability getAvailability() {
    return this.availability;
  }
  
  public Rating getRating() {
    return this.rating;
  }
  
  public void setRating(Rating rating) {
    this.rating = rating;
  }
  
  public void addRating(double rating) {
    this.rating.addRating(rating);
  }
}