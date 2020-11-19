/* 
 SPI.cpp - SPI library for esp8266

 Copyright (c) 2015 Hristo Gochkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.
 
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
 */

#include "SPI.h"

#ifdef BRIKI_MBC_WB_ESP

SPIClass::SPIClass(uint8_t spi_bus)
    :_spi_num(spi_bus)
    ,_spi(NULL)
    ,_use_hw_ss(false)
    ,_sck(-1)
    ,_miso(-1)
    ,_mosi(-1)
    ,_ss(-1)
    ,_div(0)
    ,_freq(1000000)
    ,_inTransaction(false)
{}

void SPIClass::begin(int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
{
    if(_spi) {
        return;
    }

    if(!_div) {
        _div = spiFrequencyToClockDiv(_freq);
    }

    _spi = spiStartBus(_spi_num, _div, SPI_MODE0, SPI_MSBFIRST);
    if(!_spi) {
        return;
    }

    if(sck == -1 && miso == -1 && mosi == -1 && ss == -1) {
		if (!SPI_PIN_DEFAULT){
			_sck = (_spi_num == VSPI) ? 35 : 32;
			_miso = (_spi_num == VSPI) ? 36 : 33;
			_mosi = (_spi_num == VSPI) ? 37 : 34;
			_ss = (_spi_num == VSPI) ? 10 : 9;
		} else {
            _sck = SPI_SCK;
			_miso = SPI_MISO;
			_mosi = SPI_MOSI;
			_ss = SPI_SS;	
		}			
    } else {
		// check on SPI pins
		#ifdef BRIKI_MBC_WB_ESP
			if((sck > PINS_COUNT || g_APinDescription[sck].ulPort == PIO_NOT_A_PORT) || (miso > PINS_COUNT || g_APinDescription[miso].ulPort == PIO_NOT_A_PORT) || (mosi > PINS_COUNT || g_APinDescription[mosi].ulPort == PIO_NOT_A_PORT) || (ss > PINS_COUNT || g_APinDescription[ss].ulPort == PIO_NOT_A_PORT)) {
				return;
			}
		#endif
        _sck = sck;
        _miso = miso;
        _mosi = mosi;
        _ss = ss;
    }

    spiAttachSCK(_spi, _sck);
    spiAttachMISO(_spi, _miso);
    spiAttachMOSI(_spi, _mosi);

}

void SPIClass::end()
{
    if(!_spi) {
        return;
    }
    spiDetachSCK(_spi, _sck);
    spiDetachMISO(_spi, _miso);
    spiDetachMOSI(_spi, _mosi);
    setHwCs(false);
    spiStopBus(_spi);
    _spi = NULL;
}

void SPIClass::setHwCs(bool use)
{
    if(use && !_use_hw_ss) {
        spiAttachSS(_spi, 0, _ss);
        spiSSEnable(_spi);
    } else if(_use_hw_ss) {
        spiSSDisable(_spi);
        spiDetachSS(_spi, _ss);
    }
    _use_hw_ss = use;
}

void SPIClass::setFrequency(uint32_t freq)
{
    //check if last freq changed
    uint32_t cdiv = spiGetClockDiv(_spi);
    if(_freq != freq || _div != cdiv) {
        _freq = freq;
        _div = spiFrequencyToClockDiv(_freq);
        spiSetClockDiv(_spi, _div);
    }
}

void SPIClass::setClockDivider(uint32_t clockDiv)
{
    _div = clockDiv;
    spiSetClockDiv(_spi, _div);
}

uint32_t SPIClass::getClockDivider()
{
    return spiGetClockDiv(_spi);
}

void SPIClass::setDataMode(uint8_t dataMode)
{
    spiSetDataMode(_spi, dataMode);
}

void SPIClass::setBitOrder(uint8_t bitOrder)
{
    spiSetBitOrder(_spi, bitOrder);
}

void SPIClass::beginTransaction(SPISettings settings)
{
    //check if last freq changed
    uint32_t cdiv = spiGetClockDiv(_spi);
    if(_freq != settings._clock || _div != cdiv) {
        _freq = settings._clock;
        _div = spiFrequencyToClockDiv(_freq);
    }
    spiTransaction(_spi, _div, settings._dataMode, settings._bitOrder);
    _inTransaction = true;
}

void SPIClass::endTransaction()
{
    if(_inTransaction){
        _inTransaction = false;
        spiEndTransaction(_spi);
    }
}

void SPIClass::write(uint8_t data)
{
    if(_inTransaction){
        return spiWriteByteNL(_spi, data);
    }
    spiWriteByte(_spi, data);
}

uint8_t SPIClass::transfer(uint8_t data)
{
    if(_inTransaction){
        return spiTransferByteNL(_spi, data);
    }
    return spiTransferByte(_spi, data);
}

void SPIClass::write16(uint16_t data)
{
    if(_inTransaction){
        return spiWriteShortNL(_spi, data);
    }
    spiWriteWord(_spi, data);
}

uint16_t SPIClass::transfer16(uint16_t data)
{
    if(_inTransaction){
        return spiTransferShortNL(_spi, data);
    }
    return spiTransferWord(_spi, data);
}

void SPIClass::write32(uint32_t data)
{
    if(_inTransaction){
        return spiWriteLongNL(_spi, data);
    }
    spiWriteLong(_spi, data);
}

uint32_t SPIClass::transfer32(uint32_t data)
{
    if(_inTransaction){
        return spiTransferLongNL(_spi, data);
    }
    return spiTransferLong(_spi, data);
}

void SPIClass::transferBits(uint32_t data, uint32_t * out, uint8_t bits)
{
    if(_inTransaction){
        return spiTransferBitsNL(_spi, data, out, bits);
    }
    spiTransferBits(_spi, data, out, bits);
}

/**
 * @param data uint8_t *
 * @param size uint32_t
 */
void SPIClass::writeBytes(const uint8_t * data, uint32_t size)
{
    if(_inTransaction){
        return spiWriteNL(_spi, data, size);
    }
    spiSimpleTransaction(_spi);
    spiWriteNL(_spi, data, size);
    spiEndTransaction(_spi);
}

void SPIClass::transfer(uint8_t * data, uint32_t size) 
{ 
	transferBytes(data, data, size); 
}

/**
 * @param data void *
 * @param size uint32_t
 */
void SPIClass::writePixels(const void * data, uint32_t size)
{
    if(_inTransaction){
        return spiWritePixelsNL(_spi, data, size);
    }
    spiSimpleTransaction(_spi);
    spiWritePixelsNL(_spi, data, size);
    spiEndTransaction(_spi);
}

/**
 * @param data uint8_t * data buffer. can be NULL for Read Only operation
 * @param out  uint8_t * output buffer. can be NULL for Write Only operation
 * @param size uint32_t
 */
void SPIClass::transferBytes(uint8_t * data, uint8_t * out, uint32_t size)
{
    if(_inTransaction){
        return spiTransferBytesNL(_spi, data, out, size);
    }
    spiTransferBytes(_spi, data, out, size);
}

/**
 * @param data uint8_t *
 * @param size uint8_t  max for size is 64Byte
 * @param repeat uint32_t
 */
void SPIClass::writePattern(uint8_t * data, uint8_t size, uint32_t repeat)
{
    if(size > 64) {
        return;    //max Hardware FIFO
    }

    uint32_t byte = (size * repeat);
    uint8_t r = (64 / size);
    const uint8_t max_bytes_FIFO = r * size;    // Max number of whole patterns (in bytes) that can fit into the hardware FIFO

    while(byte) {
        if(byte > max_bytes_FIFO) {
            writePattern_(data, size, r);
            byte -= max_bytes_FIFO;
        } else {
            writePattern_(data, size, (byte / size));
            byte = 0;
        }
    }
}

void SPIClass::writePattern_(uint8_t * data, uint8_t size, uint8_t repeat)
{
    uint8_t bytes = (size * repeat);
    uint8_t buffer[64];
    uint8_t * bufferPtr = &buffer[0];
    uint8_t * dataPtr;
    uint8_t dataSize = bytes;
    for(uint8_t i = 0; i < repeat; i++) {
        dataSize = size;
        dataPtr = data;
        while(dataSize--) {
            *bufferPtr = *dataPtr;
            dataPtr++;
            bufferPtr++;
        }
    }

    writeBytes(&buffer[0], bytes);
}

SPIClass SPI(VSPI);

#elif defined BRIKI_MBC_WB_SAMD

#include <Arduino.h>
#include <wiring_private.h>
#include <assert.h>

#define SPI_IMODE_NONE   0
#define SPI_IMODE_EXTINT 1
#define SPI_IMODE_GLOBAL 2

SPIClass::SPIClass(SERCOM *p_sercom, uint8_t uc_pinMISO, uint8_t uc_pinSCK, uint8_t uc_pinMOSI, SercomSpiTXPad PadTx, SercomRXPad PadRx) : settings(SPISettings(0, MSBFIRST, SPI_MODE0))
{
  initialized = false;
  assert(p_sercom != NULL);
  _p_sercom = p_sercom;

  // pins
  _uc_pinMiso = uc_pinMISO;
  _uc_pinSCK = uc_pinSCK;
  _uc_pinMosi = uc_pinMOSI;

  // SERCOM pads
  _padTx=PadTx;
  _padRx=PadRx;
}

void SPIClass::begin()
{
  init();

  // PIO init
  pinPeripheral(_uc_pinMiso, g_APinDescription[_uc_pinMiso].ulPinType);
  pinPeripheral(_uc_pinSCK, g_APinDescription[_uc_pinSCK].ulPinType);
  pinPeripheral(_uc_pinMosi, g_APinDescription[_uc_pinMosi].ulPinType);

  config(DEFAULT_SPI_SETTINGS);
}

void SPIClass::init()
{
  if (initialized)
    return;
  interruptMode = SPI_IMODE_NONE;
  interruptSave = 0;
  interruptMask = 0;
  initialized = true;
}

void SPIClass::config(SPISettings settings)
{
  if (this->settings != settings) {
    this->settings = settings;
    _p_sercom->disableSPI();

    _p_sercom->initSPI(_padTx, _padRx, SPI_CHAR_SIZE_8_BITS, settings.bitOrder);
    _p_sercom->initSPIClock(settings.dataMode, settings.clockFreq);

    _p_sercom->enableSPI();
  }
}

void SPIClass::end()
{
  _p_sercom->resetSPI();
  initialized = false;
}

#ifndef interruptsStatus
#define interruptsStatus() __interruptsStatus()
static inline unsigned char __interruptsStatus(void) __attribute__((always_inline, unused));
static inline unsigned char __interruptsStatus(void)
{
  // See http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/CHDBIBGJ.html
  return (__get_PRIMASK() ? 0 : 1);
}
#endif

void SPIClass::usingInterrupt(int interruptNumber)
{
  if ((interruptNumber == NOT_AN_INTERRUPT) || (interruptNumber == EXTERNAL_INT_NMI))
    return;

  uint8_t irestore = interruptsStatus();
  noInterrupts();

  if (interruptNumber >= EXTERNAL_NUM_INTERRUPTS)
    interruptMode = SPI_IMODE_GLOBAL;
  else
  {
    interruptMode |= SPI_IMODE_EXTINT;
    interruptMask |= (1 << g_APinDescription[interruptNumber].ulExtInt);
  }

  if (irestore)
    interrupts();
}

void SPIClass::notUsingInterrupt(int interruptNumber)
{
  if ((interruptNumber == NOT_AN_INTERRUPT) || (interruptNumber == EXTERNAL_INT_NMI))
    return;

  if (interruptMode & SPI_IMODE_GLOBAL)
    return; // can't go back, as there is no reference count

  uint8_t irestore = interruptsStatus();
  noInterrupts();

  interruptMask &= ~(1 << g_APinDescription[interruptNumber].ulExtInt);

  if (interruptMask == 0)
    interruptMode = SPI_IMODE_NONE;

  if (irestore)
    interrupts();
}

void SPIClass::beginTransaction(SPISettings settings)
{
  if (interruptMode != SPI_IMODE_NONE)
  {
    if (interruptMode & SPI_IMODE_GLOBAL)
    {
      interruptSave = interruptsStatus();
      noInterrupts();
    }
    else if (interruptMode & SPI_IMODE_EXTINT)
      EIC->INTENCLR.reg = EIC_INTENCLR_EXTINT(interruptMask);
  }

  config(settings);
}

void SPIClass::endTransaction(void)
{
  if (interruptMode != SPI_IMODE_NONE)
  {
    if (interruptMode & SPI_IMODE_GLOBAL)
    {
      if (interruptSave)
        interrupts();
    }
    else if (interruptMode & SPI_IMODE_EXTINT)
      EIC->INTENSET.reg = EIC_INTENSET_EXTINT(interruptMask);
  }
}

void SPIClass::setBitOrder(BitOrder order)
{
  if (order == LSBFIRST) {
    _p_sercom->setDataOrderSPI(LSB_FIRST);
  } else {
    _p_sercom->setDataOrderSPI(MSB_FIRST);
  }
}

void SPIClass::setDataMode(uint8_t mode)
{
  switch (mode)
  {
    case SPI_MODE0:
      _p_sercom->setClockModeSPI(SERCOM_SPI_MODE_0);
      break;

    case SPI_MODE1:
      _p_sercom->setClockModeSPI(SERCOM_SPI_MODE_1);
      break;

    case SPI_MODE2:
      _p_sercom->setClockModeSPI(SERCOM_SPI_MODE_2);
      break;

    case SPI_MODE3:
      _p_sercom->setClockModeSPI(SERCOM_SPI_MODE_3);
      break;

    default:
      break;
  }
}

void SPIClass::setClockDivider(uint8_t div)
{
  if (div < SPI_MIN_CLOCK_DIVIDER) {
    _p_sercom->setBaudrateSPI(SPI_MIN_CLOCK_DIVIDER);
  } else {
    _p_sercom->setBaudrateSPI(div);
  }
}

byte SPIClass::transfer(uint8_t data)
{
  return _p_sercom->transferDataSPI(data);
}

uint16_t SPIClass::transfer16(uint16_t data) {
  union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } t;

  t.val = data;

  if (_p_sercom->getDataOrderSPI() == LSB_FIRST) {
    t.lsb = transfer(t.lsb);
    t.msb = transfer(t.msb);
  } else {
    t.msb = transfer(t.msb);
    t.lsb = transfer(t.lsb);
  }

  return t.val;
}

