# ESP32-Smart-Medibox
The ESP32 Medibox is an intelligent, IoT-based solution that helps individuals manage their medication schedules with ease. Utilizing an ESP32 microcontroller, this project integrates sensors, actuators, and communication modules to ensure timely reminders, environmental monitoring, and user interaction. With features like remote control via MQTT and audio notifications, the Medibox is designed to support daily medication routines effectively.

# Overview
The ESP32 Medibox monitors environmental conditions such as temperature, humidity, and light intensity, while also providing scheduled alerts for medication intake. Built with WiFi connectivity and customizable settings, this project enhances medication management through automation and remote access.

## Features:
- WiFi Connectivity: Enables remote monitoring and control via MQTT.
- Temperature & Humidity Monitoring: Measures environmental conditions using the DHT22 sensor.
- Light Intensity Measurement: Uses LDRs to adjust the servo motor based on ambient light.
- Servo Motor Control: Adjusts a shaded window according to light conditions.
- Buzzer Notifications: Alerts users when it’s time to take medication.
- User Interaction: Buttons for confirming actions and canceling alarms.
- Customizable Settings: Parameters like the servo motor's angle can be adjusted via MQTT.
## Components:
- ESP32: Main microcontroller.
- DHT22: Temperature and humidity sensor.
- LDRs: Measure light intensity.
- Servo Motor: Controls the position of the window.
- Buzzer: Provides notification alerts.
- Buttons: Allow user interaction.
# How It Works:
The system continuously monitors environmental data and triggers alerts for scheduled medication times. Users can remotely manage the device, adjust settings, and monitor its status via MQTT. This flexibility makes the Medibox adaptable to various user needs.

# Node-RED Integration:
The Medibox interacts with Node-RED via MQTT for visualization and control. The Node-RED flow is available in the node-red-flow.json file.

# Getting Started:
To get started with the ESP32 Medibox, clone the repository and follow the detailed instructions for building, running, and simulating the project using Arduino IDE, PlatformIO, and Wokwi for simulation.
The Medibox interacts with Node-RED via MQTT for visualization and control. The Node-RED flow is available in the node-red-flow.json file.

