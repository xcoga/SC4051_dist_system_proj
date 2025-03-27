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
  "data": "status:SUCCESS\n<data_string>"
}
```

See below for `data_string` formats for each of the available functions.

Generic error response

```json
{
  "operation": <as_per_requestType_in_request>,
  "requestID": <as_per_requestID_in_request>,
  "data": "status:ERROR\nmessage:<error_message>"
}
```

## Operations

This section explains how `requestType` values correspond to server operations.

| Operation   | requestType | Description                                                             |
| ----------- | ----------- | ----------------------------------------------------------------------- |
| **NONE**    | -1          | Invalid operation                                                       |
| **READ**    | 0           | Retrieves data from the server                                          |
| **WRITE**   | 1           | Creates new records (e.g., booking a facility)                          |
| **UPDATE**  | 2           | Modifies existing data                                                  |
| **DELETE**  | 3           | Removes data                                                            |
| **MONITOR** | 4           | Registers a client to receive updates when a monitored resource changes |
| **ECHO**    | 5           | Echoes the request back to client without further processing            |

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
  "requestID": <any_integer>,
  "data": <any_string>
}
```

### Retrieve All Facility Names

#### Request:

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "facility,ALL"
}
```

#### Response:

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\nfacilityNames:Weekday1,Weekday2,Weekends,"
}
```

### Query Availability of a Specific Facility

#### Request:

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "facility,<facilityName>"
}
```

#### Response:

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\nfacility:<facilityName>\navailableTimeslots:\nMONDAY: 0800 - 1200,1300 - 1700,\nTUESDAY: 0800 - 1700, \nWEDNESDAY: 0800 - 1700,\n"
}
```

### Query Rating of a Specific Facility

#### Request:

```json
{
  "operation": 0,
  "requestID": <any_intger>,
  "data": "rating,<facilityName>"
}
```

#### Response:

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\nrating:5.0"
}
```

### Query a booking

#### Request:

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "booking,<bookingID>"
}
```

#### Response:

```json
{
  "operation": 1,
  "requestID": 1,
  "data": "status: SUCCESS\n
           BookingID: <confirmationID>\n
           User: <user_address>:<user_port>\n
           Facility: <facility name>\n
           Day: <day>\n
           StartTime: <time in format of %02d%02d>\n
           EndTime: <time in format of %02d%02d>\n"
}
```

### Book Facility

#### Request:

```json
{
  "operation": 1,
  "requestID": <any_integer>,
  "data": "Weekday1,MONDAY,10,0,12,0"
}
```

#### Response:

```json
{
  "operation": 1,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\nbookingID:<bookingID>\nuser:<user_address>:<user_port>\nfacility:Weekday1\nday:MONDAY\nstartTime:1000\nendTime:1200"
}
```

### Update Booking

\*\*Note: The First few fields after the FacilityName are details of the previous booking. The later fields are the new booking details to write.

#### Request:

```json
{
  "operation": 2,
  "requestID": <any_integer>,
  "data": "booking,<oldBookingID>,<facilityName>,<Day>,<startHour>,<startMinute>,<endHour>,<endMinute>"
}
```

#### Response:

```json
{
  "operation": 2,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\n
          oldBookingID:<oldBookingID>\n
          newBookingID:<newBookingID>\n
          user:<user_address>:<user_port>\n
          facility:Weekday1\n
          day:MONDAY\n
          startTime:1000\n
          endTime:1200"
}
```

### (Update) Add a rating

#### Request:

```json
{
  "operation": 2,
  "requestID": <any_integer>,
  "data": "rating,<facilityName>,<rating>"
}
```

#### Response:

```json
{
  "operation": 2,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\nrating:5.0\nuser:<user_address>:<user_port>"
}
```

### Delete a booking

#### Request:

```json
{
  "operation": 3,
  "requestID": <any_integer>,
  "data": "<bookingId_to_delete>,<facilityName>"
}
```

#### Response:

```json
{
  "operation": 3,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\n Booking_ID: <booking_id> deleted by <user_info>"
}
```

## Error Handling

The server returns error messages in the format of "status:ERROR\nmessage:<error_message>".
The server may return the following errors:

| Error Message                                  | Description                                                        |
| ---------------------------------------------- | ------------------------------------------------------------------ |
| `Facility not found`                           | Returned when the requested facility does not exist.               |
| `Unimplemented operation`                      | Returned when a requested operation is not yet supported.          |
| `Unknown operation`                            | Returned when the requestType is invalid.                          |
| `Bad request`                                  | Returned when the request is malformed or incorrectly formatted.   |
| `Invalid booking request format`               | Returned when the booking request format is invalid.               |
| `Invalid booking request parameters`           | Returned when the booking request parameters are invalid.          |
| `Facility not available at the requested time` | Returned when the facility is not available at the requested time. |
