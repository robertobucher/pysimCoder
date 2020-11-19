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
/*
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * + Pin number +  MBC Board pin   |  PIN   | Label/Name      | Comments (* is for default peripheral in use)
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | Virtual Digital  |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 0          | 0                |  ESP   |                 | 
 * | 1          | 1                |  ESP   |                 | 
 * | 2          | 2                |  ESP   |                 | 
 * | 3          | 3                |  ESP   |                 | 
 * | 4          | 4                |  ESP   |                 | 
 * | 5          | 5                |  ESP   |                 | 
 * | 6          | 6                |  ESP   |                 | 
 * | 7          | 7                |  ESP   |                 | 
 * | 8          | 8                |  ESP   |                 | 
 * | 9          | 9                |  ESP   |                 | 
 * | 10         | 10               |  ESP   |                 | 
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | Digital High     |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 11         | 11               |  PA23  |                 | EIC/EXTINT[7]                        PTC/X[11]  SERCOM3/PAD[1]                  TC4/WO[1]    *TCC0/WO[5]
 * | 12         | 12               |  PA14  |                 | EIC/EXTINT[14]                                  SERCOM2/PAD[2]                  TC3/WO[0]    *TCC0/WO[4]
 * | 13         | 13               |  PA10  |                 | EIC/EXTINT[10]  ADC/AIN[18]          PTC/X[2] +SERCOM0/PAD[2]  SERCOM2/PAD[2]  TCC1/WO[0]    *TCC0/WO[3]
 * | 14         | 14               |  PA09  |                 | EIC/EXTINT[9]   ADC/AIN[17]          PTC/X[1] +SERCOM0/PAD[1]  SERCOM2/PAD[1]  TCC0/WO[1]    *TCC1/WO[3]
 * | 15         | 15               |  PA08  |                 | NMI             ADC/AIN[16]          PTC/X[0] +SERCOM0/PAD[0]  SERCOM2/PAD[0]   TC0/WO[0]    *TCC1/WO[2]
 * | 16         | 16               |  PA27  |                 | EIC/EXTINT[15]
 * | 17         | 17               |  PB02  |                 | EIC/EXTINT[2]   ADC/AIN[10]          PTC/Y[8]                  SERCOM5/PAD[0]
 * | 18         | 18               |  PB03  |                 | EIC/EXTINT[3]   ADC/AIN[11]          PTC/Y[9]                  SERCOM5/PAD[1]
 * | 19         | 19               |  PB08  |                 | EIC/EXTINT[8]   ADC/AIN[2]           PTC/Y[14]                 SERCOM4/PAD[0]  *TC4/WO[0]
 * | 20         | 20               |  PB09  |                 | EIC/EXTINT[9]   ADC/AIN[3]           PTC/Y[15]                 SERCOM4/PAD[1]  *TC4/WO[1]
 * | 21         | 21               |  PB11  |                 | EIC/EXTINT[11]                                                 SERCOM4/PAD[3]  *TC5/WO[1]    TCC0/WO[5]
 * | 22         | 22               |  PB10  |                 | EIC/EXTINT[10]                                                 SERCOM4/PAD[2]  *TC5/WO[0]    TCC0/WO[4]
 * | 23         | 23               |  PA12  |                 | EIC/EXTINT[12]                                +SERCOM2/PAD[0]  SERCOM4/PAD[0]  TCC2/WO[0]   *TCC0/WO[6]
 * | 24         | 24               |  PA13  |                 | EIC/EXTINT[13]                                +SERCOM2/PAD[1]  SERCOM4/PAD[1]  TCC2/WO[1]   *TCC0/WO[7]
 * | 25         | 25               |  PA11  |                 |                 ADC/AIN[19]          PTC/X[3] +SERCOM0/PAD[3]  SERCOM2/PAD[3]  TCC1/WO[1]   *TCC0/WO[3]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | Analog Connector |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 26         | A0               |  PA02  | A0              | EIC/EXTINT[2] *ADC/AIN[0]  DAC/VOUT  PTC/Y[0]
 * | 27         | A1               |  PA03  | A1              | EIC/EXTINT[3] *ADC/AIN[1]            PTC/Y[1]
 * | 28         | A2               |  PA04  | A2              | EIC/EXTINT[4] *ADC/AIN[4]  AC/AIN[0] PTC/Y[2]                  SERCOM0/PAD[0]  TCC0/WO[0]
 * | 29         | A3               |  PA05  | A3              | EIC/EXTINT[5] *ADC/AIN[5]  AC/AIN[1] PTC/Y[3]                  SERCOM0/PAD[1]  TCC0/WO[1]
 * | 30         | A4               |  PA06  | A4              | EIC/EXTINT[6] *ADC/AIN[6]  AC/AIN[2] PTC/Y[4]                  SERCOM0/PAD[2]  TCC1/WO[0]
 * | 31         | A5               |  PA07  | A5              | EIC/EXTINT[7] *ADC/AIN[16]           PTC/Y[5]                  SERCOM0/PAD[3]  TCC1/WO[1]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | Virtual Digital  |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 32         | 32               |  ESP   |                 | 
 * | 33         | 33               |  ESP   |                 | 
 * | 34         | 34               |  ESP   |                 | 
 * | 35         | 35               |  ESP   |                 | 
 * | 36         | 36               |  ESP   |                 | 
 * | 37         | 37               |  ESP   |                 | 
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | SWD              |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 38         |                  |  PA31  | SWDIO           |
 * | 39         |                  |  PA30  | SWDCLK          |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | Control Signals  |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 40         |                  |  PA28  | ESP_ON          |
 * | 41         |                  |  PA20  | ESP_RST         |
 * | 42         |                  |  PA15  | ESP_BOOT        |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            |SPI (SAMD - ESP)  |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 43         |                  |  PA16  | MOSI            | EIC/EXTINT[0] SERCOM1/PAD[0]
 * | 44         |                  |  PA17  | SCK             | EIC/EXTINT[1] SERCOM1/PAD[1]
 * | 45         |                  |  PA18  | CS              |
 * | 46         |                  |  PA19  | MISO            | EIC/EXTINT[3] SERCOM1/PAD[3]
 * | 47         |                  |  PA21  | SR              | EIC/EXTINT[5]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | USB              |        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 48         |                  |  PA24  | USB_NEGATIVE    | *USB/DM
 * | 49         |                  |  PA25  | USB_POSITIVE    | *USB/DP
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | UART (SAMD - ESP)|        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 50         |                  |  PB22  | UART TX         | *SERCOM5/PAD[2]
 * | 51         |                  |  PB23  | UART RX         | *SERCOM5/PAD[3]
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * |            | Additional Analog|        |                 |
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 * | 52         | A6               |  PB02  |                 |
 * | 53         | A7               |  PB03  |                 |
 * | 54         | A8               |  PB08  |                 |
 * | 55         | A9               |  PB09  |                 |
 * | 56         | A10              |  PA11  |                 |
 * | 57         | A11              |  PA10  |                 |
 * | 58         | A12              |  PA09  |                 |
 * | 59         | A13              |  PA08  |                 |
 * | 60         | A14              |  ESP   |                 |
 * | 61         | A15              |  ESP   |                 |
 * | 62         | A16              |  ESP   |                 |
 * | 63         | A17              |  ESP   |                 |
 * | 64         | A18              |  ESP   |                 | Do not work if WiFi is enabled
 * | 65         | A19              |  ESP   |                 | Do not work if WiFi is enabled
 * | 66         | A20              |  ESP   |                 | Do not work if WiFi is enabled
 * | 67         | A21              |  ESP   |                 | Do not work if WiFi is enabled
 * | 68         | A22              |  ESP   |                 | Do not work if WiFi is enabled
 * | 69         | A23              |  ESP   |                 | Do not work if WiFi is enabled
 * | 70         | A24              |  ESP   |                 | Do not work if WiFi is enabled
 * | 71         | A25              |  ESP   |                 | Do not work if WiFi is enabled
 * | 72         | A26              |  ESP   |                 | Do not work if WiFi is enabled
 * +------------+------------------+--------+-----------------+--------------------------------------------------------------------------------------------------------
 */


