# Gesture Control Robot Car
A project for microcontroller-based robotics using Arduino and Bluetooth communication

# Project Overview
Gesture Control Robot Car is an Arduino-based robotic system that allows users to control a robot using hand gestures. The system utilizes an MPU6050 accelerometer to detect hand movements, which are then transmitted via Bluetooth HC-05 modules to a robot car that responds in real time.

![WhatsApp Image 2025-01-14 at 18 44 14_37643cb0](https://github.com/user-attachments/assets/dace6081-a6b5-49ba-b015-33c4f95ae872)

# Components
      Transmitter Unit:
Arduino Uno – Processes sensor data and sends commands.
MPU6050 Sensor – Captures hand movements.
Bluetooth HC-05 (Master) – Sends data to the robot.
Battery Pack (2x 18650 Li-ion) – Powers the transmitter.
Jumper Wires – Connects components.

    Receiver Unit (Robot Car):
Arduino Uno – Receives and processes commands.
Bluetooth HC-05 (Slave) – Receives data from the transmitter.
Chassis with 4 DC Motors – Moves the robot.
L298N Motor Driver – Controls motor speed and direction.
Battery Pack (4x 18650 Li-ion) – Powers the robot.
Jumper Wires – Connects components.

  ![WhatsApp Image 2025-01-14 at 18 44 15_35fec1ba](https://github.com/user-attachments/assets/4683330a-8e1e-4759-90be-076c3633f2a7)

# ⚙️ How It Works
Hand Gestures Detection 🖐️

The MPU6050 sensor detects hand movement and sends data to the Arduino Uno.
Wireless Communication 📡

The Arduino Uno transmits movement data via Bluetooth (HC-05 Master).
Robot Car Movement 🚗

The receiver unit (robot) gets data through Bluetooth (HC-05 Slave).
Arduino Uno interprets signals and drives motors via the L298N module.

# 📝 Code Implementation
Transmitter Code: Reads MPU6050 sensor data, processes it, and sends movement commands via Bluetooth.
Receiver Code: Receives Bluetooth signals and controls the L298N motor driver to move the robot accordingly.