void SPIClass::transfer(void *buf, size_t count)
{
  uint8_t *buffer = reinterpret_cast<uint8_t *>(buf);
  for (size_t i=0; i<count; i++) {
    *buffer = transfer(*buffer);
    buffer++;
  }
}

void SPIClass::transfer(void *buf, size_t count, void* dataRead)
{
  uint8_t *buffer = reinterpret_cast<uint8_t *>(buf);
  uint8_t *readBuf = reinterpret_cast<uint8_t *>(dataRead);
  while(count-- > 0)
  {
    SERCOM1->SPI.DATA.bit.DATA = *buffer++;            // Initiate byte transfer.
    while (SERCOM1->SPI.INTFLAG.bit.RXC == 0);       // Wait for data to be available in the receive buffer.
    *readBuf++ = SERCOM1->SPI.DATA.bit.DATA & 0xFF;   // Read received byte, then increment pointer into buffer.
  }
}

void SPIClass::attachInterrupt() {
  // Should be enableInterrupt()
}

void SPIClass::detachInterrupt() {
  // Should be disableInterrupt()
}

#if SPI_INTERFACES_COUNT > 0
  /* In case new variant doesn't define these macros,
   * we put here the ones for Arduino Zero.
   *
   * These values should be different on some variants!
   *
   * The SPI PAD values can be found in cores/arduino/SERCOM.h:
   *   - SercomSpiTXPad
   *   - SercomRXPad
   */
  #ifndef PERIPH_SPI
    #define PERIPH_SPI           sercom4
    #define PAD_SPI_TX           SPI_PAD_2_SCK_3
    #define PAD_SPI_RX           SERCOM_RX_PAD_0
  #endif // PERIPH_SPI
  SPIClass SPI (&PERIPH_SPI,  PIN_SPI_MISO,  PIN_SPI_SCK,  PIN_SPI_MOSI,  PAD_SPI_TX,  PAD_SPI_RX);
