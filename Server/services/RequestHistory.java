package Server.services;

import java.util.ArrayList;

/**
 * RequestHistory class is used to store and manage client requests.
 * It prevents duplicate requests for non-idempotent operations by maintaining a history of requests.
 * It supports adding, checking, and updating requests in the history.
 */
public class RequestHistory {
  private ArrayList<RequestInfo> requestHistory; // List to store request information

  /**
   * Constructor to initialize the RequestHistory object.
   */
  public RequestHistory() {
    this.requestHistory = new ArrayList<RequestInfo>();
  }

  /**
   * Add a request to the history.
   * 
   * @param request RequestInfo object to add to the history.
   */
  public void addRequest(RequestInfo request) {
    this.requestHistory.add(request);
  }

  /**
   * Check if a request is in the history.
   * 
   * @param requestInfo RequestInfo object to check.
   * @return The matching RequestInfo object if found, null otherwise.
   */
  public RequestInfo containsRequest(RequestInfo requestInfo) {
    for (RequestInfo r : this.requestHistory) {
      if (r.requestMessage.getRequestID() == requestInfo.requestMessage.getRequestID() && 
          r.clientAddress.equals(requestInfo.clientAddress) && 
          r.clientPort == requestInfo.clientPort) {
        return r;
      }
    }
    return null;
  }

  /**
   * Check if a more recent request is in the history and replace it if the new request is more recent.
   * 
   * @param requestInfo RequestInfo object to check and add.
   * @return The previous RequestInfo if a more recent request exists, null otherwise.
   */
  public RequestInfo containsAndReplace(RequestInfo requestInfo) {
    RequestInfo r = null;
    for (int i = 0; i < this.requestHistory.size(); i++) {
      r = this.requestHistory.get(i);

      // If there is a request from the same client in the history
      if (r.clientAddress.equals(requestInfo.clientAddress) && 
          r.clientPort == requestInfo.clientPort) {
        // If the new request id is higher, replace the request
        if (r.requestMessage.getRequestID() < requestInfo.requestMessage.getRequestID()) {
          this.requestHistory.set(i, requestInfo);
          return null;
        }
        // If the new request id is lower, return the previous request info for handling by server
        return r;
      }
    }
    return null;
  }

  /**
   * Update a request in the history.
   * 
   * @param requestInfo RequestInfo object to update.
   * @return True if the update was successful, false otherwise.
   */
  public boolean updateRequestInfo(RequestInfo requestInfo) {
    for (RequestInfo r : this.requestHistory) {
      if (r.requestMessage.getRequestID() == requestInfo.requestMessage.getRequestID() && 
          r.clientAddress.equals(requestInfo.clientAddress) && 
          r.clientPort == requestInfo.clientPort) {
            // update the previous request here.
            r.responseMessage = requestInfo.responseMessage;
            return true;
      }
    }
    return false;
  }

  /**
   * Converts the RequestHistory object to a string representation.
   * 
   * @return A string containing all requests in the history.
   */
  public String toString() {
    String str = "Request History: ";
    for (RequestInfo r : this.requestHistory) {
      str += r.toString() + ";\n";
    }
    return str;
  }
}
