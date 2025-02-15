package SC4051_dist_system_proj;

import SC4051_dist_system_proj.utils.*;

public class Main {
    enum Day {
        SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY;
    }

    public static class Address {
        private String street;
        private String city;
        private String country;

        public Address(String street, String city, String country) {
            this.street = street;
            this.city = city;
            this.country = country;
        }

        public Address() {
        } // Required for deserialization

        public String getStreet() {
            return this.street;
        }
    }

    public static class Person {
        private String name;
        private Address address;
        private Person spouse; // Nested reference to another Person

        public Person(String name, Address address) {
            this.name = name;
            this.address = address;
        }

        public Person() {
        } // Required for deserialization

        public void setSpouse(Person spouse) {
            this.spouse = spouse;
        }

        public String getName() {
            return this.name;
        }

        public String getSpouse() {
            return this.spouse.getName();
        }

        public String getAddress() {
            return this.address.getStreet();
        }
    }

    public static class RequestMessage {
        // private Integer requestType;
        private int requestID;
        private String data;

        public RequestMessage() {
        } // Required for deserialization

        // public RequestMessage(Integer requestType, Integer requestID, String data) {
        // this.requestType = requestType;
        // this.requestID = requestID;
        // this.data = data;
        // }

        public RequestMessage(int requestID, String data) {
            this.requestID = requestID;
            this.data = data;

        }

        public int getRequestID() {
            return this.requestID;
        }

        public String getData() {
            return this.data;
        }
    }

    public static void main(String[] args) {

        RequestMessage requestMessage = null;
        byte[] data;
        RequestMessage deserialised_data = null;

        try {
            // Create a new request message
            requestMessage = new RequestMessage(1, "Hello server");
            System.out.println("created msg");
            data = Serializer.serialize(requestMessage);
            System.out.println("serialized");

            deserialised_data = (RequestMessage) Serializer.deserialize(data);
            System.out.println("deserialized: " + deserialised_data.getRequestID());
        } catch (Exception e) {
            e.printStackTrace();
        }

        // Address address1 = new Address("123 Main St", "Boston", "USA");
        // Address address2 = new Address("456 Oak St", "Chicago", "USA");

        // Person person1 = new Person("John", address1);
        // Person person2 = new Person("Jane", address2);

        // System.out.println(person1.name + " lives at " + person1.address.street);

        // // Create circular reference
        // person1.setSpouse(person2);
        // person2.setSpouse(person1);

        // // Serialize
        // byte[] data;

        // try {
        // data = Serializer.serialize(person1);
        // System.out.println("This is data: " + data);
        // Person deserializedPerson = (Person) Serializer.deserialize(data);
        // System.out.println(deserializedPerson.getName());
        // System.out.println(deserializedPerson.getSpouse());
        // System.out.println(deserializedPerson.getAddress());
        // } catch (Exception e) {
        // // TODO Auto-generated catch block
        // e.printStackTrace();
        // }

    }
}
