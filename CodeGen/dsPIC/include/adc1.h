/**
 * ADC1 Generated Driver Header File
 * 
 * @file      adc1.h
 *            
 * @ingroup   adcdriver
 *            
 * @brief     This is the generated driver header file for the ADC1 driver          
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

#ifndef ADC1_H
#define ADC1_H

// Section: Included Files

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <adc_types.h>
#include <adc_interface.h>

// Section: Data Types

/** 
  @ingroup  adcdriver
  @brief    Defines the ADC Resolution
*/
#define ADC1_RESOLUTION 12

/**
 @ingroup  adcdriver
 @enum     ADC1_CHANNEL
 @brief    Defines the ADC channles that are selected from the MCC Melody 
           User Interface for the ADC conversions.
 @note     The enum list in the Help document might be just a reference to show 
           the analog channel list. Generated enum list is based on the configuration 
           done by user in the MCC Melody user interface.
*/
enum ADC1_CHANNEL
{
    ADC1_Channel0,    /**<Channel Name:Channel_0 connected to ADC1_AN0 */
    ADC1_Channel1,    /**<Channel Name:Channel_1 connected to ADC1_AN1 */
    ADC1_Channel2,    /**<Channel Name:Channel_2 connected to ADC1_AN2 */
    ADC1_Channel3,    /**<Channel Name:Channel_3 connected to ADC1_AN3 */
    ADC1_MAX_CHANNELS = 4    /**< Maximum number of channels configured by user for ADC1 */
};

/**
 @ingroup  adcdriver
 @enum     ADC_CMP
 @brief    Defines the ADC1 comparators that are 
           available for the module to use.
*/
enum ADC1_CMP
{
    ADC1_MAX_CMPS = 0    /**< Maximum Comparators configured by user for ADC1 */
};

// Section: Data Type Definitions

/**
 * @ingroup  adcdriver
 * @brief    Structure object of type ADC_INTERFACE with the custom name
 *           given by the user in the Melody Driver User interface. The default name 
 *           e.g. ADC1 can be changed by the user in the ADC user interface. 
 *           This allows defining a structure with application specific name using 
 *           the 'Custom Name' field. Application specific name allows the API Portability.
*/
extern const struct ADC_INTERFACE ADC1;


// Section: Driver Interface Functions

/**
 * @ingroup  adcdriver
 * @brief    Initializes ADC1 module, using the given initialization data
 *           This function must be called before any other ADC1 function is called
 * @param    none
 * @return   none  
 */
void ADC1_Initialize (void);

/**
 * @ingroup  adcdriver
 * @brief    Deinitializes the ADC1 to POR values
 * @param    none
 * @return   none  
 */
void ADC1_Deinitialize(void);

/**
 * @ingroup  adcdriver
 * @brief    This inline function enables the ADC1 module
 * @pre      \ref ADC1_IsReady must be called to know the status of ADC
 * @param    none
 * @return   none  
 */
inline static void ADC1_Enable(void)
{
    AD1CONbits.ON = 1U;
}

/**
 * @ingroup     adcdriver
 * @brief       This inline function returns true if ADC is ready
 * @pre         This function must be called after calling \ref ADC1_Enable to know ADC status
 * @param       none
 * @return      true - ADC is ready
 * @return      false - ADC is not ready 
 */
inline static bool ADC1_IsReady(void)
{
    return (bool)AD1CONbits.ADRDY;
}

/**
 * @ingroup  adcdriver
 * @brief    This inline function disables the ADC1 module
 * @pre      none
 * @param    none
 * @return   none  
 */
inline static void ADC1_Disable(void)
{
   AD1CONbits.ON = 0U;
}

/**
 * @ingroup  adcdriver
 * @brief    This inline function sets software common trigger
 * @pre      none
 * @param    none
 * @return   none  
 */
inline static void ADC1_SoftwareTriggerEnable(void)
{
   AD1SWTRG = 0xFFFFFFFFU;
}

/**
 * @ingroup  adcdriver
 * @brief    This inline function resets software common trigger
 * @pre      none
 * @param    none
 * @return   none  
 */
inline static void ADC1_SoftwareTriggerDisable(void)
{
   AD1SWTRG = 0x0U;
}

/**
 * @ingroup     adcdriver
 * @brief       This inline function sets individual software trigger
 * @pre         none
 * @param[in]   channel - Channel for conversion      none
 * @return      none  
 */
