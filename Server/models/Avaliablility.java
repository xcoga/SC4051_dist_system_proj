package Server.models;

import java.time.DayOfWeek;

public class Avaliablility {
  private boolean[] days;
  private int[] startHour;
  private int[] startMinute;
  private int[] endHour;
  private int[] endMinute;

  public Avaliablility() {
    this.days = new boolean[7];
    this.startHour = new int[7];
    this.startMinute = new int[7];
    this.endHour = new int[7];
    this.endMinute = new int[7];
  }

  public String toString() {
    String str = "Avaliablility: ";
    for (int i = 0; i < 7; i++) {
      str += "Day " + DayOfWeek.of(i + 1).toString() + ": " + (this.days[i] ? "Open" : "Closed") + " ";
      if (this.days[i]) {
        // this is to format the time to hh:mm nicely
        str += "Start: " + (this.startHour[i] < 10 ? "0" + this.startHour[i] : this.startHour[i]) + ":" +
            (this.startMinute[i] < 10 ? "0" + this.startMinute[i] : this.startMinute[i]) + "h ";
        str += "End: " + (this.endHour[i] < 10 ? "0" + this.endHour[i] : this.endHour[i]) + ":" +
            (this.endMinute[i] < 10 ? "0" + this.endMinute[i] : this.endMinute[i]) + "h ";
      }
    }
    return str;
  }

  public void setAvaOfDay(DayOfWeek day, boolean open, int startHour, int startMinute, int endHour, int endMinute) {
    this.days[day.getValue() - 1] = open;
    this.startHour[day.getValue() - 1] = startHour;
    this.startMinute[day.getValue() - 1] = startMinute;
    this.endHour[day.getValue() - 1] = endHour;
    this.endMinute[day.getValue() - 1] = endMinute;
  }

  public boolean getAvaOfDay(DayOfWeek day) {
    return this.days[day.getValue() - 1];
  }

  public int getStartHour(DayOfWeek day) {
    return this.startHour[day.getValue() - 1];
  }

  public int getStartMinute(DayOfWeek day) {
    return this.startMinute[day.getValue() - 1];
  }

  public int getEndHour(DayOfWeek day) {
    return this.endHour[day.getValue() - 1];
  }

  public int getEndMinute(DayOfWeek day) {
    return this.endMinute[day.getValue() - 1];
  }
}
