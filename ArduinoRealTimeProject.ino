#include <SPI.h>
#include <WiFiNINA.h>
#include "config.h"
#include <Arduino_MKRIoTCarrier.h>
#include <Firebase_Arduino_WiFiNINA.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>
#include "icons.h"
#include <TimeLib.h>


char ssid[] = SECRET_SSID; // Network SSID
char pass[] = SECRET_PASS; // Network password

MKRIoTCarrier carrier;
FirebaseData fbdo;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP); 

// PIR Sensor
const int pirPin = A6; 
bool pirState = LOW;
uint32_t colorGreen = carrier.leds.Color(0, 50, 0); // Green
uint32_t colorRed = carrier.leds.Color(50, 0, 0);   // Red


bool motionDetected = false;
unsigned long motionDetectedTime = 0;
const unsigned long redLightDuration = 30000; // 30 seconds
uint8_t gesture = carrier.Light.readGesture(); 


unsigned long displayStartMillis = 0; // Global variable to store start time
const unsigned long displayDuration = 20000; // Display duration in milliseconds (20 seconds)
bool displayActive = false; // Flag to indicate whether the display is active



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

  Serial.println("Time synchronized");
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
  Firebase.setFloat(fbdo, path + "/temperature", temperature);
  Firebase.setFloat(fbdo, path + "/humidity", humidity);
  Firebase.setFloat(fbdo, path + "/pressure", pressure);
  Firebase.setInt(fbdo, path + "/moisture", moisture);
}



void loop() {
  // sendDataToFirebase();

  //   if (Firebase.getBool(fbdo, "ledCommand/red")) {
  //   if (fbdo.dataType() == "boolean") {
  //     bool red = fbdo.boolData();
  //     if (red) {
  //       carrier.display.fillScreen(0x07E0);
  //     } else {
  //       carrier.display.fillScreen(0x0000FF);
  //     }
  //     carrier.leds.show();
  //   }
  // }
  carrier.Buttons.update(); // Update the status of the touch buttons

  // Check for the "UP" gesture
  if (carrier.Light.gestureAvailable()) {
    uint8_t gesture = carrier.Light.readGesture();
    Serial.println(gesture);

    if (gesture == UP) { // 
    carrier.Buzzer.beep(800, 20);
      displayDateTimeTempHumidity();
    }
  }

    // Check if button 0 is pressed
  if (carrier.Buttons.onTouchDown(TOUCH0)) {
    carrier.Buzzer.beep(800, 20);
    displayDateTimeTempHumidity();
  }

      if (displayActive && (millis() - displayStartMillis >= displayDuration)) {
        carrier.display.fillScreen(ST77XX_BLACK); // Turn the display black
        displayActive = false; // Reset the display active flag
    }

  // pirState = digitalRead(pirPin);
  // Serial.println(pirState);

  // if (pirState == HIGH) {
  //   if (!motionDetected) {
  //     motionDetected = true;
  //     motionDetectedTime = millis();
  //     // displayMessage("Warning! Movement detected.");
  //     carrier.leds.fill(colorRed, 0, 5);
  //   }
  // } else {
  //   if (motionDetected && millis() - motionDetectedTime > redLightDuration) {
  //     motionDetected = false;
  //     // displayMessage("No movement detected. Everything is good here.");
  //     carrier.leds.fill(colorGreen, 0, 5);
  //   }
  // }
  // carrier.leds.show();

  delay(1000); 
}

// void displayMessage(const char* message) {
//   Serial.println(message);
//   carrier.display.fillScreen(ST77XX_BLACK);
//   carrier.display.setCursor(30, 100);
//   carrier.display.setTextSize(2);
//   carrier.display.println(message);
// }


