/**
 * ADC Generated Driver Interface Header File
 * 
 * @file      adc_interface.h
 *            
 * @defgroup  adcdriver ADC Driver
 *            
 * @brief      12-bit Analog-to-Digital Converters (ADC) that feature low conversion latency,
 *             high resolution and oversampling capabilities to improve performance.
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

#ifndef ADC_INTERFACE_H
#define ADC_INTERFACE_H

// Section: Included Files
#include <stdint.h>
#include <stdbool.h>
#include <adc_types.h>
#include <interrupt_types.h>

// Section: Data Type Definitions
  
/**
 @ingroup  adcdriver
 @struct   ADC_INTERFACE
 @brief    Structure containing the function pointers of ADC driver
*/
struct ADC_INTERFACE
{   
    void (*Initialize)(void);              
    ///< Pointer to ADCx_Initialize e.g. \ref ADC1_Initialize
    
    void (*Deinitialize)(void);            
    ///< Pointer to ADCx_Deinitialize e.g. \ref ADC1_Deinitialize
    
    void (*Enable)(void);                  
    ///< Pointer to ADCx_Enable e.g. \ref ADC1_Enable
    
    bool (*IsReady)(void);                  
    ///< Pointer to ADCx_IsReady e.g. \ref ADC1_IsReady
    
    void (*Disable)(void);                 
    ///< Pointer to ADCx_Disable e.g. \ref ADC1_Disable
    
    void (*SoftwareTriggerEnable)(void);   
    ///< Pointer to ADCx_SoftwareTriggerEnable e.g. \ref ADC1_SoftwareTriggerEnable
    
    void (*SoftwareTriggerDisable)(void);  
    ///< Pointer to ADCx_SoftwareTriggerDisable e.g. \ref ADC1_SoftwareTriggerDisable
    
    void (*ChannelSelect)(const unsigned channel);  
    ///< Pointer to ADCx_ChannelSelect e.g. \ref ADC1_ChannelSelect
    
    uint32_t (*ConversionResultGet)(const unsigned channel);  
    ///< Pointer to ADCx_ConversionResultGet e.g. \ref ADC1_ConversionResultGet
    
    uint32_t (*SecondaryAccumulatorGet)(const unsigned channel);  
    ///< Pointer to ADCx_SecondaryAccumulatorGet e.g. \ref ADC1_SecondaryAccumulatorGet
    
    bool (*IsConversionComplete)(const unsigned channel);     
    ///< Pointer to ADCx_IsConversionComplete e.g. \ref ADC1_IsConversionComplete

    void (*ResolutionSet)(enum ADC_RESOLUTION_TYPE resolution);
    ///< Pointer to ADCx_ResolutionSet e.g. \ref ADC1_ResolutionSet

    void (*InterruptEnable)(void);        
    ///< Pointer to ADCx_InterruptEnable e.g. \ref ADC1_InterruptEnable
    
    void (*InterruptDisable)(void);       
    ///< Pointer to ADCx_InterruptDisable e.g. \ref ADC1_InterruptDisable
    
    void (*InterruptFlagClear)(void);     
    ///< Pointer to ADCx_InterruptFlagClear e.g. \ref ADC1_InterruptFlagClear
    
    void (*InterruptPrioritySet)(uint16_t priorityValue);
    ///< Pointer to ADCx_InterruptPrioritySet e.g. \ref ADC1_InterruptPrioritySet
    
    void (*CommonCallbackRegister)(void (*callback)(void));  
    ///< Pointer to ADCx_CommonCallbackRegister e.g. \ref ADC1_CommonCallbackRegister
    
    void (*Tasks)(void);                                       
    ///< Pointer to ADCx_Tasks e.g. \ref ADC1_Tasks (Supported only in polling mode) 
    
    const struct ADC_MULTICORE *adcMulticoreInterface; 
    ///< Pointer to \ref ADC_MULTICORE
    
};

