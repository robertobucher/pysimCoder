/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _VARIANT_ARDUINO_ZERO_
#define _VARIANT_ARDUINO_ZERO_

// The definitions here needs a SAMD core >=1.6.10
#define ARDUINO_SAMD_VARIANT_COMPLIANCE 10610

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/** Frequency of the board main oscillator */
#define VARIANT_MAINOSC		(32768ul)

/** Master clock frequency */
#define VARIANT_MCK			  (48000000ul)

//#define HW_VER_1_0	// HW ver. 1.0
#define HW_VER_1_1		// HW ver. 1.1

//#define VERY_LOW_POWER
/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
#include "SERCOM.h"
#include "Uart.h"
#endif // __cplusplus

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/*----------------------------------------------------------------------------
 *        Pins
 *----------------------------------------------------------------------------*/

// ESP PIN definitions
#	define ESP_ON				  (40u)
#	define ESP_ON_PIN			PORT_PA22
#	define ESP_RST				(41u)
#	define ESP_RST_PIN		PORT_PA20
#	define ESP_BOOT				(42u)
#	define ESP_BOOT_PIN		PORT_PA15
#	define ESP_CS				  (45u)
#	define ACK_PIN				(47u)
 
// Number of pins defined in PinDescription array
#	define PINS_COUNT           (38u)
#	define NUM_DIGITAL_PINS     (32u)
#	define NUM_ANALOG_INPUTS    (18u)
#	define NUM_ANALOG_OUTPUTS   (1u)
#	define ANALOG_PIN_OFFSET1   (26u)
#	define ANALOG_PIN_OFFSET2   (52u)

#define analogInputToDigitalPin(p)  ((p < NUM_ANALOG_INPUTS) ? ((p < 6) ? (p) + ANALOG_PIN_OFFSET1 : (p + ANALOG_PIN_OFFSET2 - 6)) : -1)
#define digitalPinToPort(P)        ( &(PORT->Group[g_APinDescription[P].ulPort]) )
#define digitalPinToBitMask(P)     ( 1 << g_APinDescription[P].ulPin )
//#define analogInPinToBit(P)        ( )
#define portOutputRegister(port)   ( &(port->OUT.reg) )
#define portInputRegister(port)    ( &(port->IN.reg) )
#define portModeRegister(port)     ( &(port->DIR.reg) )
#define digitalPinHasPWM(P)        ( g_APinDescription[P].ulPWMChannel != NOT_ON_PWM || g_APinDescription[P].ulTCChannel != NOT_ON_TIMER )

/*
 * digitalPinToTimer(..) is AVR-specific and is not defined for SAMD
 * architecture. If you need to check if a pin supports PWM you must
 * use digitalPinHasPWM(..).
 *
 * https://github.com/arduino/Arduino/issues/1833
 */
// #define digitalPinToTimer(P)

// LEDs
#ifdef BRIKI_ABC
# if defined HW_VER_1_0
#	  define LED_BUILTIN          (50u)
# elif defined HW_VER_1_1
#	  define LED_BUILTIN          (74u)
# endif
#	define SDCARD_SS_PIN        (16u)
#else
#	define LED_BUILTIN          (73u)
#endif

/*
 * Analog pins
 */
