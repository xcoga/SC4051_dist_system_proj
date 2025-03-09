# SC4051 Distributed Systems Project

This is a repository for the SC4051 group project to design and implement a `Distributed Facility Booking System`.

## Request-Reply Structure

### Request

| Byte Offset | Field            | Size (in bytes) | Data Type | Description                               |
| ----------- | ---------------- | --------------- | --------- | ----------------------------------------- |
| 0           | Request Type     | 4               | Integer   | See [Request Types](#request-types)       |
| 4           | Request ID       | 4               | Integer   | 0-indexed for every client                |
| 8           | Application Data | Variable        |           | See [Data Marshalling](#data-marshalling) |

#### Request Types

| ID  | Request Type | Description |
| --- | ------------ | ----------- |
| 0   | `READ`       |             |
| 1   | `WRITE`      |             |
| 2   | `UPDATE`     |             |
| 3   | `DELETE`     |             |
| 4   | `MONITOR`    |             |

### Reply

| Byte Offset | Field            | Size (in bytes) | Data Type | Description                               |
| ----------- | ---------------- | --------------- | --------- | ----------------------------------------- |
| 0           | Request ID       | 4               | Integer   | ID of request that server is replying to  |
| 4           | Application Data | Variable        |           | See [Data Marshalling](#data-marshalling) |

## Data Marshalling

Data is in Big Endian format. MSB of data is at LSB of buffer.

### Structure

1. Null marker (`bool`) $\rightarrow$ Checks if data is empty.

2. Object reference marker (`bool`) $\rightarrow$ Checks if object has been serialized before.

3. Object reference / handle (`Object`) $\rightarrow$ Consists of the entire object's `fieldname`, `fieldtype`, and `data`.

   - Every object reference consists of: `ClassName`, `field_lengths`, `fieldname`, `fieldType`, `fieldVal`

## Compilation and Execution

### Java Server

1. To change directory into `Server/`, run: `cd Server/`

2. To compile Java source files into `bin/` directory, run: `javac -d bin Server.java utils/*.java`

3. To start the Java server, run: `java -cp bin Server.Server`

### C++ Client

1. To change directory into `Client/`, run: `cd Client/`

2. To create `build/` directory and change into it, run: `mkdir build/ && cd build/`

3. To generate build files using CMake, run: `cmake ..`

4. To compile C++ source files into `build/` directory, run `cmake --build .`

5. To start the C++ client,

   - On Windows, run: `Client.exe`

   - On UNIX, run: `./Client`


#### Windows-specific
1. Compile into .exe file called 'Client':
   g++ Client/Client.cpp Client/utils/ByteBuffer.cpp Client/utils/ByteReader.cpp Client/utils/Serializer.cpp -o Client/Client -lws2_32

Note: The -lws2_32 flag in your compilation command is a linker flag that tells the compiler to link your program with the Windows Socket 2 library (ws2_32.dll)

2.  Execute the file: Client.exe
