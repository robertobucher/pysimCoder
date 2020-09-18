/*!
 * @file Adafruit_VL6180X.cpp
 *
 * @mainpage Adafruit VL6180X ToF sensor driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's VL6180X driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit VL6180X breakout: http://www.adafruit.com/products/3316
 *
 * These sensors use I2C to communicate, 2 pins (SCL+SDA) are required
 * to interface with the breakout.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section author Author
 *
 * Written by ladyada for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#include "Arduino.h"
#include <Wire.h>
#include "Adafruit_VL6180X.h"


/**************************************************************************/
/*! 
    @brief  Instantiates a new VL6180X class
*/
/**************************************************************************/
Adafruit_VL6180X::Adafruit_VL6180X(void) {
}

/**************************************************************************/
/*! 
    @brief  Initializes I2C interface, checks that VL6180X is found and resets chip.
    @param  theWire Optional pointer to I2C interface, &Wire is used by default
    @returns True if chip found and initialized, False otherwise
*/
/**************************************************************************/
boolean Adafruit_VL6180X::begin(TwoWire *theWire) {
  _i2caddr = VL6180X_DEFAULT_I2C_ADDR;
  if (! theWire) {
    _i2c = &Wire;
  } else {
    _i2c = theWire;
  }
  _i2c-> begin();


  if (read8(VL6180X_REG_IDENTIFICATION_MODEL_ID) != 0xB4) {
    return false;
  }

  //if (read8(VL6180X_REG_SYSTEM_FRESH_OUT_OF_RESET) == 0x01) {
    loadSettings();
  //}

  write8(VL6180X_REG_SYSTEM_FRESH_OUT_OF_RESET, 0x00);

  return true;
}

/**************************************************************************/
/*! 
    @brief  Load the settings for proximity/distance ranging
*/
/**************************************************************************/

void Adafruit_VL6180X::loadSettings(void) {
    // load settings!

    // private settings from page 24 of app note
    write8(0x0207, 0x01);
    write8(0x0208, 0x01);
    write8(0x0096, 0x00);
    write8(0x0097, 0xfd);
    write8(0x00e3, 0x00);
    write8(0x00e4, 0x04);
    write8(0x00e5, 0x02);
    write8(0x00e6, 0x01);
    write8(0x00e7, 0x03);
    write8(0x00f5, 0x02);
    write8(0x00d9, 0x05);
    write8(0x00db, 0xce);
    write8(0x00dc, 0x03);
    write8(0x00dd, 0xf8);
    write8(0x009f, 0x00);
    write8(0x00a3, 0x3c);
    write8(0x00b7, 0x00);
    write8(0x00bb, 0x3c);
    write8(0x00b2, 0x09);
    write8(0x00ca, 0x09);
    write8(0x0198, 0x01);
    write8(0x01b0, 0x17);
    write8(0x01ad, 0x00);
    write8(0x00ff, 0x05);
    write8(0x0100, 0x05);
    write8(0x0199, 0x05);
    write8(0x01a6, 0x1b);
    write8(0x01ac, 0x3e);
    write8(0x01a7, 0x1f);
    write8(0x0030, 0x00);

    // Recommended : Public registers - See data sheet for more detail
    write8(0x0011, 0x10);       // Enables polling for 'New Sample ready'
                                // when measurement completes
    write8(0x010a, 0x30);       // Set the averaging sample period
                                // (compromise between lower noise and
                                // increased execution time)
    write8(0x003f, 0x46);       // Sets the light and dark gain (upper
                                // nibble). Dark gain should not be
                                // changed.
    write8(0x0031, 0xFF);       // sets the # of range measurements after
                                // which auto calibration of system is
                                // performed
    write8(0x0040, 0x63);       // Set ALS integration time to 100ms
    write8(0x002e, 0x01);       // perform a single temperature calibration
                                // of the ranging sensor

    // Optional: Public registers - See data sheet for more detail
    write8(0x001b, 0x09);       // Set default ranging inter-measurement
                                // period to 100ms
    write8(0x003e, 0x31);       // Set default ALS inter-measurement period
                                // to 500ms
    write8(0x0014, 0x24);       // Configures interrupt on 'New Sample
                                // Ready threshold event'
}


/**************************************************************************/
/*! 
    @brief  Single shot ranging. Be sure to check the return of {@link readRangeStatus} to before using the return value!
    @return Distance in millimeters if valid
*/
/**************************************************************************/

uint8_t Adafruit_VL6180X::readRange(void) {
  // wait for device to be ready for range measurement
  while (! (read8(VL6180X_REG_RESULT_RANGE_STATUS) & 0x01));

  // Start a range measurement
  write8(VL6180X_REG_SYSRANGE_START, 0x01);

  // Poll until bit 2 is set
  while (! (read8(VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO) & 0x04));

  // read range in mm
  uint8_t range = read8(VL6180X_REG_RESULT_RANGE_VAL);

  // clear interrupt
  write8(VL6180X_REG_SYSTEM_INTERRUPT_CLEAR, 0x07);

  return range;
}


