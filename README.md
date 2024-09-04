# ESP32-Smart-Medibox

## Overview
The ESP32 Smart Medibox is an IoT-based medication management system designed to improve adherence to prescribed medication schedules. This project utilizes the ESP32 microcontroller along with sensors, actuators, and communication modules to ensure timely reminders, environmental monitoring, and user interaction. The Medibox monitors environmental conditions such as temperature, humidity, and light intensity, while also providing scheduled alerts for medication intake. With WiFi connectivity and customizable settings, this project enhances medication management through automation and remote access.

## Features:
- WiFi Connectivity: Enables remote monitoring and control via MQTT.
- Temperature & Humidity Monitoring: Measures environmental conditions using the DHT22 sensor.
- Light Intensity Measurement: Uses LDRs to adjust the servo motor based on ambient light.
- Servo Motor Control: Adjusts a shaded window according to light conditions.
- Buzzer Notifications: Alerts users when it’s time to take medication.
- User Interaction: Buttons for confirming actions and canceling alarms.
- Customizable Settings: Parameters like the servo motor's angle can be adjusted via MQTT.
## Components Used:
- ESP32: Main microcontroller.
- DHT22: Temperature and humidity sensor.
- LDRs: Measure light intensity.
- Servo Motor: Controls the position of the window.
- Buzzer: Provides notification alerts.
- Buttons: Allow user interaction.

## Project Structure
The repository is divided into two main folders:
- Smart Medibox:
  - Contains the initial version of the project.
  - Features include:
    - Temperature and humidity monitoring.
    - Buzzer alerts if environmental conditions are outside healthy levels.
    - User-configurable alarms and options to stop alarms.

  <div>
    <img src="https://github.com/sanuda-ats/ESP32-Smart-Medibox/blob/main/smart%20medibox/Circuit%20Diagram.png">
  </div>
- Smart Medibox - With Node-RED
  - This folder includes the IoT-enabled version of the Medibox.
  - Additional Features:
    - Light intensity control using LDRs and servo motor.
    - Node-RED dashboard for enhanced visualization and control.
  - The node-red-flow.json file contains the Node-RED flow for the dashboard and MQTT communication.
  <div>
    <img src="https://github.com/sanuda-ats/ESP32-Smart-Medibox/blob/main/smart%20medibox%20with%20nodered/Circut%20Diagram.png">
  </div>

## How It Works:
- Environmental Monitoring:
  The ESP32 continuously monitors temperature, humidity, and light intensity. If the temperature or humidity exceeds predefined thresholds, a buzzer alerts the user. Additionally, the light intensity is monitored   to adjust the servo motor, which controls a shaded window to maintain optimal light conditions.

- Medication Alerts:
  Scheduled alerts for medication intake are managed by the ESP32, which triggers a buzzer at the specified times. The user can interact with the Medibox using buttons to confirm actions or cancel alarms.

- Remote Access & Control:
  With WiFi connectivity, users can remotely manage the Medibox, monitor its status, and adjust settings via MQTT. The Node-RED dashboard provides an intuitive interface for interacting with the Medibox from anywhere.

## Node-RED Integration:
The Node-RED flow, provided in the node-red-flow.json file, integrates with the Medibox to enable remote control and visualization of environmental data. The dashboard allows users to:
  - Monitor temperature, humidity, and light intensity in real-time.
  - Set and adjust medication schedules.
  - Control the servo motor and other components.
<div>
  <img src="https://github.com/sanuda-ats/ESP32-Smart-Medibox/blob/main/smart%20medibox%20with%20nodered/Node-RED%20flows.png">
</div>

# Contributing
Contributions are welcome! Please fork the repository and submit a pull request for review.

# License
This project is licensed under the MIT License. See the [LICENSE](https://choosealicense.com/licenses/mit/) file for more details.

