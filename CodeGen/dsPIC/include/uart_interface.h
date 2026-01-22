/**
 * UART  Generated Driver Interface Header File
 * 
 * @file      uart_interface.h
 *            
 * @defgroup  uartdriver UART Driver
 *            
 * @brief     Universal Asynchronous Receiver Transmitter driver using dsPIC MCUs
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

#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

// Section: Included Files

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <uart_types.h>

// Section: Data Type Definitions  
      
/**
 @ingroup  uartdriver
 @struct   UART_INTERFACE
 @brief    Structure containing the function pointers of UART driver
*/
struct UART_INTERFACE{
    void (*Initialize)(void);
    ///< Pointer to UARTx_Initialize e.g. \ref UART1_Initialize
    
    void (*Deinitialize)(void);
    ///< Pointer to UARTx_Deinitialize e.g. \ref UART1_Deinitialize
    
    uint8_t (*Read)(void);
    ///< Pointer to UARTx_Read e.g. \ref UART1_Read
    
    void (*Write)(uint8_t);
    ///< Pointer to UARTx_Write e.g. \ref UART1_Write
    
    bool (*IsRxReady)(void);
    ///< Pointer to UARTx_IsRxReady e.g. \ref UART1_IsRxReady
    
    bool (*IsTxReady)(void);
    ///< Pointer to UARTx_IsTxReady e.g. \ref UART1_IsTxReady
    
    bool (*IsTxDone)(void);
    ///< Pointer to UARTx_IsTxDone e.g. \ref UART1_IsTxDone
    
    void (*TransmitEnable)(void);
    ///< Pointer to UARTx_TransmitEnable e.g. \ref UART1_TransmitEnable
    
    void (*TransmitDisable)(void);
    ///< Pointer to UARTx_TransmitDisable e.g. \ref UART1_TransmitDisable
    
    void (*TransmitInterruptEnable)(void);
    ///< Pointer to UARTx_TransmitInterruptEnable e.g. \ref UART1_TransmitInterruptEnable
    
    void (*TransmitInterruptDisable)(void);
    ///< Pointer to UARTx_TransmitInterruptDisable e.g. \ref UART1_TransmitInterruptDisable

    void (*AutoBaudSet)(bool enable);
    ///< Pointer to UARTx_AutoBaudSet e.g. \ref UART1_AutoBaudSet
    
    bool (*AutoBaudQuery)(void);
    ///< Pointer to UARTx_AutoBaudQuery e.g. \ref UART1_AutoBaudQuery
    
    bool (*AutoBaudEventEnableGet)(void);
    ///< Pointer to UARTx_AutoBaudEventEnableGet e.g. \ref UART1_AutoBaudEventEnableGet
    
    void (*BRGCountSet)(uint32_t brgValue);
    ///< Pointer to UARTx_BRGCountSet e.g. \ref UART1_BRGCountSet
    
    uint32_t (*BRGCountGet)(void);
    ///< Pointer to UARTx_BRGCOuntGet e.g. \ref UART1_BRGCountGet
    
    void (*BaudRateSet)(uint32_t baudRate);
    ///< Pointer to UARTx_BaudRateSet e.g. \ref UART1_BaudRateSet
    
    uint32_t (*BaudRateGet)(void);
    ///< Pointer to UARTx_BaudRateGet e.g. \ref UART1_BaudRateGet
    
    size_t (*ErrorGet)(void);
    ///< Pointer to UARTx_ErrorGet e.g. \ref UART1_ErrorGet
    
    void (*RxCompleteCallbackRegister)(void (*CallbackHandler)(void));
    ///< Pointer to UARTx_RxCompleteCallbackRegister e.g. \ref UART1_RxCompleteCallbackRegister (defined only in interrupt mode)
    void (*TxCompleteCallbackRegister)(void (*CallbackHandler)(void));
    ///< Pointer to UARTx_TxCompleteCallbackRegister e.g. \ref UART1_TxCompleteCallbackRegister (defined only in interrupt mode)
    void (*TxCollisionCallbackRegister)(void (*CallbackHandler)(void));
    ///< Pointer to UARTx_TxCollisionCallbackRegister e.g. \ref UART1_TxCollisionCallbackRegister (defined only in interrupt mode)
    void (*FramingErrorCallbackRegister)(void (*CallbackHandler)(void));
    ///< Pointer to UARTx_FramingErrorCallbackRegister e.g. \ref UART1_FramingErrorCallbackRegister (defined only in interrupt mode)
    void (*OverrunErrorCallbackRegister)(void (*CallbackHandler)(void));
    ///< Pointer to UARTx_OverrunErrorCallbackRegister e.g. \ref UART1_OverrunErrorCallbackRegister (defined only in interrupt mode)
    void (*ParityErrorCallbackRegister)(void (*CallbackHandler)(void));
    ///< Pointer to UARTx_ParityErrorCallbackRegister e.g. \ref UART1_ParityErrorCallbackRegister (defined only in interrupt mode)
};
#endif

