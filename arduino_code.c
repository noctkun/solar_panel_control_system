#include <Servo.h>               // For controlling the servo motor
#include <Wire.h>                // For I2C communication
#include <LiquidCrystal_I2C.h>   // For I2C LCD

// Constants and pins
const int tempSensorPin = A0;    // Temperature sensor pin
const int coolingSystemPin = 9;  // Cooling system pin (DC Motor)
const int redLEDPin = 10;        // Red LED pin
const int servoPin = 3;          // Servo motor control pin

Servo solarPanelServo;           // Servo object
LiquidCrystal_I2C lcd(32, 16, 2); // I2C address (check your module)

const float tempThreshold = 120.0; // Overheating threshold in Celsius
const int updateInterval = 5400 * 1000; // 1.5 hours in milliseconds
int currentAngle = 0;            // Current angle of the panel
const int angleStep = 15;        // Sun moves 15 degrees every 1.5 hours

unsigned long lastUpdate = 0;    // Time since last angle update

void setup() {
  // Initialize components
  solarPanelServo.attach(servoPin);
  pinMode(coolingSystemPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);

  lcd.init();                     // Initialize the LCD
  lcd.backlight();                // Turn on the backlight

  // Start at initial angle
  solarPanelServo.write(currentAngle);

  // Display initial message
  lcd.setCursor(0, 0);
  lcd.print("Student RegNo:");
  lcd.setCursor(0, 1);
  lcd.print("22BCE1180"); // Ganesh Kumar 22BCE1180

  delay(1000);
  lcd.clear();
}

void loop() {
  // Check if it's time to update the panel angle
  unsigned long currentTime = millis();
  if (currentTime - lastUpdate >= updateInterval) {
    lastUpdate = currentTime;
    currentAngle += angleStep;
    if (currentAngle > 180) currentAngle = 0; // Reset angle if beyond range
    solarPanelServo.write(currentAngle);
  }

  // Read temperature
  int tempReading = analogRead(tempSensorPin);
  float voltage = tempReading * (5.0 / 1023.0); // Convert to voltage
  float temperature = (voltage - 0.5) * 100.0;  // Convert to Celsius

  // Check for overheating
  if (temperature > tempThreshold) {
    digitalWrite(coolingSystemPin, HIGH); // Turn on cooling system
    digitalWrite(redLEDPin, HIGH);       // Turn on red LED

    // Display warning message
    lcd.setCursor(0, 0);
    lcd.print("Overheating!");
    lcd.setCursor(0, 1);
    lcd.print("Cooling Active");
  } else {
    digitalWrite(coolingSystemPin, LOW); // Turn off cooling system
    digitalWrite(redLEDPin, LOW);       // Turn off red LED
  }

  // Toggle Student ID display every second without clearing the LCD
  static unsigned long lastToggleTime = 0;
  static bool isIDDisplayed = false;
  if (currentTime - lastToggleTime >= 1000) { // Every 1000 ms (1 second)
    lastToggleTime = currentTime;
    if (isIDDisplayed) {
      // Just hide the student ID by printing spaces on the same positions
      lcd.setCursor(0, 0);
      lcd.print("                ");  // Clear the first line
      lcd.setCursor(0, 1);
      lcd.print("                ");  // Clear the second line
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Student RegNo:");
      lcd.setCursor(0, 1);
      lcd.print("22BCE1180"); // Ganesh Kumar 22BCE1180
    }
    isIDDisplayed = !isIDDisplayed; // Toggle display
  }
}