inline static void ADC1_ChannelSoftwareTriggerEnable(const enum ADC1_CHANNEL channel)
{
    switch(channel)
    {
        case ADC1_Channel0:
                AD1SWTRGbits.CH0TRG = 0x1U;
                break;
        case ADC1_Channel1:
                AD1SWTRGbits.CH1TRG = 0x1U;
                break;
        case ADC1_Channel2:
                AD1SWTRGbits.CH2TRG = 0x1U;
                break;
        case ADC1_Channel3:
                AD1SWTRGbits.CH3TRG = 0x1U;
                break;
        default:
                break;
    }
}

/**
 * @ingroup     adcdriver
 * @brief       This inline function clears individual software trigger
 * @pre         none
 * @param[in]   channel - Channel for conversion  
 * @return      none  
 */
inline static void ADC1_ChannelSoftwareTriggerDisable(const enum ADC1_CHANNEL channel)
{
    switch(channel)
    {
        case ADC1_Channel0:
                AD1SWTRGbits.CH0TRG = 0x0U;
                break;
        case ADC1_Channel1:
                AD1SWTRGbits.CH1TRG = 0x0U;
                break;
        case ADC1_Channel2:
                AD1SWTRGbits.CH2TRG = 0x0U;
                break;
        case ADC1_Channel3:
                AD1SWTRGbits.CH3TRG = 0x0U;
                break;
        default:
                break;
    }
}

/**
 * @ingroup    adcdriver
 * @brief      This inline function returns the requested conversion count
 * @pre        none
 * @param[in]  channel - Channel for conversion  
 * @return     requested number of conversions  
 * @note       This function is applicable in Window mode and Integration conversion mode only 
 */
inline static uint16_t ADC1_SampleCountGet(const enum ADC1_CHANNEL channel)
{
    uint16_t count = 0x0U;

    switch(channel)
    {
        case ADC1_Channel0:
                count = AD1CH0CNTbits.CNT;
                break;
        case ADC1_Channel1:
                count = AD1CH1CNTbits.CNT;
                break;
        case ADC1_Channel2:
                count = AD1CH2CNTbits.CNT;
                break;
        case ADC1_Channel3:
                count = AD1CH3CNTbits.CNT;
                break;
        default:
                break;
    }
    return count;
}

/**
 * @ingroup    adcdriver
 * @brief      This inline function returns the status of completed conversion count
 * @pre        none
 * @param[in]  channel - Channel for conversion  
 * @return     number of conversions completed  
 * @note       This function is applicable in Window mode and Integration conversion mode only 
 */
inline static uint16_t ADC1_SampleCountStatusGet(const enum ADC1_CHANNEL channel)
{
    uint16_t countStatus = 0x0U;

    switch(channel)
    {
        case ADC1_Channel0:
                countStatus = AD1CH0CNTbits.CNTSTAT;
                break;
        case ADC1_Channel1:
                countStatus = AD1CH1CNTbits.CNTSTAT;
                break;
        case ADC1_Channel2:
                countStatus = AD1CH2CNTbits.CNTSTAT;
                break;
        case ADC1_Channel3:
                countStatus = AD1CH3CNTbits.CNTSTAT;
                break;
        default:
                break;
    }
    return countStatus;
}

/**
 * @ingroup    adcdriver
 * @brief      Returns the conversion value for the channel selected
 * @pre        This inline function returns the conversion value only after the conversion is complete. 
 *             Conversion completion status can be checked using 
 *             \ref ADC1_IsConversionComplete(channel) function.
 * @param[in]  channel - Selected channel  
 * @return     Returns the analog to digital converted value  
 */
inline static uint32_t ADC1_ConversionResultGet(const enum ADC1_CHANNEL channel)
{
    uint32_t result = 0x0U;

    switch(channel)
    {
        case ADC1_Channel0:
                result = AD1CH0DATA;
                break;
        case ADC1_Channel1:
                result = AD1CH1DATA;
                break;
        case ADC1_Channel2:
                result = AD1CH2DATA;
                break;
        case ADC1_Channel3:
                result = AD1CH3DATA;
                break;
        default:
                break;
    }
    return result;
}

