/*
 * ADMT4000 Arduino Library
 *
 * Implementation file for the ADMT4000 Multiturn Magnetic Sensor
 */

#include "ADMT4000.h"

ADMT4000::ADMT4000(uint8_t cs, uint8_t sclk, uint8_t sdo, uint8_t sdi)
    : csPin(cs), sclkPin(sclk), sdoPin(sdo), sdiPin(sdi),
      spiSettings(10000000, MSBFIRST, SPI_MODE0) {}

uint8_t ADMT4000::calculateCRC5(uint32_t data, uint8_t numBits) {
  uint8_t crc = 0x1F;

  for (int i = numBits - 1; i >= 0; i--) {
    uint8_t bit = (data >> i) & 0x01;
    uint8_t xor_bit = bit ^ ((crc >> 4) & 0x01);

    crc = ((crc << 1) & 0x1F) | xor_bit;
    if (xor_bit) {
      crc ^= 0x05;
    }
  }

  return crc & 0x1F;
}

uint16_t ADMT4000::readRegister(uint8_t address) {
  uint32_t txData = 0;

  txData = (CMD_READ << 6) | (address & 0x3F);

  SPI.beginTransaction(spiSettings);
  digitalWrite(csPin, LOW);
  delayMicroseconds(1);

  SPI.transfer((txData >> 0) & 0xFF);

  uint16_t dataHigh = SPI.transfer(0x00);
  uint16_t dataLow = SPI.transfer(0x00);
  uint16_t registerData = (dataHigh << 8) | dataLow;

  uint8_t faultBit = SPI.transfer(0x00);
  uint8_t convCounter = SPI.transfer(0x00);
  uint8_t crc = SPI.transfer(0x00);

  delayMicroseconds(1);
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();

  return registerData;
}

void ADMT4000::writeRegister(uint8_t address, uint16_t data) {
  uint32_t txData = 0;

  txData = (CMD_WRITE << 22) | ((address & 0x3F) << 16) | data;

  uint32_t crcInput =
      ((CMD_WRITE << 22) | ((address & 0x3F) << 16) | data) >> 5;
  uint8_t crc = calculateCRC5(crcInput, 26);

  SPI.beginTransaction(spiSettings);
  digitalWrite(csPin, LOW);
  delayMicroseconds(1);

  SPI.transfer((txData >> 24) & 0xFF);
  SPI.transfer((txData >> 16) & 0xFF);
  SPI.transfer((txData >> 8) & 0xFF);
  SPI.transfer((txData >> 0) & 0xFF);
  SPI.transfer((crc & 0x1F));

  delayMicroseconds(1);
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
}

void ADMT4000::setPage(uint8_t page) {
  writeRegister(REG_CNVPAGE, page & 0x1F);
}

bool ADMT4000::begin() {
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);

  SPI.begin(sclkPin, sdoPin, sdiPin, csPin);

  delay(15);
  writeRegister(REG_FAULT, 0x0000);
  startConversion();
  delay(10);

  return true;
}

void ADMT4000::startConversion() {
  uint16_t cmd = (CMD_CNV_START << 14);
  writeRegister(REG_CNVPAGE, cmd);
}

void ADMT4000::abortConversion() {
  uint16_t cmd = (CMD_CNV_ABORT << 14);
  writeRegister(REG_CNVPAGE, cmd);
}

float ADMT4000::readAngle() {
  uint16_t angleReg = readRegister(REG_ANGLE);
  uint16_t angleCode = (angleReg >> 4) & 0x0FFF;
  float angle = (angleCode * 360.0) / 4096.0;
  return angle;
}

float ADMT4000::readAbsoluteAngle() {
  float currentAngle = readAngle();
  uint16_t absAngleReg = readRegister(REG_ABSANGLE);

  // Bits [15:10] contain the FULL TURN count (NOT quarter turns!)
  // Despite the datasheet saying "quarter turns", empirically we've found
  // that this increments by 1 for each 360° rotation
  uint8_t rawTurnCount = (absAngleReg >> 10) & 0x3F;

  if (rawTurnCount == 0x36) {
    return -1.0; // Invalid
  }

  int fullTurns;
  if (rawTurnCount >= 0x37) {
    // Negative value (two's complement)
    fullTurns = (int8_t)(rawTurnCount - 64);
  } else {
    // Positive value - this is already in FULL turns, not quarter turns
    fullTurns = rawTurnCount;
  }

  // Combine: full turns * 360° + current angle
  float absoluteAngle = (fullTurns * 360.0) + currentAngle;

  return absoluteAngle;
}

void ADMT4000::readTurnCountAndAngle(int8_t &turns, float &angle) {
  uint16_t absAngleReg = readRegister(REG_ABSANGLE);

  // Extract turn count (empirically found to be FULL turns, not quarter turns)
  uint8_t rawTurnCount = (absAngleReg >> 10) & 0x3F;

  if (rawTurnCount == 0x36) {
    turns = -999;
    angle = 0;
    return;
  } else if (rawTurnCount >= 0x37) {
    turns = (int8_t)(rawTurnCount - 64);
  } else {
    turns = rawTurnCount; // NO division by 4!
  }

  angle = readAngle();
}

float ADMT4000::readTemperature() {
  setPage(0x00);
  uint16_t tempReg = readRegister(REG_TMP);
  uint16_t tempCode = (tempReg >> 4) & 0x0FFF;

  float temperature = (tempCode - 1168.0) / 15.66;

  return temperature;
}
