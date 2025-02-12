# SC4051 Distributed Systems Project

This is a repo for the group project to design and implement a `Distributed Facility Booking System`.

## Request-Reply Structure
### Request
| Byte Offset | Field | Size (in bytes) | Data Type | Description |
|---|---|---|---|---|
| 0 | Request Type | 4 | Integer | See [Request Types](#request-types) |
| 4 | Request ID | 4 | Integer | 0-indexed for every client |
| 8 | Application Data | Variable |  | See [Data Marshalling](#data-marshalling) |

#### Request Types
| ID | Request Type | Description |
| --- | --- | --- |
| 0 | `READ` |  |
| 1 | `WRITE` |  |
| 2 | `UPDATE` |  |
| 3 | `DELETE` |  |
| 4 | `MONITOR` |  |

### Reply
| Byte Offset | Field | Size (in bytes) | Data Type | Description |
|---|---|---|---|---|
| 0 | Request ID | 4 | Integer | ID of request that server is replying to |
| 4 | Application Data | Variable |  | See [Data Marshalling](#data-marshalling) |

## Data Marshalling
Data is in Big Endian format. MSB of data is at LSB of buffer.

### Structure
1. Null marker (bool) => Check if data is empty.
2. Object reference marker (bool) => Checks if object has been serialised before
3. Object reference/handle (Object) => consists of the entire object's fieldname, fieldtype + data.<br />
   3.1. Each object reference consists of : ClassName, field_lengths, fieldname, fieldType, fieldVal
