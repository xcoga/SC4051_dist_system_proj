package Server.services;

import java.util.ArrayList;

import Server.models.Facility;

public class FacilityFactory {
  private ArrayList<Facility> facilities;

  public FacilityFactory() {
    this.facilities = new ArrayList<Facility>();
  }

  public ArrayList<Facility> getFacilities() {
    if (facilities == null) {
      facilities = new ArrayList<Facility> ();
    }
    return facilities;
  }

  public Facility newFacility(String name) {
    Facility newFacility = new Facility(name);
    if (facilities == null) {
      facilities = new ArrayList<Facility> ();
    }
    facilities.add(newFacility);
    return newFacility;
  }

  public Facility getFacility(String name) {
    for (Facility facility : getFacilities()) {
      if (facility.getName().equals(name)) {
        return facility;
      }
    }
    return null;
  }

}