void displayDateTimeTempHumidity() {
    carrier.display.setRotation(180);
    timeClient.update();
    String greeting;

    // Get the current Unix timestamp and convert to time structure
    time_t rawtime = timeClient.getEpochTime();
    struct tm *ti = localtime(&rawtime);

    char currentDate[11]; // Buffer to store the formatted date
    strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", ti); // Format date as YYYY-MM-DD

    String currentTime = timeClient.getFormattedTime();

    float temperature = carrier.Env.readTemperature();
    float humidity = carrier.Env.readHumidity();

    // Display dimensions and bitmap size
    int displayWidth = carrier.display.width();
    int displayHeight = carrier.display.height();
    int bitmapSize = 48; // Size of the  images
    int spacing = 10; // Spacing between elements

    // Text size settings
    carrier.display.setTextSize(2);
    int charWidth = 6 * 2; 
    int charHeight = 8 * 2; 

    // Define the gradient color based on the time of day
    int currentHour = timeClient.getHours();
    uint8_t r, g, b;
    for (int y = 0; y < displayHeight; y++) {
        if (currentHour >= 6 && currentHour < 12) {
            // Morning: Sunny gradient
            greeting = "Good Morning";
            r = map(y, 0, displayHeight - 1, 255, 255);
            g = map(y, 0, displayHeight - 1, 200, 100);
            b = map(y, 0, displayHeight - 1, 50, 0);
        } else if (currentHour >= 12 && currentHour <= 18) {
            // Afternoon: Orange gradient
            greeting = "Good Afternoon";
            r = map(y, 0, displayHeight - 1, 255, 204);
            g = map(y, 0, displayHeight - 1, 128, 50);
            b = map(y, 0, displayHeight - 1, 0, 0);
        } else {
            // Evening/Night: Darker gradient
            greeting = "Good Evening";
            r = map(y, 0, displayHeight - 1, 0, 50);
            g = map(y, 0, displayHeight - 1, 0, 50);
            b = map(y, 0, displayHeight - 1, 100, 150);
        }
        carrier.display.drawFastHLine(0, y, displayWidth, carrier.display.color565(r, g, b));
    }

    int yPos = 30; // Starting Y position

    // Draw greeting text
    int textX = (displayWidth - greeting.length() * charWidth) / 2;
    carrier.display.setCursor(textX, yPos);
    carrier.display.println(greeting);
    yPos += charHeight + spacing;

    // Display date 
    carrier.display.drawBitmap(10, yPos, m_calendar, bitmapSize, bitmapSize, ST77XX_WHITE);
    textX = 10 + bitmapSize + 5;
    carrier.display.setCursor(textX, yPos + (bitmapSize - charHeight) / 2);
    carrier.display.println(currentDate);
    yPos += bitmapSize + spacing;

    // Display time w
    carrier.display.drawBitmap(10, yPos, m_clock, bitmapSize, bitmapSize, ST77XX_WHITE);
    textX = 10 + bitmapSize + 5;
    carrier.display.setCursor(textX, yPos + (bitmapSize - charHeight) / 2);
    carrier.display.println(currentTime);
    yPos += bitmapSize + spacing;

    // Display temperature 
    String tempString = String(temperature) + " C";
    carrier.display.drawBitmap(10, yPos, m_thermostat, bitmapSize, bitmapSize, ST77XX_WHITE);
    textX = 10 + bitmapSize + 5;
    carrier.display.setCursor(textX, yPos + (bitmapSize - charHeight) / 2);
    carrier.display.println(tempString);
    yPos += bitmapSize + spacing;

    // Display humidity 
    String humidityString = "Hum: " + String(humidity) + "%";
    carrier.display.drawBitmap(10, yPos, m_humidity, bitmapSize, bitmapSize, ST77XX_WHITE);
    textX = 10 + bitmapSize + 5;
    carrier.display.setCursor(textX, yPos + (bitmapSize - charHeight) / 2);
    carrier.display.println(humidityString);

    displayActive = true; // Set the display active flag
    displayStartMillis = millis(); // Record the start time
}

