package Server.services;

import java.net.InetAddress;

/**
 * Monitor class is used to store information
 * to be used to notify clients when the underlying resource is updated.
 */
public class Monitor {
  public String resourceName;
  public int requestID;
  public InetAddress clientAddress;//client address
  public int clientPort;//client port
  
  // created time of monitor
  public long createdTime;
  // monitor interval in seconds
  public int monitorInterval;
  
  // This contructor should be used by default to create a monitor
  // with the current time as the created time.
  public Monitor(String resourceName, int requestID, InetAddress clientAddress, int clientPort, int monitorInterval) {
    this.resourceName = resourceName;
    this.requestID = requestID;
    this.clientAddress = clientAddress;
    this.clientPort = clientPort;
    this.createdTime = System.currentTimeMillis();
    this.monitorInterval = monitorInterval;
  }

  public Monitor(String resourceName, int requestID, InetAddress clientAddress, int clientPort, long createdTime, int monitorInterval) {
    this.resourceName = resourceName;
    this.requestID = requestID;
    this.clientAddress = clientAddress;
    this.clientPort = clientPort;
    this.createdTime = createdTime;
    this.monitorInterval = monitorInterval;
  }

  public boolean ifExpired(){
    long currentTime = System.currentTimeMillis();
    return (currentTime - this.createdTime) > this.monitorInterval * 1000;
  }
}
