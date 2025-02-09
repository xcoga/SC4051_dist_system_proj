# SC4051 DISTRIBUTED SYSTEMS

This is a repo for the group project to design and implement a `Distributed Facility Booking System`.

# Marshalling

Data is in Big Endian format. MSB of data is at LSB of buffer.

Marhsalling Structure:

1. Null marker (bool) => Check if data is empty.
2. Object reference marker (bool) => Checks if object has been serialised before
3. Object reference/handle (Object) => consists of the entire object's fieldname, fieldtype + data.<br />
   3.1) Each object reference consists of : ClassName, field_lengths, fieldname, fieldType, fieldVal
