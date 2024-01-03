# Project Readme: Smart Environment Monitoring System with Firebase Integration

## Table of Contents
1. [Introduction](#introduction)
2. [Features](#features)
3. [Main Requirements](#main-requirements)
4. [Project Dependencies](#project-dependencies)
5. [Setup and Configuration](#setup-and-configuration)
6. [System Parameters](#system-parameters)
7. [Core functions in the projetw](#core-functions-in-the-projet)
8. [Function Descriptions](#function-descriptions)
9. [Usage Instructions](#usage-instructions)
10. [Troubleshooting](#troubleshooting)
11. [Documentation](#documentation)

## Introduction
This project involves creating a Smart Environment Monitoring System using Arduino MKR IoT Carrier 2. The system is capable of sensing environmental parameters and sending the data to Firebase for storage and further analysis. It's an IoT project that exemplifies the integration of hardware sensors with cloud-based data management.

Please be aware that the project consists of three parts:

Part 1: Arduino code functionality<br>
Part 2: Firebase Database<br>
Part 3: Ionic app and can be accessed here: <a href="https://github.com/GuineaCoding/ArduinoDevProject">https://github.com/GuineaCoding/ArduinoDevProject</a>
 
## Features
- **Real-time Environment Monitoring**: Measures temperature, humidity, air pressure, and air quality (CO2 levels, VOCs).
- **Motion Detection**: Utilizes PIR sensors for motion detection with custom alert systems.
- **Firebase Integration**: Seamlessly sends sensor data to Firebase for real-time monitoring and analytics.
- **Gesture Control**: Includes gesture recognition for interactive control.
- **OLED Display**: Shows environmental data and system status on an integrated OLED display.
- **Holiday Mode**: Special mode for enhanced security and alert systems during holidays.

## Main Requirements
- Arduino MKR IoT Carrier 
- PIR Motion Sensor
- Various environmental sensors (included in the MKR IoT Carrier 2)
- Active Internet Connection

## Project Dependencies
- Arduino IDE
- SPI.h
- WiFiNINA.h
- Firebase_Arduino_WiFiNINA.h
- Arduino_MKRIoTCarrier.h
- NTPClient.h
- TimeLib.h

## Setup and Configuration
1. **Hardware Setup**: Connect all the sensors to the Arduino MKR IoT Carrier 2.
2. **WiFi Configuration**: Enter your WiFi credentials in `config.h`.
3. **Firebase Setup**: Create config.h file, create a Firebase project and obtain the database URL and secret key. Update these in `config.h`.
4. **Arduino IDE Setup**: Install the Arduino IDE and all required libraries.

## System Parameters
The system involves sensors interfaced with the Arduino MKR IoT Carrier 2, which connects to the internet via WiFi. The data from sensors is sent to Firebase for storage and analysis.

## Core functions in the projet
The code is structured into several key functions:
- `setup()`: Initializes the hardware and network connections.
- `sendDataToFirebase()`: Gathers sensor data and sends it to Firebase.
- `loop()`: Main loop handling sensor readings, display updates, and Firebase communication.
- `displayDateTimeTempHumidity()`: Manages the OLED display for showing date, time, and environmental data.
- `holidayMode()`: Activates special alert and monitoring mode.

## Function Descriptions
- **setup()**: Establishes serial communication, WiFi connection, and initializes all sensors and Firebase.
- **sendDataToFirebase()**: Formats and sends environmental data to Firebase.
- **loop()**: Regularly checks for sensor updates and controls the display and Firebase data upload.
- **displayDateTimeTempHumidity()**: Updates the OLED display with the current time, date, and environmental readings.(smart watches)
- **holidayMode()**: Monitors the PIR sensor and alters system behavior based on Firebase's 'holidayMode' status.

## Usage Instructions
1. Power up the Arduino MKR IoT Carrier 2.
2. Ensure it's connected to the WiFi network.
3. Observe the sensor readings on the firbase database or ionic app.
4. Access the Firebase console to view real-time data.
5. Access ionic app to view real-time data

## Troubleshooting
- **WiFi Connectivity Issues**: Ensure the WiFi credentials are correct and the network is stable.
- **Firebase Communication Errors**: Verify Firebase secret and host if it's correct

## Documentation

<p>Helpful Resources and Troubleshooting Guides which helped me in bad situations</p>

- [Cheat sheet for MKR IoT Carrier Rev2](https://docs.arduino.cc/tutorials/mkr-iot-carrier-rev2/cheat-sheet): Provides a quick reference for using the MKR IoT Carrier Rev2, which is helpful for initial setup and common functions.
  
- [Image Convertor Tool](https://javl.github.io/image2cpp/): Useful for converting images into formats compatible with Arduino displays.

- [Reducing the Sensitivity of a PIR Sensor](https://electronics.stackexchange.com/questions/30981/reducing-the-sensitivity-of-a-pir-sensor): Offers insights into adjusting the sensitivity of PIR sensors, which is crucial for motion detection accuracy.

- [Troubleshooting PIR Sensor Output Issues](https://forum.arduino.cc/t/pir-sensor-output-toggled-always-even-noone-near-to-sensor/996894/8): Discussion on Arduino Forum about resolving erratic PIR sensor outputs.

- [Adjusting PIR Sensor Sensitivity on Arduino](https://support.arduino.cc/hc/en-us/articles/4403050020114-Troubleshooting-PIR-Sensor-and-sensitivity-adjustments): Official guide on calibrating PIR sensors connected to Arduino, which helped in fine-tuning motion detection.

- [Sending Data to Firebase using Arduino Uno WiFi Rev2](https://www.cytron.io/tutorial/send-data-to-firebase-using-arduino-uno-wifi-rev2): A tutorial on setting up Firebase connectivity, which was instrumental in implementing real-time data syncing.

- [Resolving 'ArduinoJson.h No Such File or Directory' Error](https://stackoverflow.com/questions/50543488/arduinojson-h-no-such-file-or-directory-arduino): A StackOverflow discussion that helped fix library inclusion errors in the Arduino IDE.

- [Real-Time Arduino to Android Communication with Firebase](https://rashm1n.medium.com/arduino-to-android-real-time-communication-for-iot-with-firebase-60df579f962): A Medium article that provided insights into establishing a communication link between Arduino and Android apps via Firebase.

- [Solving Port Monitor Error on Arduino](https://forum.arduino.cc/t/port-monitor-error-command-open-failed-invalid-serial-port-could-not-connect-to-com-serial-port/1127713): A forum thread that helped address issues related to serial port connections in Arduino projects.

- [Fixing Upload Errors on Arduino](https://forum.arduino.cc/t/failed-uploading-uploading-error-exit-status-1/1177663): Useful for troubleshooting problems encountered during sketch uploads to Arduino boards.

- [Resolving 'No Device Found on ttyACM0'](https://forum.arduino.cc/t/can-no-longer-upload-sketch-to-board-no-device-found-on-ttyacm0/690382): A discussion on how to fix device recognition issues, especially on Linux-based systems.

- [Interpreting Gas Resistor and VOC Readings](https://forum.arduino.cc/t/need-help-interpreting-the-values-of-gas-resistor-and-volatile-organic-compounds-read-from-the-arduino-mkriotcarrier-library/1144148): A useful thread for understanding and utilizing gas sensor readings from the MKR IoT Carrier.

- [Bosch BME688 Datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme688-ds000.pdf): The datasheet for the BME688 sensor, providing detailed specifications and usage information.

- [Coding Help for MKR IoT Carrier Rev2](https://forum.arduino.cc/t/a-code-for-mkr-iot-carrier-rev2-please-help/1109215): A forum post discussing coding challenges and solutions for the MKR IoT Carrier Rev2.

- [MKR IoT Carrier Rev2 Operating System](https://forum.arduino.cc/t/mkr-iot-carrier-rev2-os-operating-system/1106706): An innovative project presenting an operating system concept for the MKR IoT Carrier Rev2.
