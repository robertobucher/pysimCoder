/**
 * MAIN Generated Driver Header File
 * 
 * @file      system.h
 * 
 * @defgroup  systemdriver System Driver
 * 
 * @brief     System driver using dsPIC MCUs.
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

#include <xc.h>
#include <stdint.h>
#include <system_types.h>

#ifndef SYSTEM_H
#define	SYSTEM_H

/**
 * @ingroup systemdriver
 * @brief   Initializes the System module
 * @return  none 
 */
void SYSTEM_Initialize(void);

/**
 * @ingroup systemdriver
 * @brief   Initializes the CPU core control register
 * @return  none 
 */
inline static void SYSTEM_CORCONInitialize(void)
{
    CORCON = (CORCON & 0x00F2) | CORCON_MODE_PORVALUES;
}

/**
 * @ingroup   systemdriver
 * @brief     Sets the CPU core control register (CORCON) operating mode 
 *            according to the SYSTEM_CORCON_MODES argument.
 * @param[in] modeValue - SYSTEM_CORCON_MODES initialization mode specifier
 * @return    none
 */
inline static void SYSTEM_CORCONModeOperatingSet(SYSTEM_CORCON_MODES modeValue)
{
    CORCON = (CORCON & 0x00F2) | modeValue;
}

/**
 * @ingroup    systemdriver
 * @brief      This inline function sets the value of CPU core control register
 * @param[in]  value - value that needs to be written to the CPU core control register
 * @return     none 
 */
    inline static void SYSTEM_CORCONRegisterValueSet(uint32_t value)
    {
        CORCON = value;
    }

/**
 * @ingroup systemdriver
 * @brief   This inline function gets the value of CPU core control register
 * @return  value of the CPU core control register 
 */
inline static uint32_t SYSTEM_CORCONRegisterValueGet(void)
{    
    return CORCON;
}

/**
 * @ingroup systemdriver
 * @brief   This inline function gets the base address of the DEVID register 
 *          for the currently selected device
 * @return  base address of the DEVID register 
 */
inline static uint32_t SYSTEM_DeviceIdRegisterAddressGet(void)
{
    return __DEVID_BASE;
}

#endif	/* SYSTEM_H */
/**
 End of File
*/