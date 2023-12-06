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


char ssid[] = SECRET_SSID;  // Network SSID
char pass[] = SECRET_PASS;  // Network password

MKRIoTCarrier carrier;
FirebaseData fbdo;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// PIR Sensor
const int pirPin = A6;
bool pirState = LOW;
uint32_t colorGreen = carrier.leds.Color(0, 50, 0);  // Green
uint32_t colorRed = carrier.leds.Color(50, 0, 0);    // Red


bool motionDetected = false;
unsigned long motionDetectedTime = 0;
const unsigned long redLightDuration = 30000;  // 30 seconds
uint8_t gesture = carrier.Light.readGesture();


unsigned long displayStartMillis = 0;         // Global variable to store start time
const unsigned long displayDuration = 20000;  // Display duration in milliseconds (20 seconds)
bool displayActive = false;                   // Flag to indicate whether the display is active



void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;  // Wait for the serial port to connect

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
  unsigned long timestamp = timeClient.getEpochTime();  // Get current Unix timestamp

  // Serial.println("Time synchronized");
  // Read sensor data
  float temperature = carrier.Env.readTemperature();
  float humidity = carrier.Env.readHumidity();
  float pressure = carrier.Pressure.readPressure();
  int moisture = analogRead(A0);
  float gasResistor = carrier.AirQuality.readGasResistor();
  float volatileOrganicCompounds = carrier.AirQuality.readVOC();
  float co2 = carrier.AirQuality.readCO2();
  pirState = digitalRead(pirPin);
  if (pirState == HIGH) {
  Serial.println(pirState);
    Serial.println(" ");
    Serial.println("High ");
  };
    pirState = digitalRead(pir);
  Serial.println(pirState);

    if (pirState == LOW) {
  Serial.println(pirState);
  };

  // Create a unique path for each set of readings using a timestamp
  String path = "sensorReadings/" + String(timestamp);

  // Send data to Firebase
  Firebase.setFloat(fbdo, path + "/temperature", temperature);
  Firebase.setFloat(fbdo, path + "/humidity", humidity);
  Firebase.setFloat(fbdo, path + "/pressure", pressure);
  Firebase.setInt(fbdo, path + "/moisture", moisture);
  Firebase.setInt(fbdo, path + "/gasResistor", gasResistor);
  Firebase.setInt(fbdo, path + "/volatileOrganicCompounds", volatileOrganicCompounds);
  Firebase.setInt(fbdo, path + "/co2", co2);
  Firebase.setInt(fbdo, path + "/timestamp", timestamp);
}



void loop() {
  sendDataToFirebase();

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
  carrier.Buttons.update();  // Update the status of the touch buttons

  // Check for the "UP" gesture
  if (carrier.Light.gestureAvailable()) {
    uint8_t gesture = carrier.Light.readGesture();
    Serial.println(gesture);

    if (gesture == UP) {  //
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
    carrier.display.fillScreen(ST77XX_BLACK);  // Turn the display black
    displayActive = false;                     // Reset the display active flag
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
  struct tm* ti = localtime(&rawtime);

  char currentDate[11];  // Buffer to store the formatted date
  strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", ti);  // Format date as YYYY-MM-DD

  String currentTime = timeClient.getFormattedTime();

  float temperature = carrier.Env.readTemperature();
  float humidity = carrier.Env.readHumidity();

  // Display dimensions and bitmap size
  int displayWidth = carrier.display.width();
  int displayHeight = carrier.display.height();
  int bitmapSize = 32;  // Size of the  images
  int spacing = 4;      // Spacing between elements

  // Text size settings
  carrier.display.setTextSize(2);
  int charWidth = 6 * 2;
  int charHeight = 8 * 2;

  // Define the gradient color based on the time of day
  int currentHour = timeClient.getHours();
  Serial.println(currentHour);
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
  int bitmapX = (displayWidth - bitmapSize) / 2;
  int greetingY = 10 + bitmapSize + spacing; // Position for the greeting text below the bitmap

  // Draw the appropriate bitmap for the time of day
  if (currentHour >= 6 && currentHour < 12) {
    carrier.display.drawBitmap(bitmapX, 10, m_sun, bitmapSize, bitmapSize, ST77XX_WHITE);
  } else if (currentHour >= 12 && currentHour <= 18) {
    carrier.display.drawBitmap(bitmapX, 10, m_sun_with_clouds, bitmapSize, bitmapSize, ST77XX_WHITE);
  } else {
    carrier.display.drawBitmap(bitmapX, 10, m_moon, bitmapSize, bitmapSize, ST77XX_WHITE);
  }

  // Draw greeting text
  int greetingX = (displayWidth - greeting.length() * charWidth) / 2;
  carrier.display.setCursor(greetingX, greetingY);
  carrier.display.println(greeting);

  int yPos = greetingY + charHeight + spacing; // Starting Y position for the icons and text

  // Constants for layout
  int iconWidth = bitmapSize; // Width of icons
  int gapBetweenIconAndText = 5; // Gap between the icon and the text

  // Iterate over each item (date, time, temperature, humidity)
  String items[] = { currentDate, currentTime, String(temperature) + " C", "Hum: " + String(humidity) + "%" };
  const uint8_t* icons[] = { m_calendarx32, m_clockx32, m_thermostatx32, m_humidityx32 };

  for (int i = 0; i < 4; i++) {
    String item = items[i];
    int textWidth = item.length() * charWidth; // Estimated text width
    int totalWidth = iconWidth + gapBetweenIconAndText + textWidth; // Total width of the row
    int startX = (displayWidth - totalWidth) / 2; // Starting X position to center the row

    // Draw icon and text
    carrier.display.drawBitmap(startX, yPos, icons[i], iconWidth, iconWidth, ST77XX_WHITE);
    carrier.display.setCursor(startX + iconWidth + gapBetweenIconAndText, yPos + (iconWidth - charHeight) / 2);
    carrier.display.println(item);

    yPos += iconWidth + spacing; // Update yPos for the next item
  }

  displayActive = true; // Set the display active flag
  displayStartMillis = millis(); // Record the start time
}
