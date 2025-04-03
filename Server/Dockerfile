# You can use this Dockerfile to build a Docker image for the UDP server.
# Run this command after building the image: docker run -p 6789:6789/udp <your_image_name>

# Use an official OpenJDK 8 runtime as a parent image
FROM openjdk:8-jdk-slim as builder

# Set the working directory in the container
WORKDIR /app
RUN mkdir -p /app/bin

# Copy the current directory contents into the container at /app
COPY . /app

# Compile all Java files and specify the output directory for class files
RUN javac -d /app/bin $(find . -name "*.java")


# Use a smaller base image for the final image
FROM openjdk:8-jre-slim
# Set the working directory in the container
WORKDIR /app

# Copy the jar file from the builder stage to the final image
COPY --from=builder /app/bin /app

# Expose the port that the UDP server listens on
EXPOSE 6789

# Run the server when the container launches
CMD ["java", "Server.Server"]
