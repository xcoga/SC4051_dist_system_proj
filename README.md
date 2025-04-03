# SC4051 Distributed Systems Project

This is a repository for the AY24/25 Semester 2 SC4051 group project to design and implement a Distributed Facility Booking System.

## Contributors

- Bay Yong Wei Nicholas ([@nicholasbay](https://github.com/nicholasbay))
- Choh Lit Han Owen ([@Owen-Choh](https://github.com/Owen-Choh))
- Dai Xichen ([@xcoga](https://github.com/xcoga))

## Request-Reply Structure

Refer to the [API specifications](API_Specification.md) for the request-reply structure.

## Data Marshalling

Data is in big-endian format, meaning the most-significant byte (MSB) of data is at the least-significant byte (LSB) of buffer.

### Structure

1. Null marker (`bool`) $\rightarrow$ Checks if data is empty.

2. Object reference marker (`bool`) $\rightarrow$ Checks if object has been serialized before.

3. Object reference / handle (`Object`) $\rightarrow$ Consists of the entire object's `fieldname`, `fieldtype`, and `data`.

   - Every object reference consists of: `ClassName`, `field_lengths`, `fieldname`, `fieldType`, `fieldVal`

## Compilation and Execution

### Java Server

1. To compile Java source files into `Server/bin/` directory, run: `javac -d Server/bin Server/Server.java Server/utils/*.java`

2. To start the Java server, run: `java -cp Server/bin Server.Server`
   - For at-least-once configuration with request dropping: run: `java -cp Server/bin Server.Server at-least-once true`
   - For at-most-once configuration with request dropping: run: `java -cp Server/bin Server.Server at-most-once true`

### C++ Client

1. To change directory into `Client/`, run: `cd Client/`

2. To create `build/` directory and change into it, run: `mkdir build/ && cd build/`

3. To generate build files using CMake, run: `cmake ..`

4. To compile C++ source files into `build/` directory, run `cmake --build .`

5. To start the C++ client,

   - On Windows, run: `Client.exe`

   - On UNIX, run: `./Client`