/**
 * @ingroup    adcdriver
 * @brief      This inline function returns the status of conversion.This function is used to 
 *             determine if conversion is completed. When conversion is complete 
 *             the function returns true otherwise false.
 * @pre        \ref ADC1_SoftwareTriggerEnable() function should have been 
 *             called before calling this function.
 * @param[in]  channel - Selected channel  
 * @return     true - Conversion is complete.
 * @return     false - Conversion is not complete.  
 */
inline static bool ADC1_IsConversionComplete(const enum ADC1_CHANNEL channel)
{
    bool status = false;

    switch(channel)
    {
        case ADC1_Channel0:
                status = AD1STATbits.CH0RDY;
                break;
        case ADC1_Channel1:
                status = AD1STATbits.CH1RDY;
                break;
        case ADC1_Channel2:
                status = AD1STATbits.CH2RDY;
                break;
        case ADC1_Channel3:
                status = AD1STATbits.CH3RDY;
                break;
        default:
                break;
    }

    return status;
}

/**
 * @ingroup    adcdriver
 * @brief      This inline function enables individual channel interrupt
 * @pre        none
 * @param[in]  channel - Selected channel  
 * @return     none  
 */
inline static void ADC1_IndividualChannelInterruptEnable(const enum ADC1_CHANNEL channel)
{
    switch(channel)
    {
        case ADC1_Channel0:
                IEC4bits.AD1CH0IE = 1U;
                break;
        case ADC1_Channel1:
                IEC4bits.AD1CH1IE = 1U;
                break;
        case ADC1_Channel2:
                IEC5bits.AD1CH2IE = 1U;
                break;
        case ADC1_Channel3:
                IEC5bits.AD1CH3IE = 1U;
                break;
        default:
                break;
    }
}

/**
 * @ingroup    adcdriver
 * @brief      This inline function disables individual channel interrupt
 * @pre        none
 * @param[in]  channel - Selected channel  
 * @return     none  
 */
inline static void ADC1_IndividualChannelInterruptDisable(const enum ADC1_CHANNEL channel)
{
    switch(channel)
    {
        case ADC1_Channel0:
                IEC4bits.AD1CH0IE = 0U;
                break;
        case ADC1_Channel1:
                IEC4bits.AD1CH1IE = 0U;
                break;
        case ADC1_Channel2:
                IEC5bits.AD1CH2IE = 0U;
                break;
        case ADC1_Channel3:
                IEC5bits.AD1CH3IE = 0U;
                break;
        default:
                break;
    }
}

/**
 * @ingroup    adcdriver
 * @brief      This inline function clears individual channel interrupt flag
 * @pre        The flag is not cleared without reading the data from buffer.
 *             Hence call \ref ADC1_ConversionResultGet() function to read data 
 *             before calling this function
 * @param[in]  channel - Selected channel  
 * @return     none  
 */
inline static void ADC1_IndividualChannelInterruptFlagClear(const enum ADC1_CHANNEL channel)
{
    switch(channel)
    {
        case ADC1_Channel0:
                IFS4bits.AD1CH0IF = 0U;
                break;
        case ADC1_Channel1:
                IFS4bits.AD1CH1IF = 0U;
                break;
        case ADC1_Channel2:
                IFS5bits.AD1CH2IF = 0U;
                break;
        case ADC1_Channel3:
                IFS5bits.AD1CH3IF = 0U;
                break;
        default:
                break;
    }
}

/**
 * @ingroup    adcdriver
 * @brief      This inline function allows selection of priority for individual channel interrupt
 * @pre        none
 * @param[in]  channel - Selected channel 
 * @param[in]  priorityValue  -  The numerical value of interrupt priority
 * @return     none  
 */
inline static void ADC1_IndividualChannelInterruptPrioritySet(const enum ADC1_CHANNEL channel, enum INTERRUPT_PRIORITY priorityValue)
{
	switch(channel)
	{
		case ADC1_Channel0:
				IPC19bits.AD1CH0IP = priorityValue;
				break;
		case ADC1_Channel1:
				IPC19bits.AD1CH1IP = priorityValue;
				break;
		case ADC1_Channel2:
				IPC20bits.AD1CH2IP = priorityValue;
				break;
		case ADC1_Channel3:
				IPC20bits.AD1CH3IP = priorityValue;
				break;
		default:
				break;
	}
}

/**
 * @ingroup    adcdriver
 * @brief      This function can be used to override default callback \ref ADC1_ChannelCallback
 *             and to define custom callback for ADC1 Channel event. 
 *             Read the conversion result of the corresponding channel in the custom callback.
 * @pre        none
 * @param[in]  callback - Address of the callback function.  
 * @return     none  
 */
