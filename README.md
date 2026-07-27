# ESP32-Autonomous-Mobile-Robot
# ESP32 Dual-Mode Mobile Robot with Autonomous Obstacle Avoidance & Teleoperation 🤖🚗

A modular, 4-wheel mobile robot powered by an **ESP32 DevKit V1** microcontroller. The system features dual operating modes: real-time **Bluetooth teleoperation** and **autonomous obstacle avoidance** driven by ultrasonic servo-scanning and optical speed encoders.

---

## 📸 System Architecture & Block Diagram
The system integrates power distribution, sensing, actuation, and visual telemetry:

- **Brain:** ESP32 DevKit V1 (Wi-Fi/Bluetooth enabled)
- **Motor Control:** L298N Dual H-Bridge Motor Driver
- **Obstacle Sensing:** HC-SR04 Ultrasonic Sensor mounted on an SG90 Servo Motor (180° scanning)
- **Speed & Distance Tracking:** Optical Speed Encoder utilizing Hardware Interrupts (ISR)
- **Telemetry Display:** 16x2 I2C LCD

---

## 🎯 Key Features & Operating Modes

### 1. Manual Control Mode (Bluetooth Teleoperation)
Receives real-time motion commands (`Forward`, `Backward`, `Left`, `Right`, `Stop`) over Bluetooth Classic and executes them immediately.

### 2. Autonomous Obstacle Avoidance Mode (Active Safety)
Monitors distance continuously using the HC-SR04 sensor. When an obstacle is detected within **25 cm**:
1. The robot halts immediately.
2. The SG90 servo motor rotates the ultrasonic sensor to scan both **Right** (5°) and **Left** (175°).
3. The robot compares available distances and automatically steers toward the clearest path.

### 3. Non-Blocking Speed & Distance Tracking
Uses an Optical Encoder connected to an **Interrupt Service Routine (ISR)** to count pulse signals in real time without causing software delays in the main loop execution.

---

## 🛠️ Hardware Component List

| Component | Function / Role |
| :--- | :--- |
| **ESP32 DevKit V1** | Core Microcontroller (Processes Bluetooth & Sensor Logic) |
| **L298N Motor Driver** | Controls direction and PWM speed for 4 DC Motors |
| **HC-SR04 Ultrasonic** | Measures distance to frontal obstacles |
| **SG90 Servo Motor** | Rotates the Ultrasonic sensor for multi-directional scanning |
| **I2C LCD 16x2** | Displays real-time obstacle distance and encoder data |
| **Optical Speed Encoder** | Measures wheel rotations to compute traveled distance |
| **LM7805 / Buck Converter**| Regulates 12V Battery power supply to 5V logic power |

---

## 💻 Software & Libraries Used
- `BluetoothSerial.h` - Handles Classic Bluetooth communication
- `Wire.h` - Manages I2C protocol for the LCD display
- `LiquidCrystal_I2C.h` - Controls the 16x2 I2C LCD screen
- `ESP32Servo.h` - Controls SG90 servo rotation angles

---

## 👥 Project Team & Acknowledgments
- **Submitted By:** Toqa Haitham, Malak Sayed, Radwa Khaled, Anas Hesham, Ahmad Osama[cite: 2]
- **Supervised By:** Prof. Dr. Taha Helmy, Eng. Hussein Rashed, Eng. Kareem Ameed, Eng. Sara Abdel-Naser[cite: 2]
- **Institution:** Modern University for Technology and Information (MTI) - Mechatronics Department[cite: 2]
