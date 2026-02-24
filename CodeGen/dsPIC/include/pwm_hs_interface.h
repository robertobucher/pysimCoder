/**
 * PWM Generated Driver Interface Header File
 * 
 * @file      pwm_hs_interface.h
 * 
 * @defgroup  pwmhsdriver PWM Driver
 * 
 * @brief     High-Resolution Pulse-Width Modulated (PWM) with Fine Edge 
 *            Placement using dsPIC MCUs. 
 *
 * @skipline @version   PLIB Version 1.2.0
 *
 * @skipline  Device : 
*/

/*disclaimer*/

#ifndef PWM_HS_INTERFACE_H
#define PWM_HS_INTERFACE_H

// Section: Included Files

#include <stdbool.h>
#include <stdint.h>
#include <pwm_hs_types.h>

// Section: Data Type Definitions
        
/**
 @ingroup  pwmhsdriver
 @struct   PWM_HS_INTERFACE
 @brief    Structure containing the function pointers of PWM driver
*/

struct PWM_HS_INTERFACE{
    void (*Initialize)(void);
    ///< Pointer to \ref PWM_Initialize
    
    void (*Deinitialize)(void);
    ///< Pointer to \ref PWM_Deinitialize
    
    void (*Disable)(void);
    ///< Pointer to \ref PWM_Disable
    
    void (*Enable)(void);
    ///< Pointer to \ref PWM_Enable
    
    void (*MasterPeriodSet)(uint32_t period);
    ///< Pointer to \ref PWM_MasterPeriodSet
    
    void (*MasterDutyCycleSet)(uint32_t masterDutyCycle);
    ///< Pointer to \ref PWM_MasterDutyCycleSet

    void (*MasterPhaseSet)(uint32_t masterPhase);
    ///< Pointer to \ref PWM_MasterPhaseSet (Defined if PWM_GENERATOR_MASTER_PHASE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL) 
    
    void (*PeriodSet)(enum PWM_GENERATOR genNum, uint32_t period);
    ///< Pointer to \ref PWM_PeriodSet
    
    void (*ModeSet)(enum PWM_GENERATOR genNum, enum PWM_MODES mode);
    ///< Pointer to \ref PWM_ModeSet
    
    void (*OutputModeSet)(enum PWM_GENERATOR genNum, enum PWM_HS_OUTPUT_MODES outputMode);
    ///< Pointer to \ref PWM_OutputModeSet
	
    void (*DutyCycleSet)(enum PWM_GENERATOR genNum, uint32_t dutyCycle);
    ///< Pointer to \ref PWM_DutyCycleSet
	
    void (*PhaseSelect)(enum PWM_GENERATOR genNum, enum PWM_SOURCE_SELECT source);
    ///< Pointer to \ref PWM_PhaseSelect
    
    void (*PhaseSet)(enum PWM_GENERATOR genNum, uint32_t phase);
    ///< Pointer to \ref PWM_PhaseSet
    
    void (*OverrideDataSet)(enum PWM_GENERATOR genNum, uint16_t overrideData);
    ///< Pointer to \ref PWM_OverrideDataSet
    
    void (*OverrideDataHighSet)(enum PWM_GENERATOR genNum, bool overrideDataHigh);
    ///< Pointer to \ref PWM_OverrideDataHighSet
    
    void (*OverrideDataLowSet)(enum PWM_GENERATOR genNum, bool overrideDataLow);
    ///< Pointer to \ref PWM_OverrideDataLowSet
    
    uint16_t (*OverrideDataGet)(enum PWM_GENERATOR genNum);
    ///< Pointer to \ref PWM_OverrideDataGet
    
    void (*OverrideHighEnable)(enum PWM_GENERATOR genNum);
    ///< Pointer to \ref PWM_OverrideHighEnable
    
    void (*OverrideLowEnable)(enum PWM_GENERATOR genNum);
    ///< Pointer to \ref PWM_OverrideLowEnable
    
    void (*OverrideHighDisable)(enum PWM_GENERATOR genNum);
    ///< Pointer to \ref PWM_OverrideHighDisable
    
