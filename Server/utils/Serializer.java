package Server.utils;

import java.util.HashMap;
import java.util.Map;

/**
 * Serializer class provides methods to serialize and deserialize objects.
 * It handles circular references, primitive types, arrays, and custom objects.
 * It also includes parity bit calculation for data integrity.
 */
public class Serializer {
    private static Map<Object, Integer> serializedObjects = new HashMap<>(); // Tracks serialized objects
    private static Map<Integer, Object> deserializedObjects = new HashMap<>(); // Tracks deserialized objects
    private static int objectCounter = 0; // Counter for object references

    /**
     * Serialize an object into a byte array.
     * 
     * @param obj The object to serialize.
     * @return A byte array representing the serialized object.
     * @throws Exception If serialization fails.
     */
    public static byte[] serialize(Object obj) throws Exception {
        serializedObjects.clear();
        objectCounter = 0;
        ByteBuffer buffer = new ByteBuffer(1024);
        serializeObject(obj, buffer);

        // Get the serialized data
        byte[] serializedData = buffer.getBuffer();
        byte parityBit = Parity.calculateEvenParityBit(serializedData);

        // Add the parity bit to the end of the same buffer
        buffer.writeByte(parityBit);

        // Return the buffer contents (now includes the parity bit)
        return buffer.getBuffer();
    }

    /**
     * Serialize an individual object and write it to the buffer.
     * 
     * @param obj    The object to serialize.
     * @param buffer The ByteBuffer to write serialized data to.
     * @throws Exception If serialization fails.
     */
    private static void serializeObject(Object obj, ByteBuffer buffer) throws Exception {
        /*
         * Structure of the byte array:
         * 
         * Null marker
         * Object reference marker (Checks if object has been serialized before)
         * Object reference/handle (consists of the entire object's fieldname, fieldtype
         * + data.)
         * 
         * In each object reference/handle:
         * ClassName, field_lengths, fieldname, fieldType,fieldVal
         */

        if (obj == null) {
            buffer.writeByte((byte) 0); // null marker
            return;
        }

        buffer.writeByte((byte) 1); // non-null marker

        // Check if object type was already serialized. Check if object handle exists.
        if (serializedObjects.containsKey(obj)) {
            buffer.writeByte((byte) 1); // reference marker, object handle exists.
            buffer.writeInt(serializedObjects.get(obj));
            return;
        }

        buffer.writeByte((byte) 0); // reference marker, object handle dont exist.
        serializedObjects.put(obj, objectCounter++);

        // Write class metadata
        buffer.writeString(obj.getClass().getName());

        // Issue with accessing restricted internal Java classes for wrapper classes
        // types Integer, Long, etc.,
        // which throws an error.
        java.lang.reflect.Field[] allFields = obj.getClass().getDeclaredFields();
        java.util.List<java.lang.reflect.Field> serializableFields = new java.util.ArrayList<>();

        for (java.lang.reflect.Field field : allFields) {
            // System.out.println("object's field: " + field);
            int modifiers = field.getModifiers();
            if (!java.lang.reflect.Modifier.isStatic(modifiers) &&
                    !java.lang.reflect.Modifier.isTransient(modifiers)) {
                serializableFields.add(field);
            }
        }

        // Write the number of fields to serialize.
        buffer.writeInt(serializableFields.size());

        // MODIFIED: Write field metadata and value together for each field
        // This matches the C++ serialization format
        for (java.lang.reflect.Field field : serializableFields) {
            field.setAccessible(true);

            // Write field metadata
            buffer.writeString(field.getName());
            buffer.writeString(field.getType().getName());

            // Write field value immediately after metadata
            // writeValue(buffer, field.get(obj), field.getType());
        }

        // TESTING THIS LOOP
        for (java.lang.reflect.Field field : serializableFields) {
            writeValue(buffer, field.get(obj), field.getType());
        }

    }

