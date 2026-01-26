/**
 * MAIN Generated Driver Header File
 * 
 * @file      system.c
 *            
 * @ingroup   systemdriver
 *            
 * @brief     This is the generated driver header file for the System driver
 *            
 * @skipline  Device : dsPIC33AK512MPS512
*/
/*
© [2026] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include <system.h>
#include <system_types.h>
#include <clock.h>
#include <pins.h>
#include <adc5.h>
#include <qei1.h>
#include <qei2.h>
#include <qei3.h>
#include <sccp1.h>
#include <sccp2.h>
#include <sccp3.h>
#include <uart1.h>
#include <interrupt.h>


void SYSTEM_Initialize(void)
{
    CLOCK_Initialize();
    PINS_Initialize();
    ADC5_Initialize();
    QEI1_Initialize();
    QEI2_Initialize();
    QEI3_Initialize();
    SCCP1_PWM_Initialize();
    SCCP2_PWM_Initialize();
    SCCP3_PWM_Initialize();
    UART1_Initialize();
    INTERRUPT_GlobalEnable();
    INTERRUPT_Initialize();
}

/**
 End of File
*/