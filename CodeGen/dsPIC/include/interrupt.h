/**
 * INTERRUPT Generated Driver Header File 
 * 
 * @file      interrupt.h
 *            
 * @defgroup  interruptdriver Interrupt Driver
 *            
 * @brief     Interrupt Priority configuration driver using dsPIC MCUs.            
 *
 * @skipline @version   PLIB Version 1.1.4
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

#ifndef INTERRUPT_H
#define INTERRUPT_H

// Section: Includes
#include <stdint.h>
#include <xc.h>

// Section: Driver Interface Function

/**
 * @ingroup  interruptdriver
 * @brief    Initializes the interrupt priorities of the modules that have been configured
 * @param    none
 * @return   none  
 */
void INTERRUPT_Initialize(void);

/**
 * @ingroup  interruptdriver
 * @brief    Deinitializes the INTERRUPT to POR values
 * @param    none
 * @return   none  
 */
void INTERRUPT_Deinitialize(void);

/**
 * @ingroup  interruptdriver
 * @brief    Enables the global interrupt bit
 * @param    none
 * @return   none  
 */
inline static void INTERRUPT_GlobalEnable(void)
{
    __builtin_enable_interrupts();
}

/**
 * @ingroup  interruptdriver
 * @brief    Disables the global interrupt bit
 * @param    none
 * @return   none  
 */
inline static void INTERRUPT_GlobalDisable(void)
{
    __builtin_disable_interrupts();
}

/**
 * @ingroup  interruptdriver
 * @brief    Returns the pending Interrupt Vector number
 * @param    none
 * @return   Returns the pending Interrupt Vector number  
 */
inline static uint16_t INTERRUPT_VectorNumberGet(void)
{
    return _VECNUM; 
}

#endif

