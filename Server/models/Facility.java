package Server.models;

public class Facility {
  private String name;
  private Availability availability;

  public Facility(String name) {
    this.name = name;
    this.availability = new Availability();
  }

  public String toString() {
    return "Facility: " + this.name + ", " + this.availability.toString();
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
}
