/**
 * MAIN Generated Driver Types Header File
 * 
 * @file      system_types.h
 *            
 * @ingroup   systemdriver
 *            
 * @brief     This is the generated driver types header file for the System driver
 *            
 * @version   Driver Version 1.0.1
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

#ifndef SYSTEM_TYPES_H
#define	SYSTEM_TYPES_H

/**
  Section: Type defines
 */ 
    
/**
 @ingroup  systemdriver
 @enum     CORCON_MODE_TYPE
 @brief    CORCON initialization type enumerator
*/
typedef enum CORCON_MODE_TYPE
{

    CORCON_MODE_PORVALUES                  = 0x0020,  /**< Use POR values of CORCON */
    CORCON_MODE_DISABLEALLSAT_ROUNDBIASED  = 0x0022,  /**< Disable saturation for ACCA, ACCB
                                                          and Dataspace write and set
                                                          rounding to Biased (conventional)
                                                          mode. Rest of CORCON settings are
                                                          set to the default POR values. */
    CORCON_MODE_DISABLEALLSAT_ROUNDUNBIASED = 0x0020, /**< Disable saturation for ACCA, ACCB
                                                           and Dataspace write and set
                                                           rounding to Unbiased (convergent)
                                                           mode. Rest of CORCON settings are
                                                           set to the default POR values. */
    
    CORCON_MODE_ENABLEALLSATNORMAL_ROUNDBIASED  = 0x00E2, /**< Enable saturation for ACCA, ACCB
                                                           and Dataspace write, enable normal
                                                           ACCA/ACCB saturation mode and set
                                                           rounding to Biased (conventional)
                                                           mode. Rest of CORCON settings are
                                                           set to the default POR values. */
    CORCON_MODE_ENABLEALLSATNORMAL_ROUNDUNBIASED = 0x00E0, /**< Enable saturation for ACCA, ACCB
                                                            and Dataspace write, enable normal
                                                            ACCA/ACCB saturation mode and set
                                                            rounding to Unbiased (convergent)
                                                            mode. Rest of CORCON settings are
                                                            set to the default POR values. */
    CORCON_MODE_ENABLEALLSATSUPER_ROUNDBIASED    = 0x00F2, /**< Enable saturation for ACCA, ACCB
                                                            and Dataspace write, enable super
                                                            ACCA/ACCB saturation mode and set
                                                            rounding to Biased (conventional)
                                                            mode. Rest of CORCON settings are
                                                            set to the default POR values. */
    CORCON_MODE_ENABLEALLSATSUPER_ROUNDUNBIASED  = 0x00F0  /**< Enable saturation for ACCA, ACCB
                                                            and Dataspace write, enable super
                                                            ACCA/ACCB saturation mode and set
                                                            rounding to Unbiased (convergent)
                                                            mode. Rest of CORCON settings are
                                                            set to the default POR values. */
} SYSTEM_CORCON_MODES;


#endif	/* SYSTEM_TYPES_H */
/**
 End of File
*/