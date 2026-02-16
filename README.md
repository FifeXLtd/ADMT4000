# ADMT4000 Arduino Library

An Arduino library for interfacing with the **ADMT4000 Multiturn Magnetic Sensor** via SPI. This library provides easy-to-use functions for reading angle, absolute angle with turn count, and temperature from the sensor.

## Features

- ✅ **12-bit angle resolution** (0-360°)
- ✅ **Multiturn tracking** (up to ±27 full rotations)
- ✅ **Temperature sensing**
- ✅ **SPI communication** with CRC error checking
- ✅ **ESP32 support** (and other Arduino-compatible boards)
- ✅ **Simple API** with clear documentation

## Installation

### Arduino Library Manager
1. Open the Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries...**
3. Search for "ADMT4000"
4. Click "Install"

### Manual Installation
1. Download this repository as a ZIP file
2. Open Arduino IDE
3. Go to **Sketch → Include Library → Add .ZIP Library...**
4. Select the downloaded ZIP file

### Git Clone
```bash
cd ~/Documents/Arduino/libraries
git clone https://github.com/yourusername/ADMT4000.git
```

## Hardware Wiring

### ESP32 Default Pins

| ADMT4000 Pin | Function | ESP32 Pin |
|--------------|----------|-----------|
| Pin 17       | SCLK     | GPIO18    |
| Pin 16       | SDI      | GPIO23    |
| Pin 15       | SDO      | GPIO19    |
| Pin 18       | CS       | GPIO5     |
| Pin 5        | VDD      | 3.3V      |
| Pin 21       | VDRIVE   | 3.3V      |
| Pins 6, 7    | GND      | GND       |

> **Note:** The ADMT4000 operates at **3.3V**. Do not connect to 5V!

## Quick Start

```cpp
#include <ADMT4000.h>

// Create sensor object
ADMT4000 sensor;

void setup() {
  Serial.begin(115200);
  
  // Initialize the sensor
  if (sensor.begin()) {
    Serial.println("Sensor ready!");
  }
}

void loop() {
  // Read current angle (0-360°)
  float angle = sensor.readAngle();
  
  // Read absolute angle (includes turn count)
  float absAngle = sensor.readAbsoluteAngle();
  
  // Read temperature
  float temp = sensor.readTemperature();
  
  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print("° | Absolute: ");
  Serial.print(absAngle);
  Serial.print("° | Temp: ");
  Serial.print(temp);
  Serial.println("°C");
  
  delay(100);
}
```

## API Reference

### Constructor

```cpp
ADMT4000(uint8_t cs = 5, uint8_t sclk = 18, uint8_t sdo = 19, uint8_t sdi = 23)
```
Creates an ADMT4000 sensor object with specified SPI pins.

**Parameters:**
- `cs` - Chip Select pin (default: GPIO5)
- `sclk` - SPI Clock pin (default: GPIO18)
- `sdo` - SPI MISO pin (default: GPIO19)
- `sdi` - SPI MOSI pin (default: GPIO23)

### Methods

#### `bool begin()`
Initializes the sensor and SPI communication.

**Returns:** `true` if successful, `false` otherwise

---

#### `float readAngle()`
Reads the current angle within a single rotation.

**Returns:** Angle in degrees (0.0 to 360.0)

---

#### `float readAbsoluteAngle()`
Reads the absolute angle including multiturn tracking.

**Returns:** Absolute angle in degrees (can be negative or > 360°)

**Example:**
- 0 turns, 45° → 45.0°
- 1 turn, 90° → 450.0°
- -2 turns, 180° → -540.0°

---

#### `void readTurnCountAndAngle(int8_t &turns, float &angle)`
Reads the turn count and current angle separately.

**Parameters:**
- `turns` - Reference to store turn count (-27 to +27)
- `angle` - Reference to store current angle (0-360°)

---

#### `float readTemperature()`
Reads the sensor's internal temperature.

**Returns:** Temperature in degrees Celsius

---

#### `void startConversion()`
Starts continuous conversion mode (called automatically by `begin()`).

---

#### `void abortConversion()`
Stops continuous conversion mode.

## Important Notes

### Turn Count Behavior
The ADMT4000 datasheet mentions "quarter turns" in the ABSANGLE register, but **empirically we've found** that the turn count increments by 1 for each **full 360° rotation**, not quarter turns. This library accounts for this behavior.

### CRC Validation
The library implements CRC-5 validation for write operations. Read operations receive CRC data but validation can be added if needed.

### SPI Settings
- **Clock Speed:** 10 MHz
- **Bit Order:** MSB First
- **Mode:** SPI_MODE0

## Examples

See the `examples/` folder for complete examples:
- **BasicReadings** - Demonstrates reading angle, absolute angle, and temperature

## Compatibility

- ✅ ESP32
- ✅ ESP8266 (with modified pins)
- ✅ Arduino boards with SPI support

## Troubleshooting

### Sensor not responding
- Check wiring connections
- Verify 3.3V power supply
- Ensure CS pin is correctly defined

### Incorrect angle readings
- Check magnet alignment with sensor
- Verify magnet strength and distance
- Ensure sensor is not in fault state

### Turn count issues
- Power cycle the sensor to reset turn count
- Check for mechanical issues causing unexpected rotations

## License

MIT License - See LICENSE file for details

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Credits

Developed by the **Past Times Project** team.

Based on the ADMT4000 datasheet and empirical testing with ESP32 hardware.

## Resources

- [ADMT4000 Datasheet](https://www.analog.com/en/products/admt4000.html)
- [Arduino SPI Reference](https://www.arduino.cc/en/reference/SPI)
