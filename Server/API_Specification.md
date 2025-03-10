# API Specification for Facility Booking Server

## Request Format
Requests to the server are structured as serialized objects containing the following fields:

- requestType (integer) - Represents the type of operation being requested.

  - Please see the following section to see how this field is interpreted as an operation

- requestID (integer) - A unique identifier for the request.
  
  - Note that requestID will be used to detect and duplicated messages

- data (string) - Additional parameters required for the request.

### Operations
This is an overview of how the requestType are mapped to server operations and their general functions.

| Operation | requestType | Description |
| ----------- | ---------------- | --------------- |
| NONE | -1 | Invalid operation |
| READ | 0 | Used for reading data |
| WRITE | 1 | Used for creating data |
| UPDATE | 2 | Used for updating data |
| DELETE | 3 | Used for deleting data |
| MONITOR | 4 | Used for adding a client to monitor a  specific resource |

