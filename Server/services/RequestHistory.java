package Server.services;

import java.util.ArrayList;

/**
 * ServiceHistory class is used to store requests from clients.
 * To be used to prevent duplicate requests for non-idempotent requests.
 * It has methods to add a request to the list, check if a request is in the list.
 * Only saves most recent request from client.
 */
public class RequestHistory {
  private ArrayList<RequestInfo> requestHistory;

  public RequestHistory() {
    this.requestHistory = new ArrayList<RequestInfo>();
  }

  /**
   * Add a request to the history.
   * @param request Request object to add to the history.
   */
  public void addRequest(RequestInfo request) {
    this.requestHistory.add(request);
  }

  /**
   * Check if a request is in the history.
   * @param requestInfo Request information object to check.
   * @return the request in the history, null otherwise.
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
   * Check if a more recent request is in the history
   * and replace it with new request if new request is more recent.
   * @param requestInfo Request information object to check and add.
   * @return the previous RequestInfo if there is a more recent request in the history, false otherwise.
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
   * Update a request is in the history.
   * @param requestInfo Request information object to update.
   * @return true if successful.
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
}
