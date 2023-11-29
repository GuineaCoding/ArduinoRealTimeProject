#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include "config.h"
#include <Arduino_MKRIoTCarrier.h>

char ssid[] = SECRET_SSID; //  network SSID 
char pass[] = SECRET_PASS; //  network password

MKRIoTCarrier carrier;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // waiting for Serial port to connect.
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  carrier.begin();
}

const char* firebaseHost = FIREBASE_HOST;
const int httpPort = 443;
WiFiSSLClient wifiClient;
HttpClient client = HttpClient(wifiClient, firebaseHost, httpPort);
String contentType = "application/json";


void sendDataToFirebase(String path, String data) {
  client.beginRequest();
  client.post(path, contentType, data);
  client.endRequest();
  
  // Read the response and print it
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
}

void loop() {
  // Read sensor data
  float temperature = carrier.Env.readTemperature();
  float humidity = carrier.Env.readHumidity();
  float pressure = carrier.Pressure.readPressure();
  int moisture = analogRead(A0); =
  // Convert sensor data to JSON string
  String data = "{\"temperature\": " + String(temperature) + 
                ", \"humidity\": " + String(humidity) + 
                ", \"pressure\": " + String(pressure) + 
                ", \"moisture\": " + String(moisture) + "}";
  String firebasePath = "/sensors.json"; // .json is important

  // Send data to Firebase
  sendDataToFirebase(firebasePath, data);

  delay(10000); 
}