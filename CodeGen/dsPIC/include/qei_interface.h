/**
 * QEI Generated Driver Interface Header File
 * 
 * @file      qei_interface.h
 *            
 * @defgroup  qeidriver QEI Driver
 *            
 * @brief     The Quadrature Encoder Interface (QEI) driver provides the interface 
 *            to incremental encoders for obtaining mechanical position data and Also 
 *            Detects position and speed of rotating motion systems using dsPIC MCUs.
 *            
 * @skipline @version   Firmware Driver Version 1.2.3
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

#ifndef QEI_INTERFACE_H
#define QEI_INTERFACE_H

// Section: Included Files

#include <stdbool.h>
#include <stdint.h>
#include <qei_types.h>

// Section: Data Type Definitions
        
/**
 @ingroup  qeidriver
 @struct   QEI_INTERFACE
 @brief    Structure containing the function pointers of QEI driver
*/
 
struct QEI_INTERFACE
{
    void (*Initialize)(void);
    ///< Pointer to QEIx_Initialize e.g. \ref QEI1_Initialize
    
    void (*Deinitialize)(void);
    ///< Pointer to QEIx_Deinitialize e.g. \ref QEI1_Deinitialize
    
    void (*Enable)(void);
    ///< Pointer to QEIx_Enable e.g. \ref QEI1_Enable
    
    void (*Disable)(void);
    ///< Pointer to QEIx_Disable e.g. \ref QEI1_Disable
    
    uint32_t (*PositionCountRead)(void);
    ///< Pointer to QEIx_PositionCountRead e.g. \ref QEI1_PositionCountRead
    
    uint16_t (*PositionCount16bitRead)(void);
    ///< Pointer to QEIx_PositionCount16bitRead e.g. \ref QEI1_PositionCount16bitRead
    
    void (*PositionCountWrite)(uint32_t positionCount);
    ///< Pointer to QEIx_PositionCountWrite e.g. \ref QEI1_PositionCountWrite
    
    void (*ModuloRangeSet)(uint32_t countsPerRevolution);
    ///< Pointer to QEIx_ModuloRangeSet e.g. \ref QEI1_ModuloRangeSet
    
    bool (*PhaseInputSwappedGet)(void);
    ///< Pointer to QEIx_PhaseInputSwappedGet e.g. \ref QEI1_PhaseInputSwappedGet
    
    void (*PhaseInputSwappedSet)(bool swapEnabled);
    ///< Pointer to QEIx_PhaseInputSwappedSet e.g. \ref QEI1_PhaseInputSwappedSet
    
    void (*PositionCaptureEnable)(void);
    ///< Pointer to QEIx_PositionCaptureEnable e.g. \ref QEI1_PositionCaptureEnable
    
    void (*PositionCaptureDisable)(void);
    ///< Pointer to QEIx_PositionCaptureDisable e.g. \ref QEI1_PositionCaptureDisable
    
    uint32_t (*PositionCaptureGet)(void);
    ///< Pointer to QEIx_PositionCaptureGet e.g. \ref QEI1_PositionCaptureGet
    
    uint16_t (*PositionCapture16bitGet)(void);
    ///< Pointer to QEIx_PositionCapture16bitGet e.g. \ref QEI1_PositionCapture16bitGet
    
    void (*PositionCaptureSet)(uint32_t initValue);
    ///< Pointer to QEIx_PositionCaptureSet e.g. \ref QEI1_PositionCaptureSet
    
    void (*CounterModeSet)(enum QEI_MODE mode);
    ///< Pointer to QEIx_CounterModeSet e.g. \ref QEI1_CounterModeSet

    void (*IMVGatedValueSet)(enum QEI_IMV_STATE state);
    ///< Pointer to QEIx_IMVGatedValueSet e.g. \ref QEI1_IMVGatedValueSet
};

#endif
