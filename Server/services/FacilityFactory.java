package Server.services;

import java.util.ArrayList;

import Server.models.Facility;

/**
 * FacilityFactory class is responsible for creating and managing facilities.
 * It provides methods to create new facilities, retrieve existing facilities,
 * and get all facilities.
 */
public class FacilityFactory {
  private ArrayList<Facility> facilities; // List of all facilities

  /**
   * Constructor to initialize the FacilityFactory object.
   */
  public FacilityFactory() {
    this.facilities = new ArrayList<Facility>();
  }

  /**
   * Get the list of all facilities.
   * 
   * @return A list of Facility objects.
   */
  public ArrayList<Facility> getFacilities() {
    if (facilities == null) {
      facilities = new ArrayList<Facility>();
    }
    return facilities;
  }

  /**
   * Create a new facility and add it to the list.
   * 
   * @param name The name of the new facility.
   * @return The newly created Facility object.
   */
  public Facility newFacility(String name) {
    Facility newFacility = new Facility(name);
    if (facilities == null) {
      facilities = new ArrayList<Facility>();
    }
    facilities.add(newFacility);
    return newFacility;
  }

  /**
   * Retrieve a facility by its name.
   * 
   * @param name The name of the facility to retrieve.
   * @return The Facility object if found, null otherwise.
   */
  public Facility getFacility(String name) {
    for (Facility facility : getFacilities()) {
      if (facility.getName().equals(name)) {
        return facility;
      }
    }
    return null;
  }
}
