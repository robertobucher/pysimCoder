/**
 * INTERRUPT Generated Driver Header File 
 * 
 * @file      interrupt_types.h
 *            
 * @defgroup  interruptdriver
 *            
 * @brief     This is the generated driver types header file for the INTERRUPT driver           
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

#ifndef INTERRUPT_TYPES_H
#define INTERRUPT_TYPES_H

/**
 * @ingroup  interruptdriver
 * @enum     INTERRUPT_PRIORITY
 * @brief    Defines the interrupt priority values.
*/

enum INTERRUPT_PRIORITY
{
	INTERRUPT_PRIORITY_1 = 1,
	INTERRUPT_PRIORITY_2 = 2,
	INTERRUPT_PRIORITY_3 = 3,
	INTERRUPT_PRIORITY_4 = 4,
	INTERRUPT_PRIORITY_5 = 5,
	INTERRUPT_PRIORITY_6 = 6,
	INTERRUPT_PRIORITY_7 = 7,
	INTERRUPT_MAX_PRIORITY = 7
};

#endif

