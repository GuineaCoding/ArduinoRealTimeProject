#include <SPI.h>
#include <WiFiNINA.h>
#include "config.h"
#include <Arduino_MKRIoTCarrier.h>
#include <Firebase_Arduino_WiFiNINA.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>


char ssid[] = SECRET_SSID; // Network SSID
char pass[] = SECRET_PASS; // Network password

MKRIoTCarrier carrier;
FirebaseData fbdo;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0); // NTP server address and time offset (adjust time offset as needed)

// PIR Sensor
const int pirPin = A6; 
bool pirState = LOW;
uint32_t colorGreen = carrier.leds.Color(0, 50, 0); // Green
uint32_t colorRed = carrier.leds.Color(50, 0, 0);   // Red

bool motionDetected = false;
unsigned long motionDetectedTime = 0;
const unsigned long redLightDuration = 30000; // 30 seconds
uint8_t gesture = carrier.Light.readGesture(); 


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
  timeClient.setTimeOffset(3600); 

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_SECRET, ssid, pass);
    // Initialize PIR sensor
  pinMode(pirPin, INPUT);
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
  Serial.print("Gas Resistor: "); Serial.println(gasResistor);
  Serial.print("VOCs: "); Serial.println(volatileOrganicCompounds);
  Serial.print("CO2: "); Serial.println(co2);

  // Create a unique path for each set of readings using a timestamp
  String path = "sensorReadings/" + String(timestamp);

  // Send data to Firebase
  // Firebase.setFloat(fbdo, path + "/temperature", temperature);
  // Firebase.setFloat(fbdo, path + "/humidity", humidity);
  // Firebase.setFloat(fbdo, path + "/pressure", pressure);
  // Firebase.setInt(fbdo, path + "/moisture", moisture);
}



void loop() {
  sendDataToFirebase();
  carrier.Buttons.update();

  if (carrier.Light.gestureAvailable()) {
    uint8_t gesture = carrier.Light.readGesture();
    Serial.println(gesture);

    if (gesture == UP || carrier.Buttons.onTouchDown(TOUCH0)) {
      displayDateTimeTempHumidity();
    }
  }

  pirState = digitalRead(pirPin);
  Serial.println(pirState);

  if (pirState == HIGH) {
    if (!motionDetected) {
      motionDetected = true;
      motionDetectedTime = millis();
      // displayMessage("Warning! Movement detected.");
      // carrier.leds.fill(colorRed, 0, 5);
    }
  } else {
    if (motionDetected && millis() - motionDetectedTime > redLightDuration) {
      motionDetected = false;
      // displayMessage("No movement detected. Everything is good here.");
      // carrier.leds.fill(colorGreen, 0, 5);
    }
  }
  carrier.leds.show();

  delay(1000); 
}

void displayMessage(const char* message) {
  Serial.println(message);
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setCursor(30, 100);
  carrier.display.setTextSize(2);
  carrier.display.println(message);
}


void displayDateTimeTempHumidity() {
  timeClient.update();

  // Get the current Unix timestamp and convert to time structure
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime(&rawtime);

  char currentDate[11]; // Buffer to store the formatted date
  strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", ti); // Format date as YYYY-MM-DD

  String currentTime = timeClient.getFormattedTime();

  float temperature = carrier.Env.readTemperature();
  float humidity = carrier.Env.readHumidity();

  // Clear the screen and set background color
  carrier.display.fillScreen(0x07E0);

  carrier.display.setTextColor(0x001F);
  carrier.display.setTextSize(2);

  carrier.display.setCursor(10, 20);
  carrier.display.println(currentDate);
  carrier.display.setCursor(10, 50);
  carrier.display.println(currentTime);
}