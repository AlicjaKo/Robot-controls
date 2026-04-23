# Robot Control System

## Overview

This project implements a web-based control system for a mobile robot using an ESP8266 and a microcontroller (e.g., Arduino). The robot can be controlled through a browser interface, enabling movement commands and heading adjustment via a compass.

---

## Features

* Forward and backward movement with fixed distances (5 cm, 20 cm)
* Compass-based rotation control (-360° to 360°)
* Automatic alignment to North (0°)
* Encoder-based distance tracking
* LCD output for status and diagnostics

---

## System Architecture

### Frontend

* `index.html`: User interface for robot control
* `script.js`: Sends HTTP requests and updates UI

### ESP8266 (Web Server)

* Hosts static files using SPIFFS
* Exposes HTTP endpoints:

  * `/forwards5`, `/forwards20`
  * `/backwards5`, `/backwards20`
  * `/compass?value=<angle>`
  * `/findNorth`
* Communicates with the robot controller via Serial

### Robot Controller (Arduino)

* Controls motors via PWM and direction pins
* Uses encoder interrupts for distance measurement
* Reads compass data over I2C
* Displays status on LCD

---

## Hardware Requirements

* ESP8266 module (e.g., NodeMCU)
* Arduino (or compatible microcontroller)
* Motor driver (e.g., L298N)
* DC motors with encoders
* I2C compass module (address `0x60`)
* 20x4 LCD display
* Power supply

---

## Setup

### 1. Upload Files to ESP8266

Upload the following to SPIFFS:

* `index.html`
* `style.css`
* `script.js`
* `favicon`

### 2. Configure Wi-Fi

Update credentials in the ESP8266 code:

```cpp
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
```

### 3. Upload Firmware

* Flash ESP8266 with server code
* Flash Arduino with robot control code

### 4. Connect Hardware

* Establish Serial communication between ESP8266 and Arduino
* Connect motors, encoders, LCD, and compass module

### 5. Run

* Open Serial Monitor to obtain ESP8266 IP address
* Access the interface in a browser:

```
http://<ESP_IP>
```

---

## Command Flow

1. User interacts with the web interface
2. JavaScript sends an HTTP request
3. ESP8266 processes the request
4. Command is sent via Serial
5. Arduino executes the action

---

## Notes

* Compass calibration may be required for accurate heading
* Movement precision depends on encoder calibration (`pulsesPerCm`)
* Additional features such as obstacle detection or PID control can be added

---

## License

This project is provided as-is for educational and development purposes.