    /**
     * Write a value to the buffer based on its type.
     * 
     * @param buffer The ByteBuffer to write to.
     * @param value  The value to write.
     * @param type   The type of the value.
     * @throws Exception If writing fails.
     */
    private static void writeValue(ByteBuffer buffer, Object value, Class<?> type) throws Exception {
        if (type.isPrimitive()) {
            writePrimitive(buffer, value, type);
        } else if (type == String.class) {
            if (value == null) {
                buffer.writeByte((byte) 0);
            } else {
                buffer.writeByte((byte) 1);
                buffer.writeString((String) value);
            }
        } else if (type.isArray()) {
            writeArray(buffer, value, type);
        } else {
            // Handle nested object
            serializeObject(value, buffer);
        }
    }

    /**
     * Write a primitive value to the buffer.
     * 
     * @param buffer The ByteBuffer to write to.
     * @param value  The primitive value to write.
     * @param type   The type of the primitive value.
     */
    private static void writePrimitive(ByteBuffer buffer, Object value, Class<?> type) {
        if (type == int.class) {
            buffer.writeInt(((Number) value).intValue());
        } else if (type == long.class) {
            buffer.writeLong(((Number) value).longValue());
        } else if (type == double.class) {
            buffer.writeDouble(((Number) value).doubleValue());
        } else if (type == byte.class) {
            buffer.writeByte(((Number) value).byteValue());
        }
    }

    /**
     * Write an array to the buffer.
     * 
     * @param buffer The ByteBuffer to write to.
     * @param value  The array to write.
     * @param type   The type of the array.
     * @throws Exception If writing fails.
     */
    private static void writeArray(ByteBuffer buffer, Object value, Class<?> type) throws Exception {
        if (value == null) {
            buffer.writeInt(-1);
            return;
        }

        int length = java.lang.reflect.Array.getLength(value);
        buffer.writeInt(length);

        Class<?> componentType = type.getComponentType();
        for (int i = 0; i < length; i++) {
            Object element = java.lang.reflect.Array.get(value, i);
            writeValue(buffer, element, componentType);
        }
    }

    /*
     * Structure of the byte array:
     * 
     * Null marker
     * Object reference marker (Checks if object has been serialized before)
     * Object reference/handle (consists of the entire object's fieldname, fieldtype
     * + data.)
     * 
     * In each object reference/handle:
     * ClassName, field_lengths, fieldname, fieldType,fieldVal
     */

    /**
     * Deserialize a byte array into an object.
     * 
     * @param data The byte array to deserialize.
     * @return The deserialized object.
     * @throws Exception If deserialization fails.
     */
    public static Object deserialize(byte[] data) throws Exception {
        // Parity checking
        byte receivedParityBit = data[data.length - 1];
        byte[] actualData = new byte[data.length - 1];
        System.arraycopy(data, 0, actualData, 0, data.length - 1);
        // Calculate and verify parity
        byte calculatedParity = Parity.calculateEvenParityBit(actualData);
        if (calculatedParity != receivedParityBit) {
            throw new Exception("Parity check failed: data corruption detected");
        }

        // Deserialization
        deserializedObjects.clear();
        objectCounter = 0;
        ByteReader reader = new ByteReader(actualData);
        return deserializeObject(reader);
    }

    /*
     * getClassFromName(String className) is a Helper function for
     * deserializeObject().
     * 
     * Primitive class types cannot get the class name using Class.forName method
     * during deserialization
     */

    /**
     * Get the class from its name, handling primitive types.
     * 
     * @param className The name of the class.
     * @return The Class object.
     * @throws ClassNotFoundException If the class cannot be found.
     */
    private static Class<?> getClassFromName(String className) throws ClassNotFoundException {
        // Handle primitive types
        switch (className) {
            case "int":
                return int.class;
            case "long":
                return long.class;
            case "double":
                return double.class;
            case "float":
                return float.class;
            case "boolean":
                return boolean.class;
            case "byte":
                return byte.class;
            case "short":
                return short.class;
            case "char":
                return char.class;
            case "void":
                return void.class;
            default:
                return Class.forName(className);
        }
    }

