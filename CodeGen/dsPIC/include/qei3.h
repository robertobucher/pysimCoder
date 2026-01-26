/**
 * QEI3 Generated Driver Header File 
 * 
 * @file      qei3.h
 *            
 * @ingroup   qeidriver
 *            
 * @brief     This is the generated driver header file for the QEI3 driver
 *
 * @skipline @version   PLIB Version 1.0.5
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

#ifndef QEI3_H
#define QEI3_H

// Section: Included Files

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <qei_types.h>
#include <qei_interface.h>

// Section: Data Type Definitions

/**
 * @ingroup  qeidriver
 * @brief    Structure object of type QEI_INTERFACE with the custom name
 *           given by the user in the Melody Driver User interface. The default name 
 *           e.g. QEI1 can be changed by the user in the QEI user interface. 
 *           This allows defining a structure with application specific name using 
 *           the 'Custom Name' field. Application specific name allows the API Portability.
*/
extern const struct QEI_INTERFACE QEI3;
// Section: QEI3 Module APIs

/**
 * @ingroup  qeidriver
 * @brief    This inline function disables the QEI3 module
 * @param    none
 * @return   none  
 */
inline static void QEI3_Disable(void)
{
    QEI3CONbits.ON = 0;        
}
    
/**
 * @ingroup  qeidriver
 * @brief    This inline function enables the QEI3 module
 * @param    none
 * @return   none
 */
inline static void  QEI3_Enable(void)
{
    QEI3CONbits.ON = 1;        
}

/**
 * @ingroup  qeidriver
 * @brief    This inline function returns the 32-bit position count value 
 * @param    none
 * @return   Returns the 32-bit position count register value
 */
inline static uint32_t QEI3_PositionCountRead(void)
{
    return POS3CNT;
}

/**
 * @ingroup  qeidriver
 * @brief    This inline function returns the 16-bit position count value from the 
 *           QEI3 position count register.
 * @param    none
 * @return   Returns the LSB 16 bits of the QEI3 position 
 *           count register.
 */
inline static uint16_t QEI3_PositionCount16bitRead(void)
{
    return (uint16_t)POS3CNT;
}

/**
 * @ingroup    qeidriver
 * @brief      This inline function sets the QEI3 position count value
 * @param[in]  positionCount - 32-bit position count value 
 * @return     none  
 */
inline static void QEI3_PositionCountWrite(uint32_t positionCount)
{
    POS3CNT = positionCount; 
}

/**
 * @ingroup    qeidriver
 * @brief      This inline function sets the 32bit modulo count value when QEI is configured to
 *             operate in 'Modulo Count' mode. The lower bound controlled by the QEILEC register is set to 0.
 * @param[in]  countsPerRevolution - Modulus number of counts per wraparound  
 * @return     none  
 */
inline static void QEI3_ModuloRangeSet(uint32_t countsPerRevolution)
{
    uint32_t maxCount = countsPerRevolution - (uint32_t)1;
    QEI3LEC = 0;
    QEI3GEC = maxCount;
}

/**
 * @ingroup  qeidriver
 * @brief    This inline function gets the status of QEI input phase swap configuration  
 * @param    none
 * @return   true   - Phase inputs are swapped
 * @return   false  - Phase inputs are not swapped
 */
inline static bool QEI3_PhaseInputSwappedGet(void)
{
    return QEI3IOCbits.SWPAB;
}

/**
 * @ingroup    qeidriver
 * @brief      This inline function sets whether the QEA and QEB pins are 
 *             swapped prior to quadrature decoder logic.
 * @param[in]  swapEnabled - specifies whether QEA and QEB pins need 
 *             to be swapped prior to quadrature decoder logic  
 * @return     none   
 */
inline static void QEI3_PhaseInputSwappedSet(bool swapEnabled)
{

    if (swapEnabled)
    {
        QEI3IOCbits.SWPAB = 1;
    }
    else
    {
        QEI3IOCbits.SWPAB = 0;   
    }  
}

/**
 * @ingroup  qeidriver
 * @brief    This inline function enables the QEI position count capture on an index event
 * @param    none
 * @return   none  
 */
inline static void QEI3_PositionCaptureEnable(void)
{
    QEI3IOCbits.QCAPEN = 1;
}

/**
 * @ingroup  qeidriver
 * @brief    This inline function disables the QEI position count capture on an index event
 * @param    none
 * @return   none  
 */
inline static void QEI3_PositionCaptureDisable(void)
{
    QEI3IOCbits.QCAPEN = 0;
}

/**
 * @ingroup  qeidriver
 * @brief    This inline function reads the 32-bit position capture value from the
 *           QEIICx 32 bit register.
 * @param    none
 * @return   Returns position capture value
 */
inline static uint32_t QEI3_PositionCaptureGet(void)
{
    return QEI3GEC;
}

/**
 * @ingroup  qeidriver
 * @brief    This inline function returns the 16-bit position capture value from 
 *           QEIGEC position count capture register
 * @param    none
 * @return   Returns the LSB 16 bits of position capture value
 */
inline static uint16_t QEI3_PositionCapture16bitGet(void)
{
    return (uint16_t)QEI3GEC;
}

/**
 * @ingroup    qeidriver
 * @brief      This inline function initializes the 32 bit position capture register value
 * @param[in]  initValue - 32 bit position capture register value  
 * @return     none  
 */
inline static void QEI3_PositionCaptureSet(uint32_t initValue)
{
    QEI3GEC = initValue;
}

/**
 * @ingroup    qeidriver
 * @brief      This inline function sets the QEI Counter mode
 * @param[in]  mode - Sets the QEI counter mode 
 * @return     none  
 */
inline static void QEI3_CounterModeSet(enum QEI_MODE mode)
{
    QEI3CONbits.PIMOD = mode;
}

/**
 * @ingroup    qeidriver
 * @brief      This inline function sets the QEI Index Match Value 
 * @param[in]  mode - Sets the QEI Index Match Value
 * @return     none  
 */
inline static void QEI3_IMVGatedValueSet(enum QEI_IMV_STATE state)
{
    QEI3CONbits.IMV = state;
}

/**
 * @ingroup  qeidriver
 * @brief    Initializes the QEI module
 * @param    none
 * @return   none 
 */
void QEI3_Initialize(void);

/**
 * @ingroup  qeidriver
 * @brief    Deinitializes the QEI3 to POR values
 * @param    none
 * @return   none  
 */
void QEI3_Deinitialize(void);

#endif // QEI3.H
