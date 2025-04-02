package Server.services;

import java.net.InetAddress;

/**
 * Monitor class stores information about a client monitoring a resource.
 * It is used to notify clients when the monitored resource is updated.
 */
public class Monitor {
  public String resourceName; // Name of the resource being monitored
  public int requestID; // Unique ID of the monitoring request
  public InetAddress clientAddress; // Client's address
  public int clientPort; // Client's port
  public long createdTime; // Time when the monitor was created
  public int monitorInterval; // Monitoring interval in seconds

  /**
   * Constructor to create a Monitor object with the current time as the created
   * time.
   * 
   * @param resourceName    The name of the resource being monitored.
   * @param requestID       The unique ID of the monitoring request.
   * @param clientAddress   The client's address.
   * @param clientPort      The client's port.
   * @param monitorInterval The monitoring interval in seconds.
   */
  public Monitor(String resourceName, int requestID, InetAddress clientAddress, int clientPort, int monitorInterval) {
    this.resourceName = resourceName;
    this.requestID = requestID;
    this.clientAddress = clientAddress;
    this.clientPort = clientPort;
    this.createdTime = System.currentTimeMillis();
    this.monitorInterval = monitorInterval;
  }

  /**
   * Constructor to create a Monitor object with a specified created time.
   * 
   * @param resourceName    The name of the resource being monitored.
   * @param requestID       The unique ID of the monitoring request.
   * @param clientAddress   The client's address.
   * @param clientPort      The client's port.
   * @param createdTime     The time when the monitor was created.
   * @param monitorInterval The monitoring interval in seconds.
   */
  public Monitor(String resourceName, int requestID, InetAddress clientAddress, int clientPort, long createdTime,
      int monitorInterval) {
    this.resourceName = resourceName;
    this.requestID = requestID;
    this.clientAddress = clientAddress;
    this.clientPort = clientPort;
    this.createdTime = createdTime;
    this.monitorInterval = monitorInterval;
  }

  /**
   * Check if the monitor has expired based on the monitoring interval.
   * 
   * @return True if the monitor has expired, false otherwise.
   */
  public boolean ifExpired() {
    long currentTime = System.currentTimeMillis();
    return (currentTime - this.createdTime) > this.monitorInterval * 1000;
  }
}