/**
 @ingroup  adcdriver
 @struct   ADC_MULTICORE
 @brief    Structure containing the function pointers of ADC driver
*/
struct ADC_MULTICORE
{
    void (*ChannelSoftwareTriggerEnable)(const unsigned channel);
    ///< Pointer to ADCx_ChannelSoftwareTriggerEnable e.g. \ref ADC1_ChannelSoftwareTriggerEnable
    
    void (*ChannelSoftwareTriggerDisable)(const unsigned channel);
    ///< Pointer to ADCx_ChannelSoftwareTriggerDisable e.g. \ref ADC1_ChannelSoftwareTriggerDisable
    
    uint16_t (*SampleCountGet)(const unsigned channel);
    ///< Pointer to ADCx_SampleCountGet e.g. \ref ADC1_SampleCountGet
    
    uint16_t (*SampleCountStatusGet)(const unsigned channel);
    ///< Pointer to ADCx_SampleCountStatusGet e.g. \ref ADC1_SampleCountStatusGet
    
    void (*ChannelCallbackRegister)(void(*callback)(const unsigned channel, uint16_t adcVal));       
    ///< Pointer to ADCx_ChannelCallbackRegister e.g. \ref ADC1_ChannelCallbackRegister
    
    void (*Result32BitChannelCallbackRegister)(void(*callback)(const unsigned channel, uint32_t adcVal));       
    ///< Pointer to ADCx_Result32BitChannelCallbackRegister e.g. \ref ADC1_Result32BitChannelCallbackRegister
    
    void (*ComparatorCallbackRegister)(void(*callback)(const unsigned comparator));    
    ///< Pointer to ADCx_ComparatorCallbackRegister e.g. \ref ADC1_ComparatorCallbackRegister
    
    void (*ChannelTasks)(const unsigned channel);  
    ///< Pointer to ADCx_ChannelTasks e.g. \ref ADC1_ChannelTasks
    
    void (*ComparatorTasks)(const unsigned comparator);  
    ///< Pointer to ADCx_ComparatorTasks e.g. \ref ADC1_ComparatorTasks
    
    void (*IndividualChannelInterruptEnable)(const unsigned channel);  
    ///< Pointer to ADCx_IndividualChannelInterruptEnable e.g. \ref ADC1_IndividualChannelInterruptEnable
    
    void (*IndividualChannelInterruptDisable)(const unsigned channel);  
    ///< Pointer to ADCx_IndividualChannelInterruptDisable e.g. \ref ADC1_IndividualChannelInterruptDisable
    
    void (*IndividualChannelInterruptFlagClear)(const unsigned channel);  
    ///< Pointer to ADCx_IndividualChannelInterruptFlagClear e.g. \ref ADC1_IndividualChannelInterruptFlagClear
    
    void (*IndividualChannelInterruptPrioritySet)(const unsigned channel,enum INTERRUPT_PRIORITY priorityValue);  
    ///< Pointer to ADCx_IndividualChannelInterruptPrioritySet e.g. \ref ADC1_IndividualChannelInterruptPrioritySet
    
    void (*CorePowerEnable)(enum ADC_DEDICATED_CORE core);  
    ///< Pointer to ADCx_CorePowerEnable e.g. \ref ADC1_CorePowerEnable
    
    void (*SharedCorePowerEnable) (void);  
    ///< Pointer to ADCx_SharedCorePowerEnable e.g. \ref ADC1_SharedCorePowerEnable
    
    void (*CoreCalibration)(enum ADC_DEDICATED_CORE core);  
    ///< Pointer to ADCx_CoreCalibration e.g. \ref ADC1_CoreCalibration
    
    void (*SharedCoreCalibration) (void); 
    ///< Pointer to ADCx_SharedCoreCalibration e.g. \ref ADC1_SharedCoreCalibration
    
    void (*PWMTriggerSourceSet)(const unsigned channel, enum ADC_PWM_INSTANCE pwmInstance,  enum ADC_PWM_TRIGGERS triggerNumber);
    ///< Pointer to ADCx_PWMTriggerSourceSet e.g. \ref ADC1_PWMTriggerSourceSet
};

#endif //ADC_INTERFACE_H
