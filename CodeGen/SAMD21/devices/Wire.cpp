/*
  TwoWire.cpp - TWI/I2C library for Arduino & Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  Modified December 2014 by Ivan Grokhotkov (ivan@esp8266.com) - esp8266 support
  Modified April 2015 by Hrsto Gochkov (ficeto@ficeto.com) - alternative esp8266 support
  Modified Nov 2017 by Chuck Todd (ctodd@cableone.net) - ESP32 ISR Support
 */

#ifdef BRIKI_MBC_WB_ESP // Wire.cpp for ESP32 mcu 

extern "C" {
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
}

#include "esp32-hal-i2c.h"
#include "Wire.h"
#include "Arduino.h"

TwoWire::TwoWire(uint8_t bus_num)
    :num(bus_num & 1)
    ,sda(-1)
    ,scl(-1)
    ,i2c(NULL)
    ,rxIndex(0)
    ,rxLength(0)
    ,rxQueued(0)
    ,txIndex(0)
    ,txLength(0)
    ,txAddress(0)
    ,txQueued(0)
    ,transmitting(0)
    ,last_error(I2C_ERROR_OK)
    ,_timeOutMillis(50)
{}

TwoWire::~TwoWire()
{
    flush();
    if(i2c) {
        i2cRelease(i2c);
        i2c=NULL;
    }
}

bool TwoWire::begin(int sdaPin, int sclPin, uint32_t frequency)
{
    if(sdaPin < 0) { // default param passed
        if(num == 0) {
            if(sda==-1) {
                sdaPin = SDA;    //use Default Pin
            } else {
				#ifdef BRIKI_MBC_WB_ESP
					if(sda > PINS_COUNT || g_APinDescription[sda].ulPort == PIO_NOT_A_PORT) {		
						return false;		
					}		
				#endif
                sdaPin = sda;    // reuse prior pin
            }
        } else {
            if(sda==-1) {
                log_e("no Default SDA Pin for Second Peripheral");
                return false; //no Default pin for Second Peripheral
            } else {
				#ifdef BRIKI_MBC_WB_ESP		
					if(sda > PINS_COUNT || g_APinDescription[sda].ulPort == PIO_NOT_A_PORT) {		
						return false;
					}		
				#endif
                sdaPin = sda;    // reuse prior pin
            }
        }
    }

    if(sclPin < 0) { // default param passed
        if(num == 0) {
            if(scl == -1) {
                sclPin = SCL;    // use Default pin
            } else {
				#ifdef BRIKI_MBC_WB_ESP		
					if(scl > PINS_COUNT || g_APinDescription[scl].ulPort == PIO_NOT_A_PORT) {		
						return false;		
					}		
				#endif
                sclPin = scl;    // reuse prior pin
            }
        } else {
            if(scl == -1) {
                log_e("no Default SCL Pin for Second Peripheral");
                return false; //no Default pin for Second Peripheral
            } else {
				#ifdef BRIKI_MBC_WB_ESP		
					if(scl > PINS_COUNT || g_APinDescription[scl].ulPort == PIO_NOT_A_PORT) {		
						return false;		
					}		
				#endif
                sclPin = scl;    // reuse prior pin
            }
        }
    }

    sda = sdaPin;
    scl = sclPin;
    i2c = i2cInit(num, sdaPin, sclPin, frequency);
    if(!i2c) {
        return false;
    }

    flush();
    return true;

}

void TwoWire::setTimeOut(uint16_t timeOutMillis)
{
    _timeOutMillis = timeOutMillis;
}

uint16_t TwoWire::getTimeOut()
{
    return _timeOutMillis;
}

void TwoWire::setClock(uint32_t frequency)
{
    i2cSetFrequency(i2c, frequency);
}

size_t TwoWire::getClock()
{
    return i2cGetFrequency(i2c);
}

/* stickBreaker Nov 2017 ISR, and bigblock 64k-1
 */
