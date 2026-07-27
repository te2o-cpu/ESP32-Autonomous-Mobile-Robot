#include "BluetoothSerial.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

BluetoothSerial SerialBT;
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

// State management
bool isStopped = true;

// Pins for Ultrasonic & Servo
const int trigPin = 5;
const int echoPin = 18;
const int servoPin = 23;

// Pins for L298N Motor Driver
const int ENA = 32;
const int ENB = 33;
const int IN1 = 25;
const int IN2 = 26;
const int IN3 = 27;
const int IN4 = 14;

// Encoder setup
#define ENCODER_PIN 13
volatile long pulseCount = 0;
volatile unsigned long lastPulseTime = 0;
const float DIST_PER_PULSE = 5.1025; // Distance in mm per pulse

// PWM Constants
const int chA = 6;
const int chB = 7;
const int freq = 5000;
const int res = 8;

// Speed variables (0-255)
int speedForward = 255;
int turnFast = 255;
int turnSlow = 0;

// Interrupt function to count pulses
void IRAM_ATTR countPulse() {
  unsigned long now = micros();
  if (now - lastPulseTime > 2000) {
    pulseCount++;
    lastPulseTime = now;
  }
}

int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 25000);
  int d = duration * 0.034 / 2;
  if (d <= 0 || d > 400) return 400;
  return d;
}

void moveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(chA, speedForward);
  ledcWrite(chB, speedForward);
}

void moveBackward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  ledcWrite(chA, speedForward);
  ledcWrite(chB, speedForward);
}

void turnRightTimed() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  ledcWrite(chA, turnFast);
  ledcWrite(chB, turnFast);
  delay(400);
}

void turnLeftTimed() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  ledcWrite(chA, turnFast);
  ledcWrite(chB, turnFast);
  delay(400);
}

void stopRobot() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  ledcWrite(chA, 0); ledcWrite(chB, 0);
}

void updateDisplay(int distance) {
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print(" cm   ");
  
  lcd.setCursor(0, 1);
  lcd.print("Pulses: ");
  lcd.print(pulseCount);
  lcd.print("   ");
}

void scanAndAvoid() {
  stopRobot();
  
  myServo.write(5); // Look Right
  delay(600);
  int rightD = getDistance();
  
  myServo.write(175); // Look Left
  delay(800);
  int leftD = getDistance();
  
  myServo.write(90); // Back to Center
  delay(400);
  
  if (rightD > leftD && rightD > 20) {
    turnRightTimed();
  } else if (leftD > rightD && leftD > 20) {
    turnLeftTimed();
  } else {
    moveBackward();
    delay(500);
    turnRightTimed();
  }
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Robot_Control");
  
  lcd.init();
  lcd.backlight();
  
  myServo.attach(servoPin);
  myServo.write(90);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  pinMode(ENCODER_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), countPulse, RISING);
  
  ledcSetup(chA, freq, res);
  ledcAttachPin(ENA, chA);
  
  ledcSetup(chB, freq, res);
  ledcAttachPin(ENB, chB);
  
  stopRobot();
}

void loop() {
  int distance = getDistance();
  
  // 1. Bluetooth Control
  if (SerialBT.available()) {
    char data = SerialBT.read();
    if (data == 'F') { moveForward(); isStopped = false; }
    else if (data == 'B') { moveBackward(); isStopped = false; }
    else if (data == 'L') { turnLeftTimed(); isStopped = false; }
    else if (data == 'R') { turnRightTimed(); isStopped = false; }
    else if (data == 'S') { stopRobot(); isStopped = true; }
  }
  
  // 2. Obstacle Detection (Active only if not stopped)
  if (!isStopped && distance < 25) {
    scanAndAvoid();
  }
  
  // 3. Update LCD with Distance and Encoder data
  updateDisplay(distance);
}
