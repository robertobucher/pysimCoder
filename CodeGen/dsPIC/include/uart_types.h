/**
 * UART Generated Driver Types Header File
 * 
 * @file      uart_types.h
 * 
 * @ingroup   uartdriver
 * 
 * @brief     This is the generated driver types header file for the UART driver
 *
 * @skipline @version   PLIB Version 1.1.2
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

#ifndef UART_TYPES_H
#define UART_TYPES_H

/**
 @ingroup  uartdriver
 @enum     UART_ERROR_MASKS
 @brief    This Enum can be used to know UART error type 
           using UARTx_ErrorGet function e.g. \ref UART1_ErrorGet.
*/
enum UART_ERROR_MASKS{
    UART_ERROR_FRAMING_MASK = 0x1,
    UART_ERROR_PARITY_MASK = 0x2,
    UART_ERROR_RX_OVERRUN_MASK = 0x4,
    UART_ERROR_TX_COLLISION_MASK = 0x8,
    UART_ERROR_AUTOBAUD_OVERFLOW_MASK = 0x10,
};

#endif


