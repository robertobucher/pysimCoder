/**
 * PWM Generated Feature Header File
 * 
 * @file      pwm_features.h
 * 
 * @ingroup   pwmhsdriver
 * 
 * @brief     This is the generated module feature header file for PWM driver. 
 *            This file provides module feature list available on the selected 
 *            device. The macros defined in this file provides the flexibility 
 *            to easily migrate the user application to other device which might 
 *            have varied feature list. 
 * 
 *            The content in this file is strictly "read only" and should not be altered
 *
 * @skipline @version   PLIB Version 1.2.0
 *
 * @skipline  Device : 
*/

/*disclaimer*/

#ifndef PWM_FEATURES
#define PWM_FEATURES

/*******************************************************************************
            Macros defined for features supported in the device
*******************************************************************************/

/** 
 @ingroup  pwmhsdriver
 @brief    This macro defines the PWM master phase feature availability in 
           the PWM driver.

 <b>APIs Supported:</b><br>
  PWM_MasterPhaseSet(uint16_t masterPhase);<br>
  Refer driver header file for detailed description of the APIs.
*/
#define PWM_GENERATOR_MASTER_PHASE_FEATURE_AVAILABLE 1

/** 
 @ingroup  pwmhsdriver
 @brief    This macro defines the PWM generator enable feature availability in 
           the PWM driver.
           
 <b>APIs Supported:</b><br>
  PWM_GeneratorEnable(PWM_GENERATOR genNum);<br>
  PWM_GeneratorDisable(PWM_GENERATOR genNum);<br>
  Refer driver header file for detailed description of the APIs.
*/
#define PWM_GENERATOR_ENABLE_FEATURE_AVAILABLE   1 

/** 
 @ingroup  pwmhsdriver
 @brief    This macro defines the PWM data update request feature availability in 
           the PWM driver.
           
 <b>APIs Supported:</b><br>
  PWM_SoftwareUpdateRequest(PWM_GENERATOR genNum);<br>
  PWM_SoftwareUpdatePending(PWM_GENERATOR genNum);<br>
  Refer driver header file for detailed description of the APIs.
*/
#define PWM_SOFTWARE_UPDATE_FEATURE_AVAILABLE    1  
        
/** 
 @ingroup  pwmhsdriver
 @brief    This macro defines the PWM fault latch clear through software feature 
           availability in the PWM driver.
           
 <b>APIs Supported:</b><br>
  PWM_FaultModeLatchClear(PWM_GENERATOR genNum);<br>
  Refer driver header file for detailed description of the APIs.
*/
#define PWM_FAULT_LATCH_SOFTWARE_CLEAR_FEATURE_AVAILABLE 1          

/** 
 @ingroup  pwmhsdriver
 @brief    This macro defines the PWM multiple trigger feature availability in 
           the PWM driver.
           
 <b>APIs Supported:</b><br>
  PWM_TriggerACompareValueSet(PWM_GENERATOR genNum, uint16_t trigCompValue);<br>
  PWM_TriggerBCompareValueSet(PWM_GENERATOR genNum, uint16_t trigCompValue);<br>
  PWM_TriggerCCompareValueSet(PWM_GENERATOR genNum, uint16_t trigCompValue);<br>
  Refer driver header file for detailed description of the APIs.
*/
#define PWM_MULTIPLE_TRIGGER_FEATURE_AVAILABLE   1

/** 
 @ingroup  pwmhsdriver
 @brief    This macro defines the PWM multiple trigger enable availability in 
           the PWM driver.
           
 <b>APIs Supported:</b><br>
  PWM_Trigger1Enable(enum PWM_GENERATOR genNum, enum PWM_TRIGGER_COMPARE compareRegister);<br>
  PWM_Trigger1Disable(enum PWM_GENERATOR genNum, enum PWM_TRIGGER_COMPARE compareRegister);<br>
  PWM_Trigger2Enable(enum PWM_GENERATOR genNum, enum PWM_TRIGGER_COMPARE compareRegister);<br>
  PWM_Trigger2Disable(enum PWM_GENERATOR genNum, enum PWM_TRIGGER_COMPARE compareRegister);<br>
  Refer driver header file for detailed description of the APIs.
*/
#define PWM_TRIGGER_ENABLE_FEATURE_AVAILABLE   1

/*******************************************************************************
            Macros defined for features not supported in the device
*******************************************************************************/
  
/** 
 @ingroup  pwmhsdriver
 @brief    This macro defines the PWM special event trigger feature availability 
           in the PWM driver.
           
 <b>APIs Supported:</b><br>
  NA
*/
#define PWM_SPECIAL_EVENT_FEATURE_AVAILABLE  0

/** 
 @ingroup  pwmhsdriver
 @brief    This macro defines the PWM fault mode enable and disable feature 
           availability in the PWM driver.
           
 <b>APIs Supported:</b><br>
  NA
*/
#define PWM_FAULT_MODE_ENABLE_FEATURE_AVAILABLE 0

#endif //PWM_FEATURES
