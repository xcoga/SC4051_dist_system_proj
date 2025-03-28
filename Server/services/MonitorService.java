package Server.services;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.util.ArrayList;
import java.util.logging.Logger;

import Server.Operation;
import Server.RequestMessage;

/**
 * MonitorService class is used to manage all the monitors.
 * It can register, remove, and notify clients
 * when the underlying resource is updated.
 */
public class MonitorService {
  private static final Logger LOGGER = Logger.getLogger("MonitorService");
  private ArrayList<Monitor> monitors;

  public MonitorService() {
    this.monitors = new ArrayList<Monitor>();
  }

  /**
   * Register a monitor to the monitor service.
   * 
   * @param monitor Monitor object to register.
   */
  public void registerMonitor(Monitor monitor) {
    this.monitors.add(monitor);
  }

  /**
   * Remove a monitor from the monitor service.
   * 
   * @param monitor Monitor object to remove.
   */
  public void removeMonitor(Monitor monitor) {
    this.monitors.remove(monitor);
  }

  /**
   * Notify all clients when the underlying resource is updated.
   * 
   * @param socket DatagramSocket object for sending notifications.
   */
  public void notifyAll(DatagramSocket socket, String facilityName, String message) {
    // Create the notification message
    RequestMessage notificationMessage = new RequestMessage(Operation.READ.getOpCode(), 0, message);
    byte[] notificationBuffer = notificationMessage.toString().getBytes();

    // Send notification to all monitors
    for (Monitor monitor : this.monitors) {
      if (!monitor.resourceName.equals(facilityName)) {
        continue;
      }

      DatagramPacket notificationPacket = new DatagramPacket(notificationBuffer,
          notificationBuffer.length,
          monitor.clientAddress,
          monitor.clientPort);
      try {
        socket.send(notificationPacket);
      } catch (Exception e) {
        LOGGER.severe("Error sending notification to " +
            monitor.clientAddress + ":" + monitor.clientPort + ".\n" +
            e.getMessage());
      }
    }

    LOGGER.info("Notified all clients");
  }

  /**
   * Remove all expired monitors.
   */
  public boolean removeExpiredMonitors() {
    return this.monitors.removeIf(Monitor::ifExpired);
  }
}
