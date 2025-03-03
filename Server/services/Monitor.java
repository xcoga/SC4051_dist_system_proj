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
  
  public Monitor(String resourceName, int requestID, InetAddress clientAddress, int clientPort) {
    this.resourceName = resourceName;
    this.requestID = requestID;
    this.clientAddress = clientAddress;
    this.clientPort = clientPort;
  }
}