    /**
     * Deserialize an individual object from the reader.
     * 
     * @param reader The ByteReader to read data from.
     * @return The deserialized object.
     * @throws Exception If deserialization fails.
     */
    private static Object deserializeObject(ByteReader reader) throws Exception {
        byte nullMarker = reader.readByte();
        if (nullMarker == 0) {
            return null;
        }

        // check if object has been deserialized before.
        byte referenceMarker = reader.readByte();
        if (referenceMarker == 1) {
            int objectId = reader.readInt();
            return deserializedObjects.get(objectId);
        }

        // Read class metadata
        String className = reader.readString();
        Class<?> cls = getClassFromName(className);

        // Handle primitive types directly
        if (cls.isPrimitive()) {
            Object value = readPrimitive(reader, cls);
            deserializedObjects.put(objectCounter++, value);
            return value;
        }

        Object obj = cls.getDeclaredConstructor().newInstance();

        // Store object before deserializing fields to handle circular references
        deserializedObjects.put(objectCounter++, obj);

        int fieldCount = reader.readInt();

        // First read all field metadata
        java.util.List<java.lang.reflect.Field> fields = new java.util.ArrayList<>();
        java.util.List<Class<?>> fieldTypes = new java.util.ArrayList<>();

        for (int i = 0; i < fieldCount; i++) {
            String fieldName = reader.readString();
            String fieldTypeName = reader.readString();

            java.lang.reflect.Field field = cls.getDeclaredField(fieldName);
            field.setAccessible(true);

            Class<?> fieldType = getClassFromName(fieldTypeName);

            fields.add(field);
            fieldTypes.add(fieldType);
        }

        // Then read all field values
        for (int i = 0; i < fieldCount; i++) {
            java.lang.reflect.Field field = fields.get(i);
            Class<?> fieldType = fieldTypes.get(i);

            // Skip if field is static or transient
            if (!java.lang.reflect.Modifier.isStatic(field.getModifiers()) &&
                    !java.lang.reflect.Modifier.isTransient(field.getModifiers())) {
                // Read field value
                Object fieldValue = readValue(reader, fieldType);
                field.set(obj, fieldValue);
            }
        }

        return obj;
    }

    /**
     * Read a value from the reader based on its type.
     * 
     * @param reader The ByteReader to read from.
     * @param type   The type of the value.
     * @return The read value.
     * @throws Exception If reading fails.
     */
    private static Object readValue(ByteReader reader, Class<?> type) throws Exception {
        if (type.isPrimitive()) {
            return readPrimitive(reader, type);
        } else if (type == String.class) {
            byte nullMarker = reader.readByte();
            return nullMarker == 0 ? null : reader.readString();
        } else if (type.isArray()) {
            return readArray(reader, type);
        } else {
            return deserializeObject(reader);
        }
    }

    /**
     * Read a primitive value from the reader.
     * 
     * @param reader The ByteReader to read from.
     * @param type   The type of the primitive value.
     * @return The read primitive value.
     */
    private static Object readPrimitive(ByteReader reader, Class<?> type) {

        if (type == int.class) {
            return reader.readInt();
        } else if (type == long.class) {
            return reader.readLong();
        } else if (type == double.class) {
            return reader.readDouble();
        } else if (type == boolean.class) {
            return reader.readByte() == 1;
        }
        return null;
    }

    /**
     * Read an array from the reader.
     * 
     * @param reader The ByteReader to read from.
     * @param type   The type of the array.
     * @return The read array.
     * @throws Exception If reading fails.
     */
    private static Object readArray(ByteReader reader, Class<?> type) throws Exception {
        int length = reader.readInt();
        if (length == -1) {
            return null;
        }

        Object array = java.lang.reflect.Array.newInstance(type.getComponentType(), length);
        for (int i = 0; i < length; i++) {
            java.lang.reflect.Array.set(array, i, readValue(reader, type.getComponentType()));
        }
        return array;
    }
}
