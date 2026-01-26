/**
 * SCCP3 Generated Driver Header File 
 * 
 * @file 	  sccp3.h
 * 
 * @ingroup   pwmdriver
 * 
 * @brief 	  This is the generated driver header file for the SCCP3 driver
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

#ifndef SCCP3_H
#define SCCP3_H

// Section: Included Files

#include <stddef.h>
#include <pwm_interface.h>

// Section: Data Type Definitions


/**
 * @ingroup  pwmdriver
 * @brief    Structure object of type PWM_INTERFACE with the custom name given by 
 *           the user in the Melody Driver User interface. The default name e.g. 
 *           PWM1 can be changed by the user in the MCCP user interface. 
 *           This allows defining a structure with application specific name using 
 *           the 'Custom Name' field. Application specific name allows the API Portability.
*/

extern const struct PWM_INTERFACE PWM3;

/**
 * @ingroup  pwmdriver
 * @brief    This macro defines the Custom Name for \ref SCCP3_PWM_Initialize API
 */
#define PWM3_Initialize SCCP3_PWM_Initialize
/**
 * @ingroup  pwmdriver
 * @brief    This macro defines the Custom Name for \ref SCCP3_PWM_Deinitialize API
 */
#define PWM3_Deinitialize SCCP3_PWM_Deinitialize
/**
 * @ingroup  pwmdriver
 * @brief    This macro defines the Custom Name for \ref SCCP3_PWM_Enable API
 */
#define PWM3_Enable SCCP3_PWM_Enable
/**
 * @ingroup  pwmdriver
 * @brief    This macro defines the Custom Name for \ref SCCP3_PWM_Disable API
 */
#define PWM3_Disable SCCP3_PWM_Disable
/**
 * @ingroup  pwmdriver
 * @brief    This macro defines the Custom Name for \ref SCCP3_PWM_PeriodSet API
 */
#define PWM3_PeriodSet SCCP3_PWM_PeriodSet
/**
 * @ingroup  pwmdriver
 * @brief    This macro defines the Custom Name for \ref SCCP3_PWM_DutyCycleSet API
 */
#define PWM3_DutyCycleSet SCCP3_PWM_DutyCycleSet
/**
 * @ingroup  pwmdriver
 * @brief    This macro defines the Custom Name for \ref SCCP3_PWM_SoftwareTriggerSet API
 */
#define PWM3_SoftwareTriggerSet SCCP3_PWM_SoftwareTriggerSet
/**
 * @ingroup  pwmdriver
 * @brief    This macro defines the Custom Name for \ref SCCP3_PWM_Tasks API
 */
#define PWM3_Tasks SCCP3_PWM_Tasks
/**
 * @ingroup  pwmdriver
 * @brief    This macro defines the Custom Name for \ref SCCP3_PWM_CallbackRegister API
 */
#define PWM3_PWM_CallbackRegister SCCP3_PWM_CallbackRegister

// Section: SCCP3 Module APIs

/**
 * @ingroup  pwmdriver
 * @brief    Initializes the SCCP3 Pulse driver.
 * 			 This function must be called before any other SCCP3 function is called.
 * @param    none
 * @return   none  
 */
void SCCP3_PWM_Initialize( void );

/**
 * @ingroup  pwmdriver
 * @brief 	 Deinitializes the SCCP3 to POR values
 * @param    none
 * @return   none  
 */
void SCCP3_PWM_Deinitialize(void);

/**
 * @ingroup  pwmdriver
 * @brief	 Enables the SCCP3 module
 * @param    none
 * @return   none  
 */
void SCCP3_PWM_Enable( void );

/**
 * @ingroup  pwmdriver
 * @brief 	 Disables the SCCP3 module
 * @param    none
 * @return   none  
 */
void SCCP3_PWM_Disable( void );

/**
 * @ingroup    pwmdriver
 * @brief 	   Sets the cycle width
 * @param[in]  periodCount - number of clock counts for PWM Period
 * @return     none  
 */
void SCCP3_PWM_PeriodSet(size_t periodCount);

/**
 * @ingroup    pwmdriver
 * @brief  	   Sets the ON pulse width
 * @param[in]  dutyCycleCount - number of cycles of ON time  
 * @return     none  
 */
void SCCP3_PWM_DutyCycleSet(size_t dutyCycleCount);

/**
 * @ingroup  pwmdriver
 * @brief 	 This function sets the manual trigger
 * @param    none
 * @return   none      
 */
void SCCP3_PWM_SoftwareTriggerSet( void );

/**
 * @ingroup    pwmdriver
 * @brief      This function can be used to override default callback and to define 
 *             custom callback for SCCP3 PWM event.
 * @param[in]  handler - Address of the callback function
 * @return     none  
 */
void SCCP3_PWM_CallbackRegister(void (*handler)(void));

/**
 * @ingroup  pwmdriver
 * @brief    This is the default callback with weak attribute. The user can 
 *           override and implement the default callback without weak attribute 
 *           or can register a custom callback function using  SCCP3_PWM_CallbackRegister.
 * @param    none
 * @return   none 
 */
void SCCP3_PWM_Callback(void);

/**
 * @ingroup  pwmdriver
 * @brief    This function is used to implement the tasks for polled implementations
 * @param    none
 * @return   none  
 */
void SCCP3_PWM_Tasks( void );

#endif //SCCP3_H
    
/**
 End of File
*/