    void (*OverrideLowDisable)(enum PWM_GENERATOR genNum);
    ///< Pointer to \ref PWM_OverrideLowDisable
    
    void (*DeadTimeLowSet)(enum PWM_GENERATOR genNum,uint16_t deadtimeLow);
    ///< Pointer to \ref PWM_DeadTimeLowSet
    
    void (*DeadTimeHighSet)(enum PWM_GENERATOR genNum,uint16_t deadtimeHigh);
    ///< Pointer to \ref PWM_DeadTimeHighSet
    
    void (*DeadTimeSet)(enum PWM_GENERATOR genNum,uint16_t deadtime);
    ///< Pointer to \ref PWM_DeadTimeSet
    
    void (*TriggerCompareValueSet)(enum PWM_GENERATOR genNum, uint32_t trigCompValue);
    ///< Pointer to \ref PWM_TriggerCompareValueSet
    
    void (*GeneratorInterruptEnable)(enum PWM_GENERATOR genNum, enum PWM_GENERATOR_INTERRUPT interrupt);
    ///< Pointer to \ref PWM_GeneratorInterruptEnable
    
    void (*GeneratorInterruptDisable)(enum PWM_GENERATOR genNum, enum PWM_GENERATOR_INTERRUPT interrupt);
    ///< Pointer to \ref PWM_GeneratorInterruptDisable
    
    bool (*GeneratorEventStatusGet)(enum PWM_GENERATOR genNum, enum PWM_GENERATOR_INTERRUPT interrupt);
    ///< Pointer to \ref PWM_GeneratorEventStatusGet
    
    void (*GeneratorEventStatusClear)(enum PWM_GENERATOR genNum, enum PWM_GENERATOR_INTERRUPT interrupt);
    ///< Pointer to \ref PWM_GeneratorEventStatusClear
        
    void (*SpecialEventPrimaryCompareValueSet)(uint16_t compareValue);
    ///< Pointer to \ref PWM_SpecialEventPrimaryCompareValueSet (Defined if PWM_SPECIAL_EVENT_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)
    
    void (*SpecialEventTriggerInterruptFlagClear)(void);
    ///< Pointer to \ref PWM_SpecialEventTriggerInterruptFlagClear (Defined if PWM_SPECIAL_EVENT_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)

    void (*SpecialEventPrimaryInterruptEnable)(void);
    ///< Pointer to \ref PWM_SpecialEventPrimaryInterruptEnable (Defined if PWM_SPECIAL_EVENT_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)

    void (*SpecialEventPrimaryInterruptDisable)(void);
    ///< Pointer to \ref PWM_SpecialEventPrimaryInterruptDisable (Defined if PWM_SPECIAL_EVENT_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)

    void (*FaultModeLatchDisable)(enum PWM_GENERATOR genNum);
    ///< Pointer to \ref PWM_FaultModeLatchDisable (Defined if PWM_FAULT_MODE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)
    
    void (*FaultModeLatchEnable)(enum PWM_GENERATOR genNum);
    ///< Pointer to \ref PWM_FaultModeLatchEnable (Defined if PWM_FAULT_MODE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)

    void (*GeneratorDisable)(enum PWM_GENERATOR genNum);
    ///< Pointer to \ref PWM_GeneratorDisable (Defined if PWM_GENERATOR_ENABLE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)

    void (*GeneratorEnable)(enum PWM_GENERATOR genNum);
    ///< Pointer to \ref PWM_GeneratorEnable (Defined if PWM_GENERATOR_ENABLE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)
    
    void (*TriggerACompareValueSet)(enum PWM_GENERATOR genNum, uint32_t trigCompValue);
    ///< Pointer to \ref PWM_TriggerACompareValueSet (Defined if PWM_MULTIPLE_TRIGGER_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)

    void (*TriggerBCompareValueSet)(enum PWM_GENERATOR genNum, uint32_t trigCompValue);
    ///< Pointer to \ref PWM_TriggerBCompareValueSet (Defined if PWM_MULTIPLE_TRIGGER_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)

