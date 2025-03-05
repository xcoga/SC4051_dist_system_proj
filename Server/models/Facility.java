package Server.models;

public class Facility {
  private String name;
  private Avaliablility avaliablility;

  public Facility(String name) {
    this.name = name;
    this.avaliablility = new Avaliablility();
  }

  public String toString() {
    return "Facility name: " + this.name + ", " + this.avaliablility.toString();
  }
  
  public String getName() {
    return this.name;
  }

  public void setAvaliablility(Avaliablility avaliablility) {
    this.avaliablility = avaliablility;
  }

  public Avaliablility getAvaliablility() {
    return this.avaliablility;
  }
}
