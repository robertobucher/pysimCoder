/**
 * QEI Generated Driver Types Header File
 * 
 * @file      qei_types.h
 *            
 * @ingroup   qeidriver
 *            
 * @brief     This is the generated driver types header file for the QEI driver 
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

#ifndef QEI_TYPES_H
#define QEI_TYPES_H

// Section: Included Files


// Section: Enum Declarations

/**
 @ingroup  qeidriver
 @enum     QEI_MODE
 @brief    Defines the QEI operating mode
*/

enum QEI_MODE
{ 
    QEI_MODE_FREE_RUNNING = 0,   /**< Index input event does not affect position counter */
    QEI_MODE_RESET_ON_INDEX = 1, /**< Every index input event resets the position counter */
    QEI_MODE_MODULO_COUNT = 6    /**< Modulo Count mode for position counter */
};

/**
 @ingroup  qeidriver
 @enum     QEI_IMV_STATE
 @brief    Defines the QEI Index Match Value states
*/
enum QEI_IMV_STATE
{
    QEI_IMV_STATE_A0B0 = 0, /**< Index Match Value State : QEA = 0 and QEB = 0 */
    QEI_IMV_STATE_A1B0 = 1, /**< Index Match Value State : QEA = 1 and QEB = 0 */
    QEI_IMV_STATE_A0B1 = 2, /**< Index Match Value State : QEA = 0 and QEB = 1 */
    QEI_IMV_STATE_A1B1 = 3, /**< Index Match Value State : QEA = 1 and QEB = 1 */
    QEI_MAX_IMV_STATE       /**< Maximum configurable Index Match Value State  */
};

#endif    /* QEI_TYPES_H */             
