# API Specifications for Facility Booking Server

## Table of Contents

1. [Request Format](#request-format)

2. [Response Format](#response-format)

3. [Operations](#operations)

4. [Supported Requests](#supported-requests)

    - [Query Facility Names](#query-facility-names)
    - [Query Facility Availability](#query-facility-availability)
    - [Book Facility](#book-facility)
    - [Query Existing Booking](#query-existing-booking)
    - [Update Existing Booking](#update-existing-booking)
    - [Delete Existing Booking](#delete-existing-booking)
    - [Monitor Facility Availability](#monitor-facility-availability)
    - [Rate Facility](#rate-facility)
    - [Query Rating of a Specific Facility](#query-rating-of-a-specific-facility)
    - [Echo Message](#echo-message)

5. [Error Handling](#error-handling)

    - [Common Error Messages](#common-error-messages)
    - [Example Error Response](#example-error-response)

## Request Format

Requests to the server are structured as serialized objects containing the following fields:

- `requestType` **(integer)** — Represents the type of operation being requested.

  - Refer to the [Operations](#operations) section for details on each request type.

- `requestID` **(integer)** — A unique identifier for the request.

  - This field allows the server to detect and ignore duplicate messages when using at-most-once semantics.

- `data` **(string)** — Additional parameters required for the request.

### Example Request

```json
{
  "requestType": 0,
  "requestID": 4051,
  "data": "facility,ALL"
}
```

## Response Format

Responses from the server are structured similarly to requests, containing the following fields:

- `operation` **(integer)** — Indicates the type of operation performed.

  - This field matches the `requestType` from the request.

- `requestID` **(integer)** — The unique identifier for the request.

  - This field matches the `requestID` from the request.

- `data` **(string)** — Contains the result of the operation or an error message.

  - The format of this field varies based on the operation performed.

The generic response format is as follows:

```json
{
  "operation": <as_per_requestType>,        // Matches the requestType from the request
  "requestID": <as_per_requestID>,          // Matches the requestID from the request
  "data": "status:SUCCESS\n<data_string>"   // Contains the result of the operation
}
```

> **Note:** Refer to the [Supported Requests](#supported-requests) section for the specific formats of `<data_string>` for each operation.

## Operations

The `requestType` field in the request determines the type of operation the server will perform. The table below explains the supported operations and their corresponding `requestType` values.

| Operation   | `requestType` | Description                                                                                   |
|-------------|---------------|-----------------------------------------------------------------------------------------------|
| **NONE**    | -1            | Invalid operation                                                                             |
| **READ**    | 0             | Retrieves data from the server                                                                |
| **WRITE**   | 1             | Creates new records (e.g., booking a facility)                                                |
| **UPDATE**  | 2             | Modifies existing data (e.g., updating a booking)                                             |
| **DELETE**  | 3             | Removes data (e.g., deleting a booking)                                                       |
| **MONITOR** | 4             | Registers a client to receive updates when the availability of the monitored facility changes |
| **ECHO**    | 5             | Echoes the request back to client without further processing                                  |

## Supported Requests

All requests and responses are transmitted using **UDP packets**. The JSON format below is used for readability purposes.

---

### Query Facility Names

This operation retrieves the names of all available facilities.

#### Request:

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "facility,ALL"
}
```

| Field       | Type      | Description                                                       |
|-------------|-----------|-------------------------------------------------------------------|
| `operation` | `integer` | The operation type (0 for read)                                   |
| `requestID` | `integer` | The unique identifier for the request                             |
| `data`      | `string`  | The string "facility,ALL" to query all facility names             |

#### Response:

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\nfacilityNames:Weekday1,Weekday2,Weekends,"
}
```

| Field       | Type      | Description                                                  |
|-------------|-----------|--------------------------------------------------------------|
| `operation` | `integer` | Matches the `operation` from the request                     |
| `requestID` | `integer` | Matches the `requestID` from the request                     |
| `data`      | `string`  | Contains the status and list of facility names               |

---

### Query Facility Availability

This operation retrieves the availability of a specific facility.

#### Request:

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "facility,<facilityName>"                     // Query all available days
}
```

> **Note:** If specific days are included in the request, the response will only include those days.

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "facility,<facilityName>,<day1>,<day2>,..."   // Optional days to query
}
```

| Field       | Type      | Description                                                       |
|-------------|-----------|-------------------------------------------------------------------|
| `operation` | `integer` | The operation type (0 for read)                                   |
| `requestID` | `integer` | The unique identifier for the request                             |
| `data`      | `string`  | The facility name and, optionally, days to query availability for |

#### Response:

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\nfacility:<facilityName>\navailableTimeslots:\nMONDAY: 0800 - 1200,1300 - 1700,\nTUESDAY: 0800 - 1700, \nWEDNESDAY: 0800 - 1700,\n"
}
```

| Field       | Type      | Description                                                  |
|-------------|-----------|--------------------------------------------------------------|
| `operation` | `integer` | Matches the `operation` from the request                     |
| `requestID` | `integer` | Matches the `requestID` from the request                     |
| `data`      | `string`  | Contains the status and available timeslots for the facility |

---

### Book Facility

This operation allows a user to book a facility for a specific time slot.

#### Request:

```json
{
  "operation": 1,
  "requestID": <any_integer>,
  "data": "Weekday1,MONDAY,10,0,12,0"
}
```

| Field       | Type      | Description                                              |
|-------------|-----------|----------------------------------------------------------|
| `operation` | `integer` | The operation type (1 for write)                         |
| `requestID` | `integer` | The unique identifier for the request                    |
| `data`      | `string`  | The facility name, day, start time, and end time to book |

#### Response:

```json
{
  "operation": 1,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\nbookingID:<bookingID>\nuser:<user_address>:<user_port>\nfacility:Weekday1\nday:MONDAY\nstartTime:1000\nendTime:1200"
}
```

| Field       | Type      | Description                                 |
|-------------|-----------|---------------------------------------------|
| `operation` | `integer` | Matches the `operation` from the request    |
| `requestID` | `integer` | Matches the `requestID` from the request    |
| `data`      | `string`  | Contains the status and new booking details |

> **Note:** The `bookingID` is a UUID v4 value that uniquely identifies the booking.

---

### Query Existing Booking

This operation retrieves the details of a specific booking.

#### Request

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "booking,<bookingID>"
}
```

| Field       | Type      | Description                           |
|-------------|-----------|---------------------------------------|
| `operation` | `integer` | The operation type (0 for read)       |
| `requestID` | `integer` | The unique identifier for the request |
| `data`      | `string`  | The booking ID to query               |

#### Response

```json
{
  "operation": 1,
  "requestID": 1,
  "data": "status: SUCCESS\nbookingID:<bookingID>\nuser: <user_address>:<user_port>\nfacility:<facilityName>\nday:<dayOfWeek>\nstartTime:<timeHHMM>\nendTime:<timeHHMM>\n"
}
```

> **Note:** The query booking operation returns the booking details in the same format as the booking operation.

| Field       | Type      | Description                                     |
|-------------|-----------|-------------------------------------------------|
| `operation` | `integer` | Matches the `operation` from the request        |
| `requestID` | `integer` | Matches the `requestID` from the request        |
| `data`      | `string`  | Contains the status and queried booking details |

---

### Update Existing Booking

This operation updates the details of an existing booking.

\*\*Note: The First few fields after the FacilityName are details of the previous booking. The later fields are the new booking details to write.

#### Request

```json
{
  "operation": 2,
  "requestID": <any_integer>,
  "data": "booking,<oldBookingID>,<facilityName>,<dayOfWeek>,<newStartHour>,<newStartMinute>,<newEndHour>,<newEndMinute>"
}
```

| Field       | Type      | Description                                                       |
|-------------|-----------|-------------------------------------------------------------------|
| `operation` | `integer` | The operation type (2 for update)                                 |
| `requestID` | `integer` | The unique identifier for the request                             |
| `data`      | `string`  | The old booking ID, old facility name, old day, and new time slot |

> **Note:** The update operation only allows a shift of the booking time. The facility and day of the week booked cannot be amended.

#### Response

```json
{
  "operation": 2,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\noldBookingID:<oldBookingID>\nnewBookingID:<newBookingID>\nuser:<user_address>:<user_port>\nfacility:Weekday1\nday:MONDAY\nstartTime:1000\nendTime:1200"
}
```

| Field       | Type      | Description                                     |
|-------------|-----------|-------------------------------------------------|
| `operation` | `integer` | Matches the `operation` from the request        |
| `requestID` | `integer` | Matches the `requestID` from the request        |
| `data`      | `string`  | Contains the status and updated booking details |

---

### Delete Existing Booking

This operation deletes a specific booking.

#### Request

```json
{
  "operation": 3,
  "requestID": <any_integer>,
  "data": "<bookingID>,<facilityName>"
}
```

| Field       | Type      | Description                                    |
|-------------|-----------|------------------------------------------------|
| `operation` | `integer` | The operation type (3 for delete)              |
| `requestID` | `integer` | The unique identifier for the request          |
| `data`      | `string`  | The old booking ID and facility name to delete |

#### Response

```json
{
  "operation": 3,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\nbookingID:<bookingID>\nuser:<user_address>:<user_port>"
}
```

| Field       | Type      | Description                                     |
|-------------|-----------|-------------------------------------------------|
| `operation` | `integer` | Matches the `operation` from the request        |
| `requestID` | `integer` | Matches the `requestID` from the request        |
| `data`      | `string`  | Contains the status and deleted booking details |

---

### Monitor Facility Availability

This operation allows a client to register for updates on the availability of a specific facility.

#### Request

```json
{
  "operation": 4,
  "requestID": <any_integer>,
  "data": "register,<facilityName>,<intervalSeconds>"
}
```

| Field       | Type      | Description                                                |
|-------------|-----------|------------------------------------------------------------|
| `operation` | `integer` | The operation type (4 for monitor)                         |
| `requestID` | `integer` | The unique identifier for the request                      |
| `data`      | `string`  | The facility name and interval for monitoring availability |

#### Responses

This operation has two types of responses:

1. **Success Response**: Indicates that the client has been successfully registered for monitoring.

    ```json
    {
      "operation": 4,
      "requestID": <any_integer>,
      "data": "status:SUCCESS\nfacility:<facilityName>\ninterval:<intervalSeconds>"
    }
    ```

    | Field       | Type      | Description                                                |
    |-------------|-----------|------------------------------------------------------------|
    | `operation` | `integer` | Matches the `operation` from the request                   |
    | `requestID` | `integer` | Matches the `requestID` from the request                   |
    | `data`      | `string`  | Contains the facility name and interval for monitoring     |

2. **Monitor Messages**: Sent periodically to the client to inform them of changes in facility availability.

    ```json
    {
      "operation": 4,
      "requestID": <any_integer>,
      "data": "status:SUCCESS\nfacility:<facilityName>\navailableTimeslots:\nMONDAY: 0800 - 1200,1300 - 1700,\nTUESDAY: 0800 - 1700, \nWEDNESDAY: 0800 - 1700,\n"
    }
    ```

    | Field       | Type      | Description                                                  |
    |-------------|-----------|--------------------------------------------------------------|
    | `operation` | `integer` | Set to 0 for read                                            |
    | `requestID` | `integer` | Set to 0                                                     |
    | `data`      | `string`  | Contains the status and available timeslots for the facility |

---

### Rate Facility

This operation allows a user to rate a specific facility.

#### Request

```json
{
  "operation": 2,
  "requestID": <any_integer>,
  "data": "rating,<facilityName>,<rating>"
}
```

| Field       | Type      | Description                            |
|-------------|-----------|----------------------------------------|
| `operation` | `integer` | The operation type (2 for update)      |
| `requestID` | `integer` | The unique identifier for the request  |
| `data`      | `string`  | The facility name and rating to submit |

#### Response

```json
{
  "operation": 2,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\nuser:<user_address>:<user_port>\nfacility:<facilityName>\nrating:<rating>"
}
```

| Field       | Type      | Description                                      |
|-------------|-----------|--------------------------------------------------|
| `operation` | `integer` | Matches the `operation` from the request         |
| `requestID` | `integer` | Matches the `requestID` from the request         |
| `data`      | `string`  | Contains the status and submitted rating details |

---

### Query Rating of a Specific Facility

This operation retrieves the average rating of a specific facility.

#### Request:

```json
{
  "operation": 0,
  "requestID": <any_intger>,
  "data": "rating,<facilityName>"
}
```

| Field       | Type      | Description                               |
|-------------|-----------|-------------------------------------------|
| `operation` | `integer` | The operation type (0 for read)           |
| `requestID` | `integer` | The unique identifier for the request     |
| `data`      | `string`  | The facility name to query the rating for |

#### Response:

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "status:SUCCESS\nfacility:<facilityName>\nrating:<averageRating>"
}
```

| Field       | Type      | Description                                    |
|-------------|-----------|------------------------------------------------|
| `operation` | `integer` | Matches the `operation` from the request       |
| `requestID` | `integer` | Matches the `requestID` from the request       |
| `data`      | `string`  | Contains the status and average rating details |

--

### Echo Message

This operation echoes the request back to the client without further processing.

#### Request

```json
{
  "operation": 5,
  "requestID": <any_integer>,
  "data": <any_string>
}
```

| Field       | Type      | Description                           |
|-------------|-----------|---------------------------------------|
| `operation` | `integer` | The operation type (5 for echo)       |
| `requestID` | `integer` | The unique identifier for the request |
| `data`      | `string`  | The data to be echoed back            |

#### Response

```json
{
  "operation": 5,
  "requestID": <any_integer>,
  "data": <any_string>
}
```

## Error Handling

The server returns error messages in the following format:

```json
{
  "operation": <as_per_requestType>,                // Matches the requestType from the request
  "requestID": <as_per_requestID>,                  // Matches the requestID from the request
  "data": "status:ERROR\nmessage:<error_message>"   // Contains the error message
}
```

### Common Error Messages

The table below lists the possible error messages and their descriptions:

| Error Message                                | Description                                                        |
|----------------------------------------------|--------------------------------------------------------------------|
| Facility not found                           | Returned when the requested facility does not exist.               |
| Unimplemented operation                      | Returned when a requested operation is not yet supported.          |
| Unknown operation                            | Returned when the `requestType` is invalid.                        |
| Bad request                                  | Returned when the request is malformed or incorrectly formatted.   |
| Invalid booking request format               | Returned when the booking request format is invalid.               |
| Invalid booking request parameters           | Returned when the booking request parameters are invalid.          |
| Facility not available at the requested time | Returned when the facility is not available at the requested time. |

### Example Error Response

```json
{
  "operation": 0,
  "requestID": <any_integer>,
  "data": "status:ERROR\nmessage:Facility not found"
}
```
