/**
 * CLOCK Generated Driver Header File 
 * 
 * @file      clock.h
 *            
 * @defgroup  clockdriver Clock Driver
 *            
 * @brief     Clock configurator driver for System and Peripheral Clock using dsPIC MCUs.
 *
 * @skipline @version   PLIB Version 1.3.1
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

#ifndef CLOCK_H
#define CLOCK_H

// Section: Included Files

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <clock_types.h>

/** 
  @ingroup  clockdriver
  @brief    This macro is used to read the System clock (FOSC) Frequency configured in 
            the MCC Melody User Interface
*/
#define CLOCK_SystemFrequencyGet()        (100024420UL)

/** 
  @ingroup  clockdriver
  @brief    This macro is used to read the Instruction Clock (FCY) Frequency
            configured in the MCC Melody User Interface
*/
#define CLOCK_InstructionFrequencyGet()   ((uint32_t)CLOCK_SystemFrequencyGet())

/** 
  @ingroup  clockdriver
  @brief    This macro is used to read the Fast Peripheral Clock (1:1) Frequency 
            configured in the MCC Melody User Interface     
*/
#define CLOCK_FastPeripheralFrequencyGet()    ((uint32_t)CLOCK_SystemFrequencyGet())

/** 
  @ingroup  clockdriver
  @brief    This macro is used to read the Standard Peripheral Clock (1:2) Frequency 
            configured in the MCC Melody User Interface     
*/
#define CLOCK_StandardPeripheralFrequencyGet()    ((uint32_t)CLOCK_SystemFrequencyGet() / (uint32_t)2)

/** 
  @ingroup  clockdriver
  @brief    This macro is used to read the Slow Peripheral Clock (1:4) Frequency 
            configured in the MCC Melody User Interface     
*/
#define CLOCK_SlowPeripheralFrequencyGet()    ((uint32_t)CLOCK_SystemFrequencyGet() / (uint32_t)4)

// Section: CLOCK APIs

/**
 * @ingroup  clockdriver
 * @brief    Initializes all the CLOCK sources and clock switch configurations. 
 * @pre      none
 * @param    none
 * @return   none  
 */
void CLOCK_Initialize(void);


/**
 * @ingroup     clockdriver
 * @brief       This inline function returns clock generator frequency in Hz
 * @pre         none
 * @param[in]   generator - instance of clock generator
 * @return      Clock frequency in Hz
 */
inline static uint32_t CLOCK_GeneratorFrequencyGet(enum CLOCK_GENERATOR generator)
{
    uint32_t genFrequency = 0x0U;

    switch(generator)
    {
        case CLOCK_GENERATOR_1:
                genFrequency = 100024420U;
                break;
        case CLOCK_GENERATOR_2:
                genFrequency = 8000000U;
                break;
        case CLOCK_GENERATOR_3:
                genFrequency = 8000000U;
                break;
        case CLOCK_GENERATOR_6:
                genFrequency = 100024420U;
                break;
        default:
                break;
    }
    return genFrequency;
}

/**
 * @ingroup     clockdriver
 * @brief       This inline function returns clock generator failure status.
 * @pre         none
 * @param       none
 * @return      32-bit status value. Use status masks in \ref CLOCK_FAIL_STATUS_MASKS to derive individual status.
 * @note        In interrupt mode this function has to be called inside \ref CLOCK_CombinedClockFailCallback 
 *              to know the status during failure event.
 */
inline static uint32_t CLOCK_FailStatusGet(void)
{
    uint32_t failStatus = (uint32_t)CLKFAIL;
    CLKFAIL =  0x0U;
    return failStatus;   
}

/**
 * @ingroup    clockdriver
 * @brief      This function can be used to override default callback and to 
 *             define custom callback for CLOCK CombinedClockFail event
 * @param[in]  handler - Address of the callback function.  
 * @return     none  
 */
void CLOCK_CombinedClockFailCallbackRegister(void (*handler)(void));

/**
 * @ingroup  clockdriver
 * @brief    This is the default callback with weak attribute. 
 *           The user can override and implement the default callback without 
 *           weak attribute or can register a custom callback function using  
 *           CLOCK_CombinedClockFailCallbackRegister.
 * @param    none
 * @return   none  
 */
void CLOCK_CombinedClockFailCallback(void);

#endif    /* CLOCK_H */



