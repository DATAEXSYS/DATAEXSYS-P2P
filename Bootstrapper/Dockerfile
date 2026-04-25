# Stage 1: Build the application with Maven
FROM eclipse-temurin:21-jdk-alpine AS build

WORKDIR /app

# Copy Maven wrapper and project files
COPY Bootstrapper/mvnw Bootstrapper/pom.xml ./
COPY Bootstrapper/.mvn .mvn

# Make Maven wrapper executable
RUN chmod +x mvnw

# Download dependencies (cached layer)
RUN ./mvnw dependency:resolve -B

# Copy source code
COPY Bootstrapper/src src

# Build the application (skip tests for faster builds)
RUN ./mvnw package -DskipTests -B

# Stage 2: Runtime image
FROM eclipse-temurin:21-jre-alpine

WORKDIR /app

COPY --from=build /app/target/globalNode-0.0.1-SNAPSHOT.jar app.jar

EXPOSE 8080

CMD ["java", "-jar", "app.jar"]