i2c_err_t TwoWire::writeTransmission(uint16_t address, uint8_t *buff, uint16_t size, bool sendStop)
{
    last_error = i2cWrite(i2c, address, buff, size, sendStop, _timeOutMillis);
    return last_error;
}

i2c_err_t TwoWire::readTransmission(uint16_t address, uint8_t *buff, uint16_t size, bool sendStop, uint32_t *readCount)
{
    last_error = i2cRead(i2c, address, buff, size, sendStop, _timeOutMillis, readCount);
    return last_error;
}

void TwoWire::beginTransmission(uint16_t address)
{
    transmitting = 1;
    txAddress = address;
    txIndex = txQueued; // allow multiple beginTransmission(),write(),endTransmission(false) until endTransmission(true)
    txLength = txQueued;
    last_error = I2C_ERROR_OK;
}

/*stickbreaker isr
 */
uint8_t TwoWire::endTransmission(bool sendStop)  // Assumes Wire.beginTransaction(), Wire.write()
{
    if(transmitting == 1) {
            // txlength is howmany bytes in txbuffer have been use
        last_error = writeTransmission(txAddress, &txBuffer[txQueued], txLength - txQueued, sendStop);
        if(last_error == I2C_ERROR_CONTINUE){
            txQueued = txLength;
        } else if( last_error == I2C_ERROR_OK){
          rxIndex = 0;
          rxLength = rxQueued;
          rxQueued = 0;
          txQueued = 0; // the SendStop=true will restart all Queueing
        }
    } else {
        last_error = I2C_ERROR_NO_BEGIN;
        flush();
    }
    txIndex = 0;
    txLength = 0;
    transmitting = 0;
    return (last_error == I2C_ERROR_CONTINUE)?I2C_ERROR_OK:last_error; // Don't return Continue for compatibility.
}

/* @stickBreaker 11/2017 fix for ReSTART timeout, ISR
 */
uint8_t TwoWire::requestFrom(uint16_t address, uint8_t size, bool sendStop)
{
    //use internal Wire rxBuffer, multiple requestFrom()'s may be pending, try to share rxBuffer
    uint32_t cnt = rxQueued; // currently queued reads, next available position in rxBuffer
    if(cnt < (I2C_BUFFER_LENGTH-1) && (size + cnt) <= I2C_BUFFER_LENGTH) { // any room left in rxBuffer
        rxQueued += size;
    } else { // no room to receive more!
        log_e("rxBuff overflow %d", cnt + size);
        cnt = 0;
        last_error = I2C_ERROR_MEMORY;
        flush();
        return cnt;
    }

    last_error = readTransmission(address, &rxBuffer[cnt], size, sendStop, &cnt);
    rxIndex = 0;
  
    rxLength = cnt;
  
    if( last_error != I2C_ERROR_CONTINUE){ // not a  buffered ReSTART operation
      // so this operation actually moved data, queuing is done.
        rxQueued = 0;
        txQueued = 0; // the SendStop=true will restart all Queueing or error condition
    }
  
    if(last_error != I2C_ERROR_OK){ // ReSTART on read does not return any data
        cnt = 0;
    }
  
    return cnt;
}

size_t TwoWire::write(uint8_t data)
{
    if(transmitting) {
        if(txLength >= I2C_BUFFER_LENGTH) {
            last_error = I2C_ERROR_MEMORY;
            return 0;
        }
        txBuffer[txIndex] = data;
        ++txIndex;
        txLength = txIndex;
        return 1;
    }
    last_error = I2C_ERROR_NO_BEGIN; // no begin, not transmitting
    return 0;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
    for(size_t i = 0; i < quantity; ++i) {
        if(!write(data[i])) {
            return i;
        }
    }
    return quantity;

}

int TwoWire::available(void)
{
    int result = rxLength - rxIndex;
    return result;
}

int TwoWire::read(void)
{
    int value = -1;
    if(rxIndex < rxLength) {
        value = rxBuffer[rxIndex];
        ++rxIndex;
    }
    return value;
}