void ADC1_ChannelCallbackRegister(void(*callback)(const enum ADC1_CHANNEL channel, uint16_t adcVal));

/**
 * @ingroup    adcdriver
 * @brief      This is the default callback function for all the analog channels. 
 *             This callback is triggered once the channel conversion is done for a
 *             channel and to read the conversion result of the corresponding channel
 * @pre        none
 * @param[in]  channel - conversion completed channel
 * @param[in]  adcVal - conversion result of channel  
 * @return     none  
 */
void ADC1_ChannelCallback(const enum ADC1_CHANNEL channel, uint16_t adcVal);

/**
 * @ingroup    adcdriver
 * @brief      This function can be used to override default callback \ref ADC1_Result32BitChannelCallback
 *             and to define custom callback for ADC1 Channel event. 
 * @pre        none
 *             Read the conversion result of the corresponding channel in the custom callback.
 * @param[in]  callback - Address of the callback function.  
 * @return     none  
 */
void ADC1_Result32BitChannelCallbackRegister(void(*callback)(const enum ADC1_CHANNEL channel, uint32_t adcVal));

/**
 * @ingroup    adcdriver
 * @brief      This is the default callback function for all the analog channels. 
 *             This callback is triggered once the channel conversion is done for a
 *             channel and to read the conversion result of the corresponding channel
 * @pre        none
 * @param[in]  channel - conversion completed channel
 * @param[in]  adcVal - conversion result of channel  
 * @return     none  
 */
void ADC1_Result32BitChannelCallback(const enum ADC1_CHANNEL channel, uint32_t adcVal);

/**
 * @ingroup    adcdriver
 * @brief      This function can be used to override default callback and to 
 *             define custom callback for ADC1_Comparator event
 * @pre        none
 * @param[in]  callback - Address of the callback function.  
 * @return     none  
 */
void ADC1_ComparatorCallbackRegister(void(*callback)(const enum ADC1_CMP comparator));

/**
 * @ingroup    adcdriver
 * @brief      Comparator callback function
 * @pre        none
 * @param[in]  comparator - comparator in which compare event occurred  
 * @return     none  
 */
void ADC1_ComparatorCallback(const enum ADC1_CMP comparator);

/**
 * @ingroup    adcdriver
 * @brief      This function call used only in polling mode, if channel 
 *             conversion is done for requested channel, the calls the 
 *             respective callback function
 * @pre        \ref ADC1_Initialize() function should have been  
 *             called before calling this function.
 * @param[in]  channel - Selected channel.  
 * @return     none  
 * @note       This function has to be polled to notify channel callbacks and clear 
 *             the channel interrupt flags in non-interrupt mode of ADC
 */
void ADC1_ChannelTasks(const enum ADC1_CHANNEL channel);


/**
 * @ingroup  adcdriver
 * @brief    Enables power for ADC1 Core
 *           This function is used to set the analog and digital power for 
 *           ADC1 shared Core.
 * @pre      none
 * @param    none
 * @return   none  
 */
void ADC1_SharedCorePowerEnable(void);

/**
 * @ingroup  adcdriver
 * @brief    Calibrates the ADC1 Core
 * @pre      none 
 * @param    none
 * @return   none  
 */
void ADC1_SharedCoreCalibration(void);

/**
 * @ingroup  adcdriver
 * @brief    Sets PWM trigger source for corresponding analog input 
 * @param[in]  channel - Selected channel  
 * @param[in]  pwmInstance - PWM instance for the trigger source
 * @param[in]  triggerNumber - 1, for PWMx Trigger 1
 * @param[in]  triggerNumber - 2, for PWMx Trigger 2
 * @return   none  
 * @note     Configure PWM trigger value using \ref PWM_TriggerACompareValueSet, \ref PWM_TriggerBCompareValueSet
 *           or \ref PWM_TriggerCCompareValueSet before calling this funcion and enable corresponding 
 *           PWM trigger using \ref PWM_Trigger1Enable or \ref PWM_Trigger2Enable post calling it.
 */
void ADC1_PWMTriggerSourceSet(const enum ADC1_CHANNEL channel, enum ADC_PWM_INSTANCE pwmInstance, enum ADC_PWM_TRIGGERS triggerNumber);


#endif //_ADC1_H
    
/**
 End of File
*/