#include "variant.h"

/*
 * Pins descriptions
 */

// TODO - fixare external intrrupts

const PinDescription g_APinDescription[]=
{
	// 0..25 - Digital pins
	// ----------------------
	// 0..10 - ESP GPIO
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP GPIO25
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP GPIO27
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP GPIO32
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP GPIO26
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP GPIO33
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP ESP_TXD
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP ESP_RXD_ID
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP GPIO35
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP GPIO34
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP GPIO4
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP GPIO2
	
	// 11..15 - SAMD GPIO
	{ PORTA, 23, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM4_CH1, TCC0_CH5, EXTERNAL_INT_7 },	//
	{ PORTA, 14, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM3_CH0, TCC0_CH4, EXTERNAL_INT_14 },  //
	{ PORTA, 10, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel18, PWM1_CH0, TCC0_CH2, EXTERNAL_INT_10 },	//
	{ PORTA, 9,  PIO_SERCOM, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel17, PWM0_CH1, TCC1_CH3, EXTERNAL_INT_9 },		// EXT_I2C_SCL: SERCOM0/PAD[1]
	{ PORTA, 8,  PIO_SERCOM, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel16, PWM0_CH0, TCC1_CH2, EXTERNAL_INT_NMI },	// EXT_I2C_SDA: SERCOM0/PAD[0]
		
	// 16..25 - SAMD GPIO
	{ PORTA, 27, PIO_OUTPUT, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_15 },
	{ PORTB, 2,  PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG), ADC_Channel10, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_2 },
	{ PORTB, 3,  PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG), ADC_Channel11, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_3 },
	{ PORTB, 8,  PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel2, PWM4_CH0, TC4_CH0, EXTERNAL_INT_8 },
	{ PORTB, 9,  PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel3, PWM4_CH1, TC4_CH1, EXTERNAL_INT_9 },
	{ PORTB, 11, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM5_CH1, TC5_CH1, EXTERNAL_INT_11 },	// EXT_USART_RX: SERCOM4/PAD[3]
	{ PORTB, 10, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM5_CH0, TC5_CH0, EXTERNAL_INT_10 },	// EXT_USART_TX: SERCOM4/PAD[2]
	{ PORTA, 12, PIO_SERCOM, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM2_CH0, TCC0_CH6, EXTERNAL_INT_12 }, 	// MOSI: SERCOM2/PAD[0]
	{ PORTA, 13, PIO_SERCOM, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), No_ADC_Channel, PWM2_CH1, TCC0_CH7, EXTERNAL_INT_13 }, 	// SCK: SERCOM2/PAD[1]
	{ PORTA, 11, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel19, PWM1_CH1, TCC0_CH3, EXTERNAL_INT_NONE },	// MISO: SERCOM2/PAD[3]
	
	// 26..31 - Analog pins
	{ PORTA,  2, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel0, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },	// ADC/AIN[0]
	{ PORTA,  3, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel1, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },	// ADC/AIN[2]
	{ PORTA,  4, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel4, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },	// ADC/AIN[3]
	{ PORTA,  5, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel5, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },	// ADC/AIN[4]
	{ PORTA,  6, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel6, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },	// ADC/AIN[5]
	{ PORTA,  7, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel7, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },	// ADC/AIN[10]
	
	// 32..37 - ESP GPIO
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP MTDO
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP MTCK
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP MTDI
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP MTMS
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP GPIO16
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP GPIO17

	// end Arduino pinout
	
	// 38..39 - SWD
	{ PORTA, 31, PIO_OUTPUT, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // SWDIO
	{ PORTA, 30, PIO_OUTPUT, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },	// SWDCLK

	// 40..42 - ESP Control (Enable, Boot, Power ON/OFF)
	#if defined HW_VER_1_0
		{ PORTA, 22, PIO_OUTPUT, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP_ON
	#elif defined HW_VER_1_1
		{ PORTA, 28, PIO_OUTPUT, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP_ON
	#endif
	{ PORTA, 20, PIO_OUTPUT, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP_RST
	{ PORTA, 15, PIO_OUTPUT, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // ESP_BOOT

	// 43..47 SPI - connected to ESP
	{ PORTA, 16, PIO_SERCOM, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_0 }, 	// MOSI: SERCOM1/PAD[0]
	{ PORTA, 17, PIO_SERCOM, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_1 }, 	// SCK: SERCOM1/PAD[1]
	{ PORTA, 18, PIO_OUTPUT, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },	// CS
	{ PORTA, 19, PIO_SERCOM, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_3 }, 	// MISO: SERCOM1/PAD[3]
	{ PORTA, 21, PIO_INPUT, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_5 },		// SR
	
	// 48..49 - USB
	{ PORTA, 24, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // USB/DM
	{ PORTA, 25, PIO_COM, PIN_ATTR_NONE, No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // USB/DP

	// 50..51 - ESP programming UART
	{ PORTB, 22, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },	// ESP TX: SERCOM5/PAD[2]
	{ PORTB, 23, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },	// ESP RX: SERCOM5/PAD[3]

	// 52..72 - Additional analog pins
	{ PORTB, 2,  PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG), ADC_Channel10, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_2 }, // A6 - SAMD PB2
	{ PORTB, 3,  PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG), ADC_Channel11, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_3 }, // A7 - SAMD PB3
	{ PORTB, 8,  PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel2, PWM4_CH0, TC4_CH0, EXTERNAL_INT_8 }, // A8 - SAMD PB8
	{ PORTB, 9,  PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel3, PWM4_CH1, TC4_CH1, EXTERNAL_INT_9 }, // A9 - SAMD PB9
	{ PORTA, 11, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel19, PWM1_CH1, TCC0_CH3, EXTERNAL_INT_NONE },	    // A10 - SAMD PA11
	{ PORTA, 10, PIO_TIMER, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel18, PWM1_CH0, TCC0_CH2, EXTERNAL_INT_10 }, // A11 - SAMD PA10
	{ PORTA, 9,  PIO_SERCOM, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel17, PWM0_CH1, TCC1_CH3, EXTERNAL_INT_9 }, // A12 - SAMD PA9
	{ PORTA, 8,  PIO_SERCOM, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel16, PWM0_CH0, TCC1_CH2, EXTERNAL_INT_NMI }, // A13 - SAMD PA8
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A14 - ESP GPIO32
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A15 - ESP GPIO33
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A16 - ESP GPIO35
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A17 - ESP GPIO34
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A18 - ESP GPIO25
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A19 - ESP GPIO27
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A20 - ESP GPIO26
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A21 - ESP GPIO4
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A22 - ESP GPIO2
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A23 - ESP GPIO15
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A24 - ESP GPIO13
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A25 - ESP GPIO12
	{ NOT_A_PORT, 0, PIO_EXTERNAL, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // A26 - ESP GPIO14

	// 73 - fake pin
	{ NOT_A_PORT, 10, PIO_NOT_A_PIN, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE },
	#if defined HW_VER_1_1
		{ PORTA, 22, PIO_OUTPUT, (PIN_ATTR_DIGITAL), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_NONE }, // MBC ID
	#endif
};

const void* g_apTCInstances[TCC_INST_NUM + TC_INST_NUM]={ TCC0, TCC1, TCC2, TC3, TC4, TC5 } ;

// Multi-serial objects instantiation
SERCOM sercom0( SERCOM0 ) ;
SERCOM sercom1( SERCOM1 ) ;
SERCOM sercom2( SERCOM2 ) ;
SERCOM sercom3( SERCOM3 ) ;
SERCOM sercom4( SERCOM4 ) ;
SERCOM sercom5( SERCOM5 ) ;

Uart Serial1(&sercom4, PIN_SERIAL1_RX, PIN_SERIAL1_TX, PAD_SERIAL1_RX, PAD_SERIAL1_TX);
Uart SerialCompanion(&sercom5, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);

// serial between SAM and ESP
void SERCOM5_Handler()
{
  SerialCompanion.IrqHandler();
}

// external serial.
void SERCOM4_Handler()
{
  Serial1.IrqHandler();
}