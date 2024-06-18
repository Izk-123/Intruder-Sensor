#include <ArduinoJson.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Servo.h>
#include <NewPing.h>

#define trigPin 12    // Trig pin connected to Arduino pin 12
#define echoPin 13    // Echo pin connected to Arduino pin 13
#define MAX_DISTANCE 500  // Maximum distance for the sensor

NewPing sonar(trigPin, echoPin, MAX_DISTANCE);  // Initialize ultrasonic sensor
int greenLed = 3;   // Green LED connected to pin 3
int redLed = 2;     // Red LED connected to pin 2
Servo myservo;      // Create servo object to control a servo

// Network settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192, 168, 1, 100); // Replace with your server's IP address
EthernetClient client;

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  pinMode(trigPin, OUTPUT);  // Set trig pin as output
  pinMode(echoPin, INPUT);   // Set echo pin as input
  pinMode(greenLed, OUTPUT); // Set green LED as output
  pinMode(redLed, OUTPUT);   // Set red LED as output
  myservo.attach(9);         // Attach servo to pin 9

  // Start Ethernet
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Try a manual configuration (optional)
    Ethernet.begin(mac, IPAddress(192, 168, 1, 177));
  }
}

void sendToServer(float distance) {
  if (client.connect(server, 8000)) {
    Serial.println("Connected to server");
    StaticJsonDocument<200> doc;
    doc["distance"] = distance;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);
    
    client.println("POST /api/sensor-data/ HTTP/1.1");
    client.println("Host: http://127.0.0.1:8000/api/sensor-data/");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(strlen(jsonBuffer));
    client.println();
    client.println(jsonBuffer);
    client.stop();
  } else {
    Serial.println("Connection failed");
  }
}

void loop() {
  int duration, distance;

  // Send a ping
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Receive the echo
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distance = (duration / 2) / 29.1;
  Serial.print(distance);
  Serial.println(" cm");

  // If the distance is less than or equal to 15 cm
  if (distance <= 15) {
    digitalWrite(greenLed, LOW);  // Turn off green LED
    digitalWrite(redLed, HIGH);   // Turn on red LED
    myservo.write(180);  // Move servo arm to 180 degrees
    delay(450);
    digitalWrite(redLed, LOW);    // Turn off red LED
    myservo.write(90);   // Move servo arm to 90 degrees
    delay(450);
    digitalWrite(redLed, HIGH);   // Turn on red LED
    myservo.write(0);    // Move servo arm to 0 degrees
    delay(450);
    digitalWrite(redLed, LOW);    // Turn off red LED
    myservo.write(90);   // Move servo arm to 90 degrees
  } else {
    // If no object is detected within 15 cm
    digitalWrite(redLed, LOW);    // Turn off red LED
    digitalWrite(greenLed, HIGH); // Turn on green LED
    myservo.write(90);   // Move servo arm to 90 degrees (default position)
  }

  // Send distance data to the server
  sendToServer(distance);
  delay(450);  // Wait before the next loop
}
