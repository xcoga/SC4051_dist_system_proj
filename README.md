# SC4051 DISTRIBUTED SYSTEMS

This is a repo for the group project to design and implement a `Distributed Facility Booking System`.

# Marshalling

Data is in Big Endian format. MSB of data is at LSB of buffer.

Marhsalling Structure:

1. Null marker (bool) => Check if data is empty.
2. Object reference marker (bool) => Checks if object has been serialised before
3. Object reference/handle (Object) => consists of the entire object's fieldname, fieldtype + data.<br />
   3.1. Each object reference consists of : ClassName, field_lengths, fieldname, fieldType, fieldVal

Just some compilation and run program tips:

1. javac -d bin Main.java utils/_.java
   This ensures:
   Main.class is inside bin/SC4051_dist_system_proj/
   utils/_.class is inside bin/SC4051_dist_system_proj/utils/

2. Since Main.java has package SC4051_dist_system_proj;, you must run it with:
   java -cp bin SC4051_dist_system_proj.Main

For C++:

1.  Compile into .exe file called 'main': g++ main.cpp -o main
2.  Execute the file: main.exe