/**************************************************************************/
/*! 
    @brief  Request ranging success/error message (retreive after ranging)
    @returns One of possible VL6180X_ERROR_* values
*/
/**************************************************************************/

uint8_t Adafruit_VL6180X::readRangeStatus(void) {
  return (read8(VL6180X_REG_RESULT_RANGE_STATUS) >> 4);
}


/**************************************************************************/
/*! 
    @brief  Single shot lux measurement
    @param  gain Gain setting, one of VL6180X_ALS_GAIN_*
    @returns Lux reading
*/
/**************************************************************************/

float Adafruit_VL6180X::readLux(uint8_t gain) {
  uint8_t reg;

  reg = read8(VL6180X_REG_SYSTEM_INTERRUPT_CONFIG);
  reg &= ~0x38;
  reg |= (0x4 << 3); // IRQ on ALS ready
  write8(VL6180X_REG_SYSTEM_INTERRUPT_CONFIG, reg);
  
  // 100 ms integration period
  write8(VL6180X_REG_SYSALS_INTEGRATION_PERIOD_HI, 0);
  write8(VL6180X_REG_SYSALS_INTEGRATION_PERIOD_LO, 100);

  // analog gain
  if (gain > VL6180X_ALS_GAIN_40) {
    gain = VL6180X_ALS_GAIN_40;
  }
  write8(VL6180X_REG_SYSALS_ANALOGUE_GAIN, 0x40 | gain);

  // start ALS
  write8(VL6180X_REG_SYSALS_START, 0x1);

  // Poll until "New Sample Ready threshold event" is set
  while (4 != ((read8(VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO) >> 3) & 0x7));

  // read lux!
  float lux = read16(VL6180X_REG_RESULT_ALS_VAL);

  // clear interrupt
  write8(VL6180X_REG_SYSTEM_INTERRUPT_CLEAR, 0x07);

  lux *= 0.32; // calibrated count/lux
  switch(gain) { 
  case VL6180X_ALS_GAIN_1: 
    break;
  case VL6180X_ALS_GAIN_1_25: 
    lux /= 1.25;
    break;
  case VL6180X_ALS_GAIN_1_67: 
    lux /= 1.67;
    break;
  case VL6180X_ALS_GAIN_2_5: 
    lux /= 2.5;
    break;
  case VL6180X_ALS_GAIN_5: 
    lux /= 5;
    break;
  case VL6180X_ALS_GAIN_10: 
    lux /= 10;
    break;
  case VL6180X_ALS_GAIN_20: 
    lux /= 20;
    break;
  case VL6180X_ALS_GAIN_40: 
    lux /= 40;
    break;
  }
  lux *= 100;
  lux /= 100; // integration time in ms


  return lux;
}

/**************************************************************************/
/*! 
    @brief  I2C low level interfacing
*/
/**************************************************************************/


// Read 1 byte from the VL6180X at 'address'
uint8_t Adafruit_VL6180X::read8(uint16_t address)
{
  uint8_t data;

  _i2c->beginTransmission(_i2caddr);
  _i2c->write(address>>8);
  _i2c->write(address);
  _i2c->endTransmission();

  _i2c->requestFrom(_i2caddr, (uint8_t)1);
  data = _i2c->read();

#if defined(I2C_DEBUG)
  Serial.print("\t$"); Serial.print(address, HEX); Serial.print(": 0x"); Serial.println(data, HEX);
#endif

  return data;
}


// Read 2 byte from the VL6180X at 'address'
uint16_t Adafruit_VL6180X::read16(uint16_t address)
{
  uint16_t data;

  _i2c->beginTransmission(_i2caddr);
  _i2c->write(address>>8);
  _i2c->write(address);
  _i2c->endTransmission();

  _i2c->requestFrom(_i2caddr, (uint8_t)2);
  data = _i2c->read();
  data <<= 8;
  data |= _i2c->read();
  
  return data;
}

// write 1 byte
void Adafruit_VL6180X::write8(uint16_t address, uint8_t data)
{
  _i2c->beginTransmission(_i2caddr);
  _i2c->write(address>>8);
  _i2c->write(address);
  _i2c->write(data);  
  _i2c->endTransmission();

#if defined(I2C_DEBUG)
  Serial.print("\t$"); Serial.print(address, HEX); Serial.print(" = 0x"); Serial.println(data, HEX);
#endif
}


// write 2 bytes
void Adafruit_VL6180X::write16(uint16_t address, uint16_t data)
{
  _i2c->beginTransmission(_i2caddr);
  _i2c->write(address>>8);
  _i2c->write(address);
  _i2c->write(data>>8);
  _i2c->write(data);
  _i2c->endTransmission();
}
