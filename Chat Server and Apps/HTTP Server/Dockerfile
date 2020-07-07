FROM openjdk:12
WORKDIR /
ADD build/libs/myJar.jar myServerJar.jar
ADD resources/ resources/
EXPOSE 8080
CMD java -jar myServerJar.jar 