int TwoWire::peek(void)
{
    int value = -1;
    if(rxIndex < rxLength) {
        value = rxBuffer[rxIndex];
    }
    return value;
}

void TwoWire::flush(void)
{
    rxIndex = 0;
    rxLength = 0;
    txIndex = 0;
    txLength = 0;
    rxQueued = 0;
    txQueued = 0;
    i2cFlush(i2c); // cleanup
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
{
    return requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), static_cast<bool>(sendStop));
}

uint8_t TwoWire::requestFrom(uint16_t address, uint8_t quantity, uint8_t sendStop)
{
    return requestFrom(address, static_cast<size_t>(quantity), static_cast<bool>(sendStop));
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
    return requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), true);
}

uint8_t TwoWire::requestFrom(uint16_t address, uint8_t quantity)
{
    return requestFrom(address, static_cast<size_t>(quantity), true);
}

uint8_t TwoWire::requestFrom(int address, int quantity)
{
    return requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop)
{
    return static_cast<uint8_t>(requestFrom(static_cast<uint16_t>(address), static_cast<size_t>(quantity), static_cast<bool>(sendStop)));
}

void TwoWire::beginTransmission(int address)
{
    beginTransmission(static_cast<uint16_t>(address));
}

void TwoWire::beginTransmission(uint8_t address)
{
    beginTransmission(static_cast<uint16_t>(address));
}

uint8_t TwoWire::endTransmission(void)
{
    return endTransmission(true);
}

/* stickbreaker Nov2017 better error reporting
 */
uint8_t TwoWire::lastError()
{
    return (uint8_t)last_error;
}

const char ERRORTEXT[] =
    "OK\0"
    "DEVICE\0"
    "ACK\0"
    "TIMEOUT\0"
    "BUS\0"
    "BUSY\0"
    "MEMORY\0"
    "CONTINUE\0"
    "NO_BEGIN\0"
    "\0";


char * TwoWire::getErrorText(uint8_t err)
{
    uint8_t t = 0;
    bool found = false;
    char * message = (char*)&ERRORTEXT;

    while(!found && message[0]) {
        found = t == err;
        if(!found) {
            message = message + strlen(message) + 1;
            t++;
        }
    }
    if(!found) {
        return NULL;
    } else {
        return message;
    }
}

/*stickbreaker Dump i2c Interrupt buffer, i2c isr Debugging
 */
 
uint32_t TwoWire::setDebugFlags( uint32_t setBits, uint32_t resetBits){
  return i2cDebug(i2c,setBits,resetBits);
}

bool TwoWire::busy(void){
  return ((i2cGetStatus(i2c) & 16 )==16);
}

TwoWire Wire = TwoWire(0);
TwoWire Wire1 = TwoWire(1);

#elif defined BRIKI_MBC_WB_SAMD // Wire.cpp for SAMD21 mcu

extern "C" {
#include <string.h>
}

#include <Arduino.h>
#include <wiring_private.h>

#include "Wire.h"

TwoWire::TwoWire(SERCOM * s, uint8_t pinSDA, uint8_t pinSCL)
{
  this->sercom = s;
  this->_uc_pinSDA=pinSDA;
  this->_uc_pinSCL=pinSCL;
  transmissionBegun = false;
}

void TwoWire::begin(void) {
  //Master Mode
  sercom->initMasterWIRE(TWI_CLOCK);
  sercom->enableWIRE();

  pinPeripheral(_uc_pinSDA, g_APinDescription[_uc_pinSDA].ulPinType);
  pinPeripheral(_uc_pinSCL, g_APinDescription[_uc_pinSCL].ulPinType);
}

