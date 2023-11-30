#include <SPI.h>
#include <WiFiNINA.h>
#include "config.h"
#include <Arduino_MKRIoTCarrier.h>
#include <Firebase_Arduino_WiFiNINA.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
char ssid[] = SECRET_SSID; // Network SSID 
char pass[] = SECRET_PASS; // Network password

MKRIoTCarrier carrier;
FirebaseData fbdo;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0); // NTP server address and time offset (adjust time offset as needed)

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for the serial port to connect

  // Connect to Wi-Fi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  // Initialize NTP Client
  timeClient.begin();
  timeClient.setTimeOffset(3600); // Set the time offset for your timezone

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_SECRET, ssid, pass);
  carrier.withCase();
  carrier.begin();
}

void sendDataToFirebase() {
  timeClient.update();
  unsigned long timestamp = timeClient.getEpochTime(); // Get current Unix timestamp

  // Read sensor data
  float temperature = carrier.Env.readTemperature();
  float humidity = carrier.Env.readHumidity();
  float pressure = carrier.Pressure.readPressure();
  int moisture = analogRead(A0);
  float gasResistor = carrier.AirQuality.readGasResistor();
  float volatileOrganicCompounds = carrier.AirQuality.readVOC();
  float co2 = carrier.AirQuality.readCO2();


  // Create a unique path for each set of readings using a timestamp
  String path = "sensorReadings/" + String(timestamp);

  // Send data to Firebase
  // Firebase.setFloat(fbdo, path + "/temperature", temperature);
  // Firebase.setFloat(fbdo, path + "/humidity", humidity);
  // Firebase.setFloat(fbdo, path + "/pressure", pressure);
  // Firebase.setInt(fbdo, path + "/moisture", moisture);
}

void loop() {
int pirState = analogRead(A6); // Read PIR value
Serial.println(pirState);
  if (pirState == HIGH) {
    // Motion detected
    Serial.println("Motion detected!");
  } else {
    // No motion detected
    Serial.println("No motion.");
  }
  sendDataToFirebase();
  delay(1000); // Adjust the delay as needed
}
