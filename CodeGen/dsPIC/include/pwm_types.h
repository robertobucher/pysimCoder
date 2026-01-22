/**
 * PWM Generated Driver Types Header File
 * 
 * @file 	  pwm_types.h
 * 
 * @ingroup   pwmdriver
 * 
 * @brief 	  This is the generated driver types header file for the PWM driver
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

#ifndef PWM_TYPES_H
#define PWM_TYPES_H

// Section: Included Files

// Section: Enum Declarations

/**
 @ingroup  pwmdriver
 @enum 	   PWM_OUTPUT_MODES
 @brief	   Enumeration containing the output modes of PWM driver
 @note     The PWM output modes may not be available for all devices, please refer
           specific device datasheet for availability.
*/
enum PWM_OUTPUT_MODES
{
    OUTPUT_SCAN_MODE = 0x6,         /**<Output Scan Mode*/
    BRUSH_DC_OUTPUT_FORWARD = 0x5,  /**<Brush DC Output Forward Mode*/
    BRUSH_DC_OUTPUT_REVERSE = 0x4,  /**<Brush DC Output Reverse Mode*/
    HALF_BRIDGE_OUTPUT = 0x2,       /**<Half Bridge Output Mode*/
    PUSH_PULL_OUTPUT = 0x1,         /**<Push Pull Output Mode*/
    STEERABLE_SINGLE_OUTPUT = 0x0,  /**<Steerable Single Output Mode*/
};

#endif  //PWM_TYPES_H
/**
 End of File
*/