    void (*TriggerCCompareValueSet)(enum PWM_GENERATOR genNum, uint32_t trigCompValue);
    ///< Pointer to \ref PWM_TriggerCCompareValueSet (Defined if PWM_MULTIPLE_TRIGGER_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)
    
    void (*TriggerDCompareValueSet)(enum PWM_GENERATOR genNum, uint32_t trigCompValue);
    ///< Pointer to \ref PWM_TriggerDCompareValueSet (Defined if PWM_MULTIPLE_TRIGGER_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)
    
    void (*TriggerECompareValueSet)(enum PWM_GENERATOR genNum, uint32_t trigCompValue);
    ///< Pointer to \ref PWM_TriggerECompareValueSet (Defined if PWM_MULTIPLE_TRIGGER_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)
    
    void (*TriggerFCompareValueSet)(enum PWM_GENERATOR genNum, uint32_t trigCompValue);
    ///< Pointer to \ref PWM_TriggerFCompareValueSet (Defined if PWM_MULTIPLE_TRIGGER_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)
    
    void (*SoftwareUpdateRequest)(enum PWM_GENERATOR genNum);
    ///< Pointer to \ref PWM_SoftwareUpdateRequest (Defined if PWM_SOFTWARE_UPDATE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL) 

    bool (*SoftwareUpdatePending)(enum PWM_GENERATOR genNum);
    ///< Pointer to \ref PWM_SoftwareUpdatePending (Defined if PWM_SOFTWARE_UPDATE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)
    
    void (*FaultModeLatchClear)(enum PWM_GENERATOR genNum);
    ///< Pointer to \ref PWM_FaultModeLatchClear (Defined if PWM_FAULT_LATCH_SOFTWARE_CLEAR_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)
    
    void (*Trigger1Enable)(enum PWM_GENERATOR genNum, enum PWM_TRIGGER_COMPARE compareRegister);
    ///< Pointer to \ref PWM_Trigger1Enable (Defined if PWM_TRIGGER_ENABLE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)

    void (*Trigger1Disable)(enum PWM_GENERATOR genNum, enum PWM_TRIGGER_COMPARE compareRegister);
    ///< Pointer to \ref PWM_Trigger1Disable (Defined if PWM_TRIGGER_ENABLE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)

    void (*Trigger2Enable)(enum PWM_GENERATOR genNum, enum PWM_TRIGGER_COMPARE compareRegister);
    ///< Pointer to \ref PWM_Trigger2Enable (Defined if PWM_TRIGGER_ENABLE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)        
    
    void (*Trigger2Disable)(enum PWM_GENERATOR genNum, enum PWM_TRIGGER_COMPARE compareRegister);
    ///< Pointer to \ref PWM_Trigger2Disable (Defined if PWM_TRIGGER_ENABLE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)
    
    void (*DACTriggerEnable)(enum PWM_GENERATOR genNum, enum PWM_DAC_TRIGGER_COMPARE compareRegister);
    ///< Pointer to \ref PWM_DACTriggerEnable (Defined if PWM_TRIGGER_ENABLE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)
    
    void (*DACTriggerDisable)(enum PWM_GENERATOR genNum, enum PWM_DAC_TRIGGER_COMPARE compareRegister);
    ///< Pointer to \ref PWM_DACTriggerDisable (Defined if PWM_TRIGGER_ENABLE_FEATURE_AVAILABLE is 1 in PWM_features.h, else NULL)
    
    void (*GeneratorEOCEventCallbackRegister)(void (*callback)(enum PWM_GENERATOR genNum));
    ///< Pointer to \ref PWM_GeneratorEOCEventCallbackRegister

    void (*CommonEventCallbackRegister)(void (*callback)(enum PWM_COMMON_EVENT event));
    ///< Pointer to \ref PWM_CommonEventCallbackRegister

    void (*GeneratorTasks)(enum PWM_GENERATOR intGen);
    ///< Pointer to \ref PWM_GeneratorTasks (Supported only in polling mode)
    
    void (*CommonEventTasks)(enum PWM_COMMON_EVENT event);
    ///< Pointer to \ref PWM_CommonEventTasks (Supported only in polling mode)
};

#endif  //PWM_HS_INTERFACE_H
/**
 End of File
*/