# define PIN_A0               (ANALOG_PIN_OFFSET1)      // SAMD analog
# define PIN_A1               (ANALOG_PIN_OFFSET1 + 1)  // SAMD analog
# define PIN_A2               (ANALOG_PIN_OFFSET1 + 2)  // SAMD analog
# define PIN_A3               (ANALOG_PIN_OFFSET1 + 3)  // SAMD analog
# define PIN_A4               (ANALOG_PIN_OFFSET1 + 4)  // SAMD analog
# define PIN_A5               (ANALOG_PIN_OFFSET1 + 5)  // SAMD analog
# define PIN_A6               (ANALOG_PIN_OFFSET2)      // SAMD analog
# define PIN_A7               (ANALOG_PIN_OFFSET2 + 1)  // SAMD analog
# define PIN_A8               (ANALOG_PIN_OFFSET2 + 2)  // SAMD analog
# define PIN_A9               (ANALOG_PIN_OFFSET2 + 3)  // SAMD analog
# define PIN_A10              (ANALOG_PIN_OFFSET2 + 4)  // SAMD analog
# define PIN_A11              (ANALOG_PIN_OFFSET2 + 5)  // SAMD analog
# define PIN_A12              (ANALOG_PIN_OFFSET2 + 6)  // SAMD analog
# define PIN_A13              (ANALOG_PIN_OFFSET2 + 7)  // SAMD analog
# define PIN_A14              (ANALOG_PIN_OFFSET2 + 8)  // ESP analog
# define PIN_A15              (ANALOG_PIN_OFFSET2 + 9)  // ESP analog
# define PIN_A16              (ANALOG_PIN_OFFSET2 + 10) // ESP analog
# define PIN_A17              (ANALOG_PIN_OFFSET2 + 11) // ESP analog
// A18 to A26 works only if WiFi is disabled
# define PIN_A18              (ANALOG_PIN_OFFSET2 + 12) // ESP analog
# define PIN_A19              (ANALOG_PIN_OFFSET2 + 13) // ESP analog
# define PIN_A20              (ANALOG_PIN_OFFSET2 + 14) // ESP analog
# define PIN_A21              (ANALOG_PIN_OFFSET2 + 15) // ESP analog
# define PIN_A22              (ANALOG_PIN_OFFSET2 + 16) // ESP analog
# define PIN_A23              (ANALOG_PIN_OFFSET2 + 17) // ESP analog
# define PIN_A24              (ANALOG_PIN_OFFSET2 + 18) // ESP analog
# define PIN_A25              (ANALOG_PIN_OFFSET2 + 19) // ESP analog
# define PIN_A26              (ANALOG_PIN_OFFSET2 + 20) // ESP analog
# define PIN_DAC0             (ANALOG_PIN_OFFSET2) // SAMD DAC

static const uint8_t A0  = PIN_A0;
static const uint8_t A1  = PIN_A1;
static const uint8_t A2  = PIN_A2;
static const uint8_t A3  = PIN_A3;
static const uint8_t A4  = PIN_A4;
static const uint8_t A5  = PIN_A5;
static const uint8_t A6  = PIN_A6;
static const uint8_t A7  = PIN_A7;
static const uint8_t A8  = PIN_A8;
static const uint8_t A9  = PIN_A9;
static const uint8_t A10  = PIN_A10;
static const uint8_t A11  = PIN_A11;
static const uint8_t A12  = PIN_A12;
static const uint8_t A13  = PIN_A13;
static const uint8_t A14  = PIN_A14;
static const uint8_t A15  = PIN_A15;
static const uint8_t A16  = PIN_A16;
static const uint8_t A17  = PIN_A17;
static const uint8_t A18  = PIN_A18;
static const uint8_t A19  = PIN_A19;
static const uint8_t A20  = PIN_A20;
static const uint8_t A21  = PIN_A21;
static const uint8_t A22  = PIN_A22;
static const uint8_t A23  = PIN_A23;
static const uint8_t A24  = PIN_A24;
static const uint8_t A25  = PIN_A25;
static const uint8_t A26  = PIN_A26;

static const uint8_t DAC0 = PIN_DAC0;

typedef enum {
  // 0..25 - Digital pins
  // 0..10 - ESP GPIO
  espGpio1  = 5,
  espGpio3,
  // 11..15 - SAMD GPIO
  samPA23   = 11,
  samPA14,
  // 16..25 - SAMD GPIO
  samPA27   = 16,
  samPB11   = 21,
  samPB10,
  samPA12,
  samPA13,
  // 26..31 - Analog pins
  samPA2    = 26,
  samPA3,
  samPA4,
  samPA5,
  samPA6,
  samPA7,
  // 32..37 - ESP GPIO
  espGpio16 = 36,
  espGpio17,
  // end Arduino pinout
  // 38..39 - SWD
  samPA31,
  samPA30,
  // 40..42 - ESP Control (Enable, Boot, Power ON/OFF)
  #if defined HW_VER_1_0
    samPA22,
  #elif defined HW_VER_1_1
    samPA28,
  #endif
  samPA20,
  samPA15,
  // 43..47 SPI - connected to ESP
  samPA16,
  samPA17,
  samPA18,
  samPA19,
  samPA21,
  // 48..49 - USB
  samPA24,
  samPA25,
  // 50..51 - ESP programming UART
  samPB22,
  samPB23,
  // 52..72 - Additional analog pins
  samPB2,
  samPB3,
  samPB8,
  samPB9,
  samPA11,
  samPA10,
  samPA9,
  samPA8,
  espGpio32,
  espGpio33,
  espGpio35,
  espGpio34,
  espGpio25,
  espGpio27,
  espGpio26,
  espGpio4,
  espGpio2,
  espGpio15,
  espGpio13,
  espGpio12,
  espGpio14,
  // 73 - fake pin
  fakeGpio,
  #if defined HW_VER_1_1
    samPA22,
  #endif
} hwPinMap_e;

