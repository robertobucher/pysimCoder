/**
 * PWM Generated Driver Interface Header File
 * 
 * @file 	  pwm_interface.h
 * 
 * @defgroup  pwmdriver PWM Driver
 * 
 * @brief	  Pulse Width Modulation Driver using dsPIC MCUs
 *
 * @skipline @version   PLIB Version 1.2.1
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

#ifndef PWM_INTERFACE_H
#define PWM_INTERFACE_H

// Section: Included Files

#include <stddef.h>
#include <pwm_types.h>

// Section: Data Type Definitions
        
/**
 @ingroup  pwmdriver
 @struct   PWM_INTERFACE
 @brief    Structure containing the function pointers of PWM generator driver
*/

struct PWM_INTERFACE{
    void (*Initialize)(void);	
    ///< Pointer to SCCPx_PWM_Initialize e.g. \ref SCCP1_PWM_Initialize
    
    void (*Deinitialize)(void);
    ///< Pointer to SCCPx_PWM_Deinitialize e.g. \ref SCCP1_PWM_Deinitialize
    
    void (*Enable)(void);
    ///< Pointer to SCCPx_PWM_Enable e.g. \ref SCCP1_PWM_Enable
    
    void (*Disable)(void);
    ///< Pointer to SCCPx_PWM_Disable e.g. \ref SCCP1_PWM_Disable
    
    void (*PeriodSet)(size_t periodCount);
    ///< Pointer to SCCPx_PWM_PeriodSet e.g. \ref SCCP1_PWM_PeriodSet
    
    void (*DutyCycleSet)(size_t dutyCycleCount);
    ///< Pointer to SCCPx_PWM_DutyCycleSet e.g. \ref SCCP1_PWM_DutyCycleSet
    
    void (*DeadTimeSet)(size_t deadTimeCount);
    ///< Pointer to SCCPx_PWM_DeadTimeSet e.g. \ref SCCP1_PWM_DeadTimeSet (This feature is hardware dependent)
    
    void (*OutputModeSet)(enum PWM_OUTPUT_MODES outputMode);
    ///< Pointer to SCCPx_PWM_OutputModeSet e.g. \ref SCCP1_PWM_OutputModeSet (This feature is hardware dependent)
    
    void (*SoftwareTriggerSet)(void);
    ///< Pointer to SCCPx_PWM_SoftwareTriggerSet e.g. \ref SCCP1_PWM_SoftwareTriggerSet
    
    void (*CallbackRegister)(void (*handler)(void));
    ///< Pointer to SCCPx_PWM_CallbackRegister e.g. \ref SCCP1_PWM_CallbackRegister
    
    void (*Tasks)(void);
    ///< Pointer to SCCPx_PWM_Tasks e.g. \ref SCCP1_PWM_Tasks (Supported only in polling mode)
};

#endif  //PWM_INTERFACE_H
/**
 End of File
*/
