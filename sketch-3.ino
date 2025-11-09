#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD with I2C address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define IR sensor pins
const int entrySensor = 9;
const int exitSensor = 10;
const int doorSensor = 8;  // New sensor to detect students near the door
const int doorPin1 = 2;
const int doorPin2 = 3;

// Define light and door motor control pins
const int lightPin = 6;  // Example pin for controlling lights
const int doorMotorPin = 7;  // Example pin for controlling door motor

// Variables to store the count of students
int studentsInside = 0;
int studentsOutside = 0;

// State variables to detect entry, exit, and door events
bool entryTriggered = false;
bool exitTriggered = false;
bool doorTriggered = false;

void setup() {
  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Set up the IR sensor pins as inputs
  pinMode(entrySensor, INPUT);
  pinMode(exitSensor, INPUT);
  pinMode(doorSensor, INPUT);

  // Set up the light and door motor control pins as outputs
  pinMode(lightPin, OUTPUT);
  pinMode(doorMotorPin, OUTPUT);
  pinMode(doorPin1, OUTPUT);
  pinMode(doorPin2, OUTPUT);

  // Display initial message
  lcd.setCursor(0, 0);
  lcd.print("In: 0  Out: 0");
  lcd.setCursor(0, 1);
  lcd.print("Inside: 0");

  // Initially turn off the lights and close the door
  digitalWrite(lightPin, HIGH);
  digitalWrite(doorMotorPin, HIGH);  // Assume LOW closes the door
  digitalWrite(doorPin1, LOW);
  digitalWrite(doorPin2, HIGH);
  delay(1030);
  digitalWrite(doorPin1, LOW);
  digitalWrite(doorPin2, LOW);
}

void loop() {
  // Read the IR sensor values
  int entryState = digitalRead(entrySensor);
  int exitState = digitalRead(exitSensor);
  int doorState = digitalRead(doorSensor);

  // Entry Logic: Detect a student entering
  if (entryState == HIGH && !entryTriggered) {
    entryTriggered = true;  // Student is in the entry path
  } else if (entryState == LOW && entryTriggered) {
    studentsInside++;
    updateDisplay();
    entryTriggered = false;  // Reset the flag after the student has entered
  }

  // Exit Logic: Detect a student exiting
  if (exitState == HIGH && !exitTriggered) {
    exitTriggered = true;  // Student is in the exit path
  } else if (exitState == LOW && exitTriggered) {
    if (studentsInside > 0) {
      studentsInside--;
      studentsOutside++;
      updateDisplay();
    }
    exitTriggered = false;  // Reset the flag after the student has exited
  }

  // Door Control Logic: Open the door when a student is detected near the door
  if (doorState == HIGH && !doorTriggered) {
    doorTriggered = true;
    digitalWrite(doorMotorPin, LOW);
    digitalWrite(doorPin1, LOW);
    digitalWrite(doorPin2, HIGH);
    delay(3000);
    digitalWrite(doorPin1, HIGH);
    digitalWrite(doorPin2, LOW);
    delay(1300);
    digitalWrite(doorPin1, LOW);
    digitalWrite(doorPin2, LOW);  // Open the door
  } else if (doorState == LOW && doorTriggered) {
    doorTriggered = false;
    digitalWrite(doorMotorPin, HIGH);  // Close the door
  }
}

void updateDisplay() {
  // Update the LCD display with the current counts
  lcd.setCursor(0, 0);
  lcd.print("In: ");
  lcd.print(studentsInside);
  lcd.print("  Out: ");
  lcd.print(studentsOutside);
  lcd.print("   ");  // Clear any remaining characters in the "Out" field

  lcd.setCursor(0, 1);
  lcd.print("Inside: ");
  lcd.print(studentsInside);
  lcd.print("    ");  // Clear any remaining characters in the "Inside" field

  // Control the lights based on the number of students inside
  if (studentsInside == 0) {
    digitalWrite(lightPin, HIGH);  // Turn off lights
  } else {
    digitalWrite(lightPin, LOW);  // Turn on lights
  }
}