void TwoWire::begin(uint8_t address, bool enableGeneralCall) {
  //Slave mode
  sercom->initSlaveWIRE(address, enableGeneralCall);
  sercom->enableWIRE();

  pinPeripheral(_uc_pinSDA, g_APinDescription[_uc_pinSDA].ulPinType);
  pinPeripheral(_uc_pinSCL, g_APinDescription[_uc_pinSCL].ulPinType);
}

void TwoWire::setClock(uint32_t baudrate) {
  sercom->disableWIRE();
  sercom->initMasterWIRE(baudrate);
  sercom->enableWIRE();
}

void TwoWire::end() {
  sercom->disableWIRE();
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit)
{
  if(quantity == 0)
  {
    return 0;
  }

  size_t byteRead = 0;

  rxBuffer.clear();

  if(sercom->startTransmissionWIRE(address, WIRE_READ_FLAG))
  {
    // Read first data
    rxBuffer.store_char(sercom->readDataWIRE());

    bool busOwner;
    // Connected to slave
    for (byteRead = 1; byteRead < quantity && (busOwner = sercom->isBusOwnerWIRE()); ++byteRead)
    {
      sercom->prepareAckBitWIRE();                          // Prepare Acknowledge
      sercom->prepareCommandBitsWire(WIRE_MASTER_ACT_READ); // Prepare the ACK command for the slave
      rxBuffer.store_char(sercom->readDataWIRE());          // Read data and send the ACK
    }
    sercom->prepareNackBitWIRE();                           // Prepare NACK to stop slave transmission
    //sercom->readDataWIRE();                               // Clear data register to send NACK

    if (stopBit && busOwner)
    {
      sercom->prepareCommandBitsWire(WIRE_MASTER_ACT_STOP);   // Send Stop unless arbitration was lost
    }

    if (!busOwner)
    {
      byteRead--;   // because last read byte was garbage/invalid
    }
  }

  return byteRead;
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity)
{
  return requestFrom(address, quantity, true);
}

void TwoWire::beginTransmission(uint8_t address) {
  // save address of target and clear buffer
  txAddress = address;
  txBuffer.clear();

  transmissionBegun = true;
}

// Errors:
//  0 : Success
//  1 : Data too long
//  2 : NACK on transmit of address
//  3 : NACK on transmit of data
//  4 : Other error
uint8_t TwoWire::endTransmission(bool stopBit)
{
  transmissionBegun = false ;

  // Start I2C transmission
  if ( !sercom->startTransmissionWIRE( txAddress, WIRE_WRITE_FLAG ) )
  {
    sercom->prepareCommandBitsWire(WIRE_MASTER_ACT_STOP);
    return 2 ;  // Address error
  }

  // Send all buffer
  while( txBuffer.available() )
  {
    // Trying to send data
    if ( !sercom->sendDataMasterWIRE( txBuffer.read_char() ) )
    {
      sercom->prepareCommandBitsWire(WIRE_MASTER_ACT_STOP);
      return 3 ;  // Nack or error
    }
  }
  
  if (stopBit)
  {
    sercom->prepareCommandBitsWire(WIRE_MASTER_ACT_STOP);
  }   

  return 0;
}

uint8_t TwoWire::endTransmission()
{
  return endTransmission(true);
}

size_t TwoWire::write(uint8_t ucData)
{
  // No writing, without begun transmission or a full buffer
  if ( !transmissionBegun || txBuffer.isFull() )
  {
    return 0 ;
  }

  txBuffer.store_char( ucData ) ;

  return 1 ;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
  //Try to store all data
  for(size_t i = 0; i < quantity; ++i)
  {
    //Return the number of data stored, when the buffer is full (if write return 0)
    if(!write(data[i]))
      return i;
  }

  //All data stored
  return quantity;
}

int TwoWire::available(void)
{
  return rxBuffer.available();
}

int TwoWire::read(void)
{
  return rxBuffer.read_char();
}

int TwoWire::peek(void)
{
  return rxBuffer.peek();
}

void TwoWire::flush(void)
{
  // Do nothing, use endTransmission(..) to force
  // data transfer.
}

