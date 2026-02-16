/*
 * ADMT4000 Arduino Library
 * 
 * A library for interfacing with the ADMT4000 Multiturn Magnetic Sensor
 * Compatible with ESP32 and other Arduino-compatible boards with SPI support
 * 
 * Author: Past Times Project
 * License: MIT
 */

#ifndef ADMT4000_H
#define ADMT4000_H

#include <Arduino.h>
#include <SPI.h>

// Register addresses (Page Agnostic)
#define REG_CNVPAGE        0x01
#define REG_ABSANGLE       0x03
#define REG_DIGIO          0x04
#define REG_ANGLE          0x05
#define REG_FAULT          0x06

// Page 0x00 Registers
#define REG_SINE           0x10
#define REG_COSINE         0x11
#define REG_RADIUS         0x18
#define REG_TMP            0x20

// Page 0x02 Registers
#define REG_GENERAL        0x10
#define REG_DIGIOEN        0x12
#define REG_CNVCNT         0x14

// Commands
#define CMD_READ           0x00
#define CMD_WRITE          0x01
#define CMD_CNV_START      0x00
#define CMD_CNV_ABORT      0x03

class ADMT4000 {
private:
  uint8_t csPin;
  SPISettings spiSettings;
  
  // CRC-5 calculation (polynomial: x^5 + x^2 + 1, seed: 0x1F)
  uint8_t calculateCRC5(uint32_t data, uint8_t numBits);
  
  // Read 16-bit register
  uint16_t readRegister(uint8_t address);
  
  // Write 16-bit register
  void writeRegister(uint8_t address, uint16_t data);
  
  // Set register page
  void setPage(uint8_t page);

public:
  /**
   * @brief Constructor for ADMT4000 sensor
   * @param cs Chip select pin (default: GPIO5)
   * @param sclk SPI clock pin (default: GPIO18)
   * @param sdo SPI MISO pin (default: GPIO19)
   * @param sdi SPI MOSI pin (default: GPIO23)
   */
  ADMT4000(uint8_t cs = 5, uint8_t sclk = 18, uint8_t sdo = 19, uint8_t sdi = 23);
  
  /**
   * @brief Initialize the sensor
   * @return true if initialization successful, false otherwise
   */
  bool begin();
  
  /**
   * @brief Start continuous conversion mode
   */
  void startConversion();
  
  /**
   * @brief Abort continuous conversion mode
   */
  void abortConversion();
  
  /**
   * @brief Read current angle (0-360°)
   * @return Angle in degrees (12-bit resolution, 0.0 to 360.0)
   */
  float readAngle();
  
  /**
   * @brief Read absolute angle including turn count
   * @return Absolute angle in degrees (can be negative or > 360°)
   */
  float readAbsoluteAngle();
  
  /**
   * @brief Read turn count and angle separately
   * @param turns Reference to store the turn count
   * @param angle Reference to store the current angle
   */
  void readTurnCountAndAngle(int8_t &turns, float &angle);
  
  /**
   * @brief Read sensor temperature
   * @return Temperature in degrees Celsius
   */
  float readTemperature();

private:
  uint8_t sclkPin;
  uint8_t sdoPin;
  uint8_t sdiPin;
};

#endif // ADMT4000_H