// Other pins
/*
#define PIN_ATN              (38ul)
static const uint8_t ATN = PIN_ATN;
*/

// Serial interfaces: Serial (SERCOM4) - external UART
#	define PIN_SERIAL1_TX      (22ul)
#	define PIN_SERIAL1_RX      (21ul)
#	define PAD_SERIAL1_TX      (UART_TX_PAD_2)
#	define PAD_SERIAL1_RX      (SERCOM_RX_PAD_3)

// SerialCompanion (SERCOM5) - ESP's SAM serial interface
#define PIN_SERIAL2_TX       (50ul)
#define PIN_SERIAL2_RX       (51ul)
#define PAD_SERIAL2_TX       (UART_TX_PAD_2)
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_3)

// SPI Interfaces - directly connected to the MBC
#define SPI_INTERFACES_COUNT 2
#define PIN_SPI_CS           (13u)
#define PIN_SPI_MISO         (25u)
#define PIN_SPI_MOSI         (23u)
#define PIN_SPI_SCK          (24u)
#define PERIPH_SPI           sercom2
#define PAD_SPI_TX           SPI_PAD_0_SCK_1 // MOSI pad[0], SCK pad[1]
#define PAD_SPI_RX           SERCOM_RX_PAD_3 // MISO pad[3]

#define PIN_SPI1_CS          (45u)
#define PIN_SPI1_MISO        (46u)
#define PIN_SPI1_MOSI        (43u)
#define PIN_SPI1_SCK         (44u)
#define PERIPH_SPI1          sercom1
#define PAD_SPI1_TX          SPI_PAD_0_SCK_1 // MOSI pad[0], SCK pad[1]
#define PAD_SPI1_RX          SERCOM_RX_PAD_3 // MISO pad[3]

static const uint8_t INT_SS   = PIN_SPI1_CS;
static const uint8_t INT_MOSI = PIN_SPI1_MOSI;
static const uint8_t INT_MISO = PIN_SPI1_MISO;
static const uint8_t INT_SCK  = PIN_SPI1_SCK;

static const uint8_t SS   = PIN_SPI_CS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

#	define WIRE_INTERFACES_COUNT 1
#	define PIN_WIRE_SCL         (14u)
#	define PIN_WIRE_SDA         (15u)
#	define PERIPH_WIRE          sercom0
#	define WIRE_IT_HANDLER      SERCOM0_Handler

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

// USB
//#	define PIN_USB_HOST_ENABLE (16ul)
#	define PIN_USB_DM          (48ul)
#	define PIN_USB_DP          (49ul)

// I2S Interfaces - TO DO [14/09/18]
#	define I2S_INTERFACES_COUNT 0

#	define I2S_DEVICE			0
#	define I2S_CLOCK_GENERATOR 3
#	define PIN_I2S_SD          (9u)
#	define PIN_I2S_SCK         (1u)
#	define PIN_I2S_FS          (0u)

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/
#ifdef __cplusplus

/*	=========================
 *	===== SERCOM DEFINITION
 *	=========================
*/
extern SERCOM sercom0;
extern SERCOM sercom1;
extern SERCOM sercom2;
extern SERCOM sercom3;
extern SERCOM sercom4;
extern SERCOM sercom5;
//extern SERCOM virtualSercom_SPI;
//extern SERCOM virtualSercom_I2C;

extern Uart Serial1;
extern Uart SerialCompanion;
//extern Uart Serial1;

#endif

// These serial port names are intended to allow libraries and architecture-neutral
// sketches to automatically default to the correct port name for a particular type
// of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
// the first hardware serial port whose RX/TX pins are not dedicated to another use.
//
// SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
//
// SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
//
// SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
//
// SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
//
// SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
//                            pins are NOT connected to anything by default.
#define SERIAL_PORT_USBVIRTUAL      Serial
#define SERIAL_PORT_MONITOR         Serial1
// Serial has no physical pins broken out, so it's not listed as HARDWARE port
/*
#define SERIAL_PORT_HARDWARE        Serial1
#define SERIAL_PORT_HARDWARE_OPEN   Serial1
*/

#endif /* _VARIANT_ARDUINO_ZERO_ */

