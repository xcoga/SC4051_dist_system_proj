# API Specification for Facility Booking Server

## Request Format
Requests to the server are structured as serialized objects containing the following fields:

- **requestType (integer)** – Represents the type of operation being requested.
  - Refer to the **Operations** section for details on each request type.

- **requestID (integer)** – A unique identifier for the request.
  - This field helps detect and ignore duplicate messages.

- **data (string)** – Additional parameters required for the request.

## Response Structures
### Generic Responses

Generic success response
```json
{
  "operation": <as_per_requestType_in_request>,
  "requestID": <as_per_requestID_in_request>,
  "data": "status: SUCCESS\n<data_string>"
}
```
See below for `data_string` formats for each of the available functions.

Generic error response
```json
{
  "operation": <as_per_requestType_in_request>,
  "requestID": <as_per_requestID_in_request>,
  "data": "status: ERROR\nmessage: <error_message>"
}
```

## Operations
This section explains how `requestType` values correspond to server operations.

| Operation | requestType | Description |
| --------- | ---------- | ----------- |
| **NONE**  | -1         | Invalid operation |
| **READ**  | 0          | Retrieves data from the server |
| **WRITE** | 1          | Creates new records (e.g., booking a facility) |
| **UPDATE** | 2         | Modifies existing data |
| **DELETE** | 3         | Removes data |
| **MONITOR** | 4        | Registers a client to receive updates when a monitored resource changes |
| **ECHO** | 5        | Echoes the request back to client without further processing |

## Response Format
Responses from the server are serialized objects that mirror the request structure.

- The server replies with the **same** `requestType` and `requestID` as the request.
- The **data** field contains the requested information or an error message.

## Supported Requests
All requests and responses are transmitted using **UDP packets**. The JSON format below is used for readability purposes.

### Echoing requests
#### Request and Response:
```json
{
  "operation": 5,
  "requestID": <any integer>,
  "data": <any string>
}
```

### Retrieve All Facility Names
#### Request:
```json
{
  "operation": 0,
  "requestID": 0,
  "data": "facility,ALL"
}
```
#### Response:
```json
{
  "operation": 0,
  "requestID": 0,
  "data": "status: SUCCESS\nWeekday1,Weekday2,Weekends,"
}
```

### Query Availability of a Specific Facility
#### Request:
```json
{
  "operation": 0,
  "requestID": 1,
  "data": "facility,Weekday1"
}
```
#### Response:
```json
{
  "operation": 0,
  "requestID": 1,
  "data": "status: SUCCESS\nAvailable timeslots: \nMONDAY: 08:00 - 17:00, \nTUESDAY: 08:00 - 17:00, \nWEDNESDAY: 08:00 - 17:00, \nTHURSDAY: Closed, \nFRIDAY: Closed, \nSATURDAY: Closed, \nSUNDAY: Closed"
}
```

### Query Rating of a Specific Facility
#### Request:
```json
{
  "operation": 0,
  "requestID": 1,
  "data": "rating,Weekday1"
}
```
#### Response:
```json
{
  "operation": 0,
  "requestID": 1,
  "data": "status: SUCCESS\nGet Rating: <rating>"
}
```

### Query a booking
#### Request:
```json
{
  "operation": 0,
  "requestID": 1,
  "data": "booking,<booking id>"
}
```
#### Response:
```json
{
  "operation": 1,
  "requestID": 1,
  "data": "status: SUCCESS\nBooking_ID: <confirmationID> by <user_address>:<user_port>"
}
```

### Book Facility
#### Request:
```json
{
  "operation": 1,
  "requestID": 1,
  "data": "Weekday1,MONDAY,10,0,12,0"
}
```
#### Response:
```json
{
  "operation": 1,
  "requestID": 1,
  "data": "status: SUCCESS\nBooking_ID: <confirmationID> by <user_address>:<user_port>"
}
```


### Update Facility
**Note: The First few fields after the FacilityName are details of the previous booking. The later fields are the new booking details to write.
#### Request:
```json
{
  "operation": 2,
  "requestID": 1,
  "data": "book,Weekday1,MONDAY,10,0,12,0,MONDAY,13,0,15,0"
}
```
#### Response:
```json
{
  "operation": 2,
  "requestID": 2,
  "data": "status: SUCCESS\nBooking_ID: <confirmationID> by <user_address>:<user_port>"
}
```


### (Update) Add a rating
#### Request:
```json
{
  "operation": 2,
  "requestID": 2,
  "data": "rating,Weekday1,5.0"
}
```
#### Response:
```json
{
  "operation": 2,
  "requestID": 2,
  "data": "status: SUCCESS\n Add Rating: <rating>"
}
```

## Error Handling
The server returns error messages in the format of "status: ERROR\nmessage: <error_message>".
The server may return the following errors:

| Error Message	| Description |
| ------------- | ----------- |
| facility not found | Returned when the requested facility does not exist. |
| unimplemented operation | Returned when a requested operation is not yet supported. |
| unknown operation | Returned when the requestType is invalid. |
| bad request | Returned when the request is malformed or incorrectly formatted. |
| invalid booking request format | Returned when the booking request format is invalid. |
| invalid booking request parameters | Returned when the booking request parameters are invalid. |
| facility not available at the requested time | Returned when the facility is not available at the requested time. |