void TwoWire::onReceive(void(*function)(int))
{
  onReceiveCallback = function;
}

void TwoWire::onRequest(void(*function)(void))
{
  onRequestCallback = function;
}

void TwoWire::onService(void)
{
  if ( sercom->isSlaveWIRE() )
  {
    if(sercom->isStopDetectedWIRE() || 
        (sercom->isAddressMatch() && sercom->isRestartDetectedWIRE() && !sercom->isMasterReadOperationWIRE())) //Stop or Restart detected
    {
      sercom->prepareAckBitWIRE();
      sercom->prepareCommandBitsWire(0x03);

      //Calling onReceiveCallback, if exists
      if(onReceiveCallback)
      {
        onReceiveCallback(available());
      }
      
      rxBuffer.clear();
    }
    else if(sercom->isAddressMatch())  //Address Match
    {
      sercom->prepareAckBitWIRE();
      sercom->prepareCommandBitsWire(0x03);

      if(sercom->isMasterReadOperationWIRE()) //Is a request ?
      {
        txBuffer.clear();

        transmissionBegun = true;

        //Calling onRequestCallback, if exists
        if(onRequestCallback)
        {
          onRequestCallback();
        }
      }
    }
    else if(sercom->isDataReadyWIRE())
    {
      if (sercom->isMasterReadOperationWIRE())
      {
        uint8_t c = 0xff;

        if( txBuffer.available() ) {
          c = txBuffer.read_char();
        }

        transmissionBegun = sercom->sendDataSlaveWIRE(c);
      } else { //Received data
        if (rxBuffer.isFull()) {
          sercom->prepareNackBitWIRE(); 
        } else {
          //Store data
          rxBuffer.store_char(sercom->readDataWIRE());

          sercom->prepareAckBitWIRE(); 
        }

        sercom->prepareCommandBitsWire(0x03);
      }
    }
  }
}

#if WIRE_INTERFACES_COUNT > 0
  /* In case new variant doesn't define these macros,
   * we put here the ones for Arduino Zero.
   *
   * These values should be different on some variants!
   */
  #ifndef PERIPH_WIRE
    #define PERIPH_WIRE          sercom3
    #define WIRE_IT_HANDLER      SERCOM3_Handler
  #endif // PERIPH_WIRE
  TwoWire Wire(&PERIPH_WIRE, PIN_WIRE_SDA, PIN_WIRE_SCL);

  void WIRE_IT_HANDLER(void) {
    Wire.onService();
  }
#endif

#if WIRE_INTERFACES_COUNT > 1
  TwoWire Wire1(&PERIPH_WIRE1, PIN_WIRE1_SDA, PIN_WIRE1_SCL);

  void WIRE1_IT_HANDLER(void) {
    Wire1.onService();
  }
#endif

#if WIRE_INTERFACES_COUNT > 2
  TwoWire Wire2(&PERIPH_WIRE2, PIN_WIRE2_SDA, PIN_WIRE2_SCL);

  void WIRE2_IT_HANDLER(void) {
    Wire2.onService();
  }
#endif

#if WIRE_INTERFACES_COUNT > 3
  TwoWire Wire3(&PERIPH_WIRE3, PIN_WIRE3_SDA, PIN_WIRE3_SCL);

  void WIRE3_IT_HANDLER(void) {
    Wire3.onService();
  }
#endif

#if WIRE_INTERFACES_COUNT > 4
  TwoWire Wire4(&PERIPH_WIRE4, PIN_WIRE4_SDA, PIN_WIRE4_SCL);

  void WIRE4_IT_HANDLER(void) {
    Wire4.onService();
  }
#endif

#if WIRE_INTERFACES_COUNT > 5
  TwoWire Wire5(&PERIPH_WIRE5, PIN_WIRE5_SDA, PIN_WIRE5_SCL);

  void WIRE5_IT_HANDLER(void) {
    Wire5.onService();
  }
#endif


#endif