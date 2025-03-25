# Response Structures
## Generic Responses

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

## Query Facility Availability

Sample response for successful query
```json
{
  "operation": 0,
  "requestID": 1,
  "data": "status: SUCCESS\nfacility: Gym\navailability:\n  MONDAY: Open 0800 to 1700\n  TUESDAY: Open 0800 to 1700\n  WEDNESDAY: Open 0800 to 1200, 1300 to 1700\n  THURSDAY: Closed\n  FRIDAY: Closed\n  SATURDAY: Closed\n  SUNDAY: Closed"
}
```

Human-readable format
```
status: SUCCESS
facility: Gym
availability:
  MONDAY: Open 0800 to 1700
  TUESDAY: Open 0800 to 1700
  WEDNESDAY: Open 0800 to 1200, 1300 to 1700
  THURSDAY: Closed
  FRIDAY: Closed
  SATURDAY: Closed
  SUNDAY: Closed
```

Error response
```json
{
  "operation": 0,
  "requestID": 1,
  "data": "status: ERROR\nmessage: Facility not found"
}
```

## Book Facility

Sample response for successful booking
```json
{
  "operation": 1,
  "requestID": 1,
  "data": "status: SUCCESS\nfacility: Gym\nbooking_date: MONDAY\nbooking_start: 1000\nbooking_end: 1200\nconfirmation_id: abc123"
}
```

Human-readable format
```
status: SUCCESS
facility: Gym
booking_date: MONDAY
booking_start: 1000
booking_end: 1200
confirmation_id: abc123
```

Error response
```json
{
  "operation": 1,
  "requestID": 1,
  "data": "status: ERROR\nmessage: Facility is unavailable during requested period."
}
```

## Change Existing Booking

Sample response for successful change
```json
{
  "operation": 2,
  “requestID": 1,
  "data": "status: SUCCESS\nconfirmation_id: xyz789\nnew_booking_start: 1100\nnew_booking_end: 1300"
}
```

Human-readable format
```
status: SUCCESS
confimration_id: xyz789
new_booking_date: MONDAY
new_booking_start: 1100
new_booking_end: 1300
```

Error response
```json
{
  "operation": 2,
  "requestID": 1,
  "data": "status: ERROR\nmessage: Invalid confirmation ID"
}
```

```json
{
  "operation": 2,
  "requestID": 1,
  "data": "status: ERROR\nmessage: Facility is unavailable during requested period"
}
```
## Monitor Facility Availability

Sample response for registration confirmation
```json
{
  "operation": 4,
  "requestID": 1,
  "data": "status: SUCCESS\nfacility: Gym\nmonitor_interval: 10 minutes\nmessage: Monitoring started"
}
```

Sample response for callback
```json
{
  "operation": 4,
  "requestID": 1,
  "data": "status: UPDATE\nfacility:Gym\navailability\n  MONDAY: Open 0800 to 1100, 1300 to 1700\n  TUESDAY: Open 0800 to 1700\n  WEDNESDAY: Open 0800 to 1200, 1300 to 1700\n  THURSDAY: Closed\n  FRIDAY: Closed\n  SATURDAY: Closed\n  SUNDAY: Closed"
}
```

Human-readable response
```
status: UPDATE
facility: Gym
availability:
  MONDAY: Open 0800 to 1100, 1300 to 1700
  TUESDAY: Open 0800 to 1700
  WEDNESDAY: Open 0800 to 1200, 1300 to 1700
  THURSDAY: Closed
  FRIDAY: Closed
  SATURDAY: Closed
  SUNDAY: Closed
```

Sample response for monitoring expiry
```json
{
  "operation": 4,
  "requestID": 1,
  "data": "status: INFO\nmessage: Monitoring interval expired"
}
```
