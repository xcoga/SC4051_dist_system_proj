# API Specification for Facility Booking Server

## Request Format
Requests to the server are structured as serialized objects containing the following fields:

- **requestType (integer)** – Represents the type of operation being requested.
  - Refer to the **Operations** section for details on each request type.

- **requestID (integer)** – A unique identifier for the request.
  - This field helps detect and ignore duplicate messages.

- **data (string)** – Additional parameters required for the request.

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

## Response Format
Responses from the server are serialized objects that mirror the request structure.

- The server replies with the **same** `requestType` and `requestID` as the request.
- The **data** field contains the requested information or an error message.

## Supported Requests

### Retrieve All Facility Names
#### Request:
```
{
  "operation": 0,
  "requestID": 0,
  "data": "ALL"
}
```
#### Response:
```
{
  "operation": 0,
  "requestID": 0,
  "data": "Facility: Weekday1 Facility: Weekday2 Facility: Weekends"
}
```

### Query Availability of a Specific Facility
#### Request:
```
{
  "operation": 0,
  "requestID": 1,
  "data": "Weekday1"
}
```
#### Response:
```
{
  "operation": 0,
  "requestID": 1,
  "data": "Facility: Weekday1, Availability: Day MONDAY: Open Start: 08:00h End: 17:00h Day TUESDAY: Open Start: 08:00h End: 17:00h Day WEDNESDAY: Open Start: 08:00h End: 17:00h Day THURSDAY: Closed Day FRIDAY: Closed Day SATURDAY: Closed Day SUNDAY: Closed"
}
```


## Error Handling
The server may return the following errors:

| Error Message	| Description |
| ------------- | ----------- |
| ERROR: facility not found | Returned when the requested facility does not exist. |
| ERROR: unimplemented operation | Returned when a requested operation is not yet supported. |
| ERROR: unknown operation | Returned when the requestType is invalid. |
| ERROR: bad request | Returned when the request is malformed or incorrectly formatted. |