/*
 * ADMT4000 Basic Readings Example
 *
 * This example demonstrates how to use the ADMT4000 library to read:
 * - Current angle (0-360°)
 * - Absolute angle (with turn count)
 * - Turn count
 * - Temperature
 *
 * Wiring for ESP32:
 * ESP32 GPIO18 -> SCLK (Pin 17)
 * ESP32 GPIO23 -> SDI  (Pin 16)
 * ESP32 GPIO19 -> SDO  (Pin 15)
 * ESP32 GPIO5  -> CS   (Pin 18)
 * 3.3V         -> VDD  (Pin 5)
 * 3.3V         -> VDRIVE (Pin 21)
 * GND          -> GND  (Pins 6, 7)
 */

#include <ADMT4000.h>

// Create sensor object with default pins (CS=5, SCLK=18, SDO=19, SDI=23)
ADMT4000 sensor;

// Or customize the pins:
// ADMT4000 sensor(CS_PIN, SCLK_PIN, SDO_PIN, SDI_PIN);

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("\n\nADMT4000 Multiturn Sensor Test");
  Serial.println("================================");

  if (sensor.begin()) {
    Serial.println("Sensor initialized successfully!");
  } else {
    Serial.println("Sensor initialization failed!");
    while (1)
      delay(1000);
  }

  delay(100);
  Serial.println("\nStarting measurements...\n");
}

void loop() {
  // Read angle data
  float angle = sensor.readAngle();
  float absAngle = sensor.readAbsoluteAngle();

  int8_t turns;
  float angleInTurn;
  sensor.readTurnCountAndAngle(turns, angleInTurn);

  // Read temperature
  float temperature = sensor.readTemperature();

  // Send data in simple format for Processing
  Serial.print("ANGLE:");
  Serial.print(angle, 2);
  Serial.print("|ABSANGLE:");
  Serial.print(absAngle, 2);
  Serial.print("|TURNS:");
  Serial.print(turns);
  Serial.print("|ANGLEINTURN:");
  Serial.print(angleInTurn, 2);
  Serial.print("|TEMP:");
  Serial.println(temperature, 1);

  delay(100);
}