#endif
#if SPI_INTERFACES_COUNT > 1
  SPIClass SPI1(&PERIPH_SPI1, PIN_SPI1_MISO, PIN_SPI1_SCK, PIN_SPI1_MOSI, PAD_SPI1_TX, PAD_SPI1_RX);
#endif
#if SPI_INTERFACES_COUNT > 2
  SPIClass SPI2(&PERIPH_SPI2, PIN_SPI2_MISO, PIN_SPI2_SCK, PIN_SPI2_MOSI, PAD_SPI2_TX, PAD_SPI2_RX);
#endif
#if SPI_INTERFACES_COUNT > 3
  SPIClass SPI3(&PERIPH_SPI3, PIN_SPI3_MISO, PIN_SPI3_SCK, PIN_SPI3_MOSI, PAD_SPI3_TX, PAD_SPI3_RX);
#endif
#if SPI_INTERFACES_COUNT > 4
  SPIClass SPI4(&PERIPH_SPI4, PIN_SPI4_MISO, PIN_SPI4_SCK, PIN_SPI4_MOSI, PAD_SPI4_TX, PAD_SPI4_RX);
#endif
#if SPI_INTERFACES_COUNT > 5
  SPIClass SPI5(&PERIPH_SPI5, PIN_SPI5_MISO, PIN_SPI5_SCK, PIN_SPI5_MOSI, PAD_SPI5_TX, PAD_SPI5_RX);
#endif

#endif
