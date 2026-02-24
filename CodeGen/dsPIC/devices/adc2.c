/**
 * ADC2 Generated Driver Source File
 * 
 * @file      adc2.c
 *            
 * @ingroup   adcdriver
 *            
 * @brief     This is the generated driver source file for ADC2 driver        
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

// Section: Included Files
#include <stddef.h>
#include <adc2.h>

// Section: File specific functions

static void (*ADC2_ChannelHandler)(enum ADC2_CHANNEL channel, uint16_t adcVal) = NULL;
static void (*ADC2_Result32BitChannelHandler)(enum ADC2_CHANNEL channel, uint32_t adcVal) = NULL;
static void (*ADC2_ComparatorHandler)(enum ADC2_CMP comparator) = NULL;

// Section: File specific data type definitions

/**
 @ingroup  adcdriver
 @enum     ADC2_PWM_TRIG_SRCS
 @brief    Defines the PWM ADC TRIGGER sources available for the module to use.
*/
enum ADC2_PWM_TRIG_SRCS {
    PWM8_TRIGGER2 = 0x13, 
    PWM8_TRIGGER1 = 0x12, 
    PWM7_TRIGGER2 = 0x11, 
    PWM7_TRIGGER1 = 0x10, 
    PWM6_TRIGGER2 = 0xf, 
    PWM6_TRIGGER1 = 0xe, 
    PWM5_TRIGGER2 = 0xd, 
    PWM5_TRIGGER1 = 0xc, 
    PWM4_TRIGGER2 = 0xb, 
    PWM4_TRIGGER1 = 0xa, 
    PWM3_TRIGGER2 = 0x9, 
    PWM3_TRIGGER1 = 0x8, 
    PWM2_TRIGGER2 = 0x7, 
    PWM2_TRIGGER1 = 0x6, 
    PWM1_TRIGGER2 = 0x5, 
    PWM1_TRIGGER1 = 0x4, 
};

//Defines an object for ADC_MULTICORE.
static const struct ADC_MULTICORE adc2Multicore = {
    .ChannelSoftwareTriggerEnable           = &ADC2_ChannelSoftwareTriggerEnable,
    .ChannelSoftwareTriggerDisable          = &ADC2_ChannelSoftwareTriggerDisable,
    .SampleCountGet                         = NULL,
    .SampleCountStatusGet                   = NULL,
    .ChannelTasks                           = &ADC2_ChannelTasks, 
    .ComparatorTasks                        = NULL,
    .IndividualChannelInterruptEnable       = &ADC2_IndividualChannelInterruptEnable,
    .IndividualChannelInterruptDisable      = &ADC2_IndividualChannelInterruptDisable,
    .IndividualChannelInterruptFlagClear    = &ADC2_IndividualChannelInterruptFlagClear,
    .IndividualChannelInterruptPrioritySet  = &ADC2_IndividualChannelInterruptPrioritySet,
    .ChannelCallbackRegister                = &ADC2_ChannelCallbackRegister,
    .Result32BitChannelCallbackRegister     = &ADC2_Result32BitChannelCallbackRegister,
    .ComparatorCallbackRegister             = &ADC2_ComparatorCallbackRegister,
    .CorePowerEnable                        = NULL,
    .SharedCorePowerEnable                  = &ADC2_SharedCorePowerEnable,
    .CoreCalibration                        = NULL,
    .SharedCoreCalibration                  = &ADC2_SharedCoreCalibration,
    .PWMTriggerSourceSet                    = &ADC2_PWMTriggerSourceSet
};

//Defines an object for ADC_INTERFACE.

const struct ADC_INTERFACE ADC2 = {
    .Initialize             = &ADC2_Initialize,
    .Deinitialize           = &ADC2_Deinitialize,
    .Enable                 = &ADC2_Enable,
    .IsReady                = &ADC2_IsReady,
    .Disable                = &ADC2_Disable,
    .SoftwareTriggerEnable  = &ADC2_SoftwareTriggerEnable,
    .SoftwareTriggerDisable = &ADC2_SoftwareTriggerDisable,
    .ChannelSelect          = NULL, 
    .ConversionResultGet    = &ADC2_ConversionResultGet,
    .SecondaryAccumulatorGet= NULL,
    .IsConversionComplete   = &ADC2_IsConversionComplete,
    .ResolutionSet          = NULL,
    .InterruptEnable        = NULL,
    .InterruptDisable       = NULL,
    .InterruptFlagClear     = NULL,
    .InterruptPrioritySet   = NULL,
    .CommonCallbackRegister = NULL,
    .Tasks                  = NULL,
    .adcMulticoreInterface = &adc2Multicore,
};

// Section: Driver Interface Function Definitions

void ADC2_Initialize(void)
{
    //CALCNT Wait for 2 activity free ADC clock cycles; BUFEN disabled; TSTEN disabled; ON enabled; STNDBY disabled; RPTCNT 1 ADC clock cycles between triggers; CALRATE Every second; ACALEN disabled; CALREQ Calibration cycle is not requested; 
    AD2CON = (uint32_t)0x8000UL & ~_AD1CON_ON_MASK;
    //DATAOVR 0x0; 
    AD2DATAOVR = 0x0UL;
    //CH0RDY disabled; CH1RDY disabled; CH2RDY disabled; CH3RDY disabled; CH4RDY disabled; CH5RDY disabled; CH6RDY disabled; CH7RDY disabled; 
    AD2STAT = 0x0UL;
    //CH0RRDY disabled; CH1RRDY disabled; CH2RRDY disabled; CH3RRDY disabled; CH4RRDY disabled; CH5RRDY disabled; CH6RRDY disabled; CH7RRDY disabled; 
    AD2RSTAT = 0x0UL;
    //CH0CMP disabled; CH1CMP disabled; CH2CMP disabled; CH3CMP disabled; CH4CMP disabled; CH5CMP disabled; CH6CMP disabled; CH7CMP disabled; 
    AD2CMPSTAT = 0x0UL;
    //CH0TRG disabled; CH1TRG disabled; CH2TRG disabled; CH3TRG disabled; CH4TRG disabled; CH5TRG disabled; CH6TRG disabled; CH7TRG disabled; 
    AD2SWTRG = 0x0UL;
    //TRG1SRC PWM1 Trigger1; MODE Single sample initiated by TRG1SRC[4:0] trigger; TRG2SRC Triggers are disabled; ACCNUM 4 samples, 13 bits result; SAMC 0.5 TAD; IRQSEL enabled; EIEN disabled; TRG1POL disabled; PINSEL AD2AN0; NINSEL disabled; FRAC Integer; DIFF disabled; 
    AD2CH0CON1 = 0x200004UL;
    //TRG1SRC PWM1 Trigger1; MODE Single sample initiated by TRG1SRC[4:0] trigger; TRG2SRC Triggers are disabled; ACCNUM 4 samples, 13 bits result; SAMC 0.5 TAD; IRQSEL enabled; EIEN disabled; TRG1POL disabled; PINSEL AD2AN1; NINSEL disabled; FRAC Integer; DIFF disabled; 
    AD2CH1CON1 = 0x1200004UL;
    //TRG1SRC PWM1 Trigger1; MODE Single sample initiated by TRG1SRC[4:0] trigger; TRG2SRC Triggers are disabled; ACCNUM 4 samples, 13 bits result; SAMC 0.5 TAD; IRQSEL enabled; EIEN disabled; TRG1POL disabled; PINSEL AD2AN2; NINSEL disabled; FRAC Integer; DIFF disabled; 
    AD2CH2CON1 = 0x2200004UL;
    //ADCMPCNT disabled; CMPMOD NONE; CMPCNTMOD disabled; CMPVAL enabled; ACCBRST disabled; ACCRO disabled; 
    AD2CH0CON2 = 0x20000000UL;
    //ADCMPCNT disabled; CMPMOD NONE; CMPCNTMOD disabled; CMPVAL enabled; ACCBRST disabled; ACCRO disabled; 
    AD2CH1CON2 = 0x20000000UL;
    //ADCMPCNT disabled; CMPMOD NONE; CMPCNTMOD disabled; CMPVAL enabled; ACCBRST disabled; ACCRO disabled; 
    AD2CH2CON2 = 0x20000000UL;
    //
    AD2CH0RES = 0x0UL;
    //
    AD2CH1RES = 0x0UL;
    //
    AD2CH2RES = 0x0UL;
    //CNT 0x0; 
    AD2CH0CNT = 0x0UL;
    //CNT 0x0; 
    AD2CH1CNT = 0x0UL;
    //CNT 0x0; 
    AD2CH2CNT = 0x0UL;
    //CMPLO 0x0; 
    AD2CH0CMPLO = 0x0UL;
    //CMPLO 0x0; 
    AD2CH1CMPLO = 0x0UL;
    //CMPLO 0x0; 
    AD2CH2CMPLO = 0x0UL;
    //CMPHI 0x0; 
    AD2CH0CMPHI = 0x0UL;
    //CMPHI 0x0; 
    AD2CH1CMPHI = 0x0UL;
    //CMPHI 0x0; 
    AD2CH2CMPHI = 0x0UL;

    ADC2_ChannelCallbackRegister(&ADC2_ChannelCallback);
    ADC2_Result32BitChannelCallbackRegister(&ADC2_Result32BitChannelCallback);
    ADC2_ComparatorCallbackRegister(&ADC2_ComparatorCallback);
    
    
    // ADC Mode change to run mode
    ADC2_SharedCorePowerEnable();
}

void ADC2_Deinitialize (void)
{
    AD2CONbits.ON = 0U;
    
    AD2CON = 0x480000UL;
    AD2DATAOVR = 0x0UL;
    AD2STAT = 0x0UL;
    AD2RSTAT = 0x0UL;
    AD2CMPSTAT = 0x0UL;
    AD2SWTRG = 0x0UL;
    AD2CH0CON1 = 0x0UL;
    AD2CH1CON1 = 0x0UL;
    AD2CH2CON1 = 0x0UL;
    AD2CH0CON2 = 0x1UL;
    AD2CH1CON2 = 0x1UL;
    AD2CH2CON2 = 0x1UL;
    AD2CH0RES = 0x0UL;
    AD2CH1RES = 0x0UL;
    AD2CH2RES = 0x0UL;
    AD2CH0CNT = 0x0UL;
    AD2CH1CNT = 0x0UL;
    AD2CH2CNT = 0x0UL;
    AD2CH0CMPLO = 0x0UL;
    AD2CH1CMPLO = 0x0UL;
    AD2CH2CMPLO = 0x0UL;
    AD2CH0CMPHI = 0x0UL;
    AD2CH1CMPHI = 0x0UL;
    AD2CH2CMPHI = 0x0UL;
}

void ADC2_SharedCorePowerEnable (void) 
{
    AD2CONbits.ON = 1U;   
    while(AD2CONbits.ADRDY == 0U)
    {
    }
}

void ADC2_SharedCoreCalibration (void) 
{
    AD2CONbits.CALREQ = 1U;    
    while(AD2CONbits.CALRDY == 0U)
    {
    }  
}

static uint16_t ADC2_TriggerSourceValueGet(enum ADC_PWM_INSTANCE pwmInstance, enum ADC_PWM_TRIGGERS triggerNumber)
{
    uint16_t adcTriggerSourceValue = 0x0U;
    switch(pwmInstance)
    {
        case ADC_PWM_GENERATOR_8:
                if(triggerNumber == ADC_PWM_TRIGGER_1)
                {
                    adcTriggerSourceValue = PWM8_TRIGGER1;
                }
                else if(triggerNumber == ADC_PWM_TRIGGER_2)
                {
                    adcTriggerSourceValue = PWM8_TRIGGER2;
                }
                else
                {
                }
                break;
        case ADC_PWM_GENERATOR_7:
                if(triggerNumber == ADC_PWM_TRIGGER_1)
                {
                    adcTriggerSourceValue = PWM7_TRIGGER1;
                }
                else if(triggerNumber == ADC_PWM_TRIGGER_2)
                {
                    adcTriggerSourceValue = PWM7_TRIGGER2;
                }
                else
                {
                }
                break;
        case ADC_PWM_GENERATOR_6:
                if(triggerNumber == ADC_PWM_TRIGGER_1)
                {
                    adcTriggerSourceValue = PWM6_TRIGGER1;
                }
                else if(triggerNumber == ADC_PWM_TRIGGER_2)
                {
                    adcTriggerSourceValue = PWM6_TRIGGER2;
                }
                else
                {
                }
                break;
        case ADC_PWM_GENERATOR_5:
                if(triggerNumber == ADC_PWM_TRIGGER_1)
                {
                    adcTriggerSourceValue = PWM5_TRIGGER1;
                }
                else if(triggerNumber == ADC_PWM_TRIGGER_2)
                {
                    adcTriggerSourceValue = PWM5_TRIGGER2;
                }
                else
                {
                }
                break;
        case ADC_PWM_GENERATOR_4:
                if(triggerNumber == ADC_PWM_TRIGGER_1)
                {
                    adcTriggerSourceValue = PWM4_TRIGGER1;
                }
                else if(triggerNumber == ADC_PWM_TRIGGER_2)
                {
                    adcTriggerSourceValue = PWM4_TRIGGER2;
                }
                else
                {
                }
                break;
        case ADC_PWM_GENERATOR_3:
                if(triggerNumber == ADC_PWM_TRIGGER_1)
                {
                    adcTriggerSourceValue = PWM3_TRIGGER1;
                }
                else if(triggerNumber == ADC_PWM_TRIGGER_2)
                {
                    adcTriggerSourceValue = PWM3_TRIGGER2;
                }
                else
                {
                }
                break;
        case ADC_PWM_GENERATOR_2:
                if(triggerNumber == ADC_PWM_TRIGGER_1)
                {
                    adcTriggerSourceValue = PWM2_TRIGGER1;
                }
                else if(triggerNumber == ADC_PWM_TRIGGER_2)
                {
                    adcTriggerSourceValue = PWM2_TRIGGER2;
                }
                else
                {
                }
                break;
        case ADC_PWM_GENERATOR_1:
                if(triggerNumber == ADC_PWM_TRIGGER_1)
                {
                    adcTriggerSourceValue = PWM1_TRIGGER1;
                }
                else if(triggerNumber == ADC_PWM_TRIGGER_2)
                {
                    adcTriggerSourceValue = PWM1_TRIGGER2;
                }
                else
                {
                }
                break;
         default:
                break;
    }
    return adcTriggerSourceValue;
}

void ADC2_PWMTriggerSourceSet(enum ADC2_CHANNEL channel, enum ADC_PWM_INSTANCE pwmInstance, enum ADC_PWM_TRIGGERS triggerNumber)
{
    uint16_t adcTriggerValue;
    adcTriggerValue= ADC2_TriggerSourceValueGet(pwmInstance, triggerNumber);
    switch(channel)
    {
        case ADC2_Channel0:
                AD2CH0CON1bits.TRG1SRC = adcTriggerValue;
                break;
        case ADC2_Channel1:
                AD2CH1CON1bits.TRG1SRC = adcTriggerValue;
                break;
        case ADC2_Channel2:
                AD2CH2CON1bits.TRG1SRC = adcTriggerValue;
                break;
        default:
                break;
    }
}

void ADC2_ChannelCallbackRegister(void(*callback)(enum ADC2_CHANNEL channel, uint16_t adcVal))
{
    if(NULL != callback)
    {
        ADC2_ChannelHandler = callback;
    }
}

void __attribute__ ( ( weak ) ) ADC2_ChannelCallback (enum ADC2_CHANNEL channel, uint16_t adcVal)
{ 
    (void)channel;
    (void)adcVal;
} 

void ADC2_Result32BitChannelCallbackRegister(void(*callback)(enum ADC2_CHANNEL channel, uint32_t adcVal))
{
    if(NULL != callback)
    {
        ADC2_Result32BitChannelHandler = callback;
    }
}

void __attribute__ ( ( weak ) ) ADC2_Result32BitChannelCallback (enum ADC2_CHANNEL channel, uint32_t adcVal)
{ 
    (void)channel;
    (void)adcVal;
} 


/* cppcheck-suppress misra-c2012-8.4
*
* (Rule 8.4) REQUIRED: A compatible declaration shall be visible when an object or 
* function with external linkage is defined
*
* Reasoning: Interrupt declaration are provided by compiler and are available
* outside the driver folder
*/
void __attribute__ ( ( __interrupt__, weak ) ) _AD2CH0Interrupt ( void )
{
    uint32_t valADC2_Channel0;
    //Read the ADC value from the ADCBUF
    valADC2_Channel0 = AD2CH0DATA;

    if(NULL != ADC2_ChannelHandler)
    {
        (*ADC2_ChannelHandler)(ADC2_Channel0, valADC2_Channel0);
    }
    if(NULL != ADC2_Result32BitChannelHandler)
    {
        (*ADC2_Result32BitChannelHandler)(ADC2_Channel0, valADC2_Channel0);
    }

    //clear the ADC2_Channel0 interrupt flag
    IFS5bits.AD2CH0IF = 0U;
}

/* cppcheck-suppress misra-c2012-8.4
*
* (Rule 8.4) REQUIRED: A compatible declaration shall be visible when an object or 
* function with external linkage is defined
*
* Reasoning: Interrupt declaration are provided by compiler and are available
* outside the driver folder
*/
void __attribute__ ( ( __interrupt__, weak ) ) _AD2CH1Interrupt ( void )
{
    uint32_t valADC2_Channel1;
    //Read the ADC value from the ADCBUF
    valADC2_Channel1 = AD2CH1DATA;

    if(NULL != ADC2_ChannelHandler)
    {
        (*ADC2_ChannelHandler)(ADC2_Channel1, valADC2_Channel1);
    }
    if(NULL != ADC2_Result32BitChannelHandler)
    {
        (*ADC2_Result32BitChannelHandler)(ADC2_Channel1, valADC2_Channel1);
    }

    //clear the ADC2_Channel1 interrupt flag
    IFS5bits.AD2CH1IF = 0U;
}

/* cppcheck-suppress misra-c2012-8.4
*
* (Rule 8.4) REQUIRED: A compatible declaration shall be visible when an object or 
* function with external linkage is defined
*
* Reasoning: Interrupt declaration are provided by compiler and are available
* outside the driver folder
*/
void __attribute__ ( ( __interrupt__, weak ) ) _AD2CH2Interrupt ( void )
{
    uint32_t valADC2_Channel2;
    //Read the ADC value from the ADCBUF
    valADC2_Channel2 = AD2CH2DATA;

    if(NULL != ADC2_ChannelHandler)
    {
        (*ADC2_ChannelHandler)(ADC2_Channel2, valADC2_Channel2);
    }
    if(NULL != ADC2_Result32BitChannelHandler)
    {
        (*ADC2_Result32BitChannelHandler)(ADC2_Channel2, valADC2_Channel2);
    }

    //clear the ADC2_Channel2 interrupt flag
    IFS5bits.AD2CH2IF = 0U;
}


void __attribute__ ( ( weak ) ) ADC2_ChannelTasks (enum ADC2_CHANNEL channel)
{
    uint32_t adcVal;
    
    switch(channel)
    {   
        case ADC2_Channel0:
            if((bool)AD2STATbits.CH0RDY == 1U)
            {
                //Read the ADC value from the ADCBUF
                adcVal = AD2CH0DATA;

                if(NULL != ADC2_ChannelHandler)
                {
                    (*ADC2_ChannelHandler)(channel, adcVal);
                }
                if(NULL != ADC2_Result32BitChannelHandler)
                {
                    (*ADC2_Result32BitChannelHandler)(channel, adcVal);
                }
            }
            break;
        case ADC2_Channel1:
            if((bool)AD2STATbits.CH1RDY == 1U)
            {
                //Read the ADC value from the ADCBUF
                adcVal = AD2CH1DATA;

                if(NULL != ADC2_ChannelHandler)
                {
                    (*ADC2_ChannelHandler)(channel, adcVal);
                }
                if(NULL != ADC2_Result32BitChannelHandler)
                {
                    (*ADC2_Result32BitChannelHandler)(channel, adcVal);
                }
            }
            break;
        case ADC2_Channel2:
            if((bool)AD2STATbits.CH2RDY == 1U)
            {
                //Read the ADC value from the ADCBUF
                adcVal = AD2CH2DATA;

                if(NULL != ADC2_ChannelHandler)
                {
                    (*ADC2_ChannelHandler)(channel, adcVal);
                }
                if(NULL != ADC2_Result32BitChannelHandler)
                {
                    (*ADC2_Result32BitChannelHandler)(channel, adcVal);
                }
            }
            break;
        default:
            break;
    }            
}

void ADC2_ComparatorCallbackRegister(void(*callback)(enum ADC2_CMP comparator))
{
    if(NULL != callback)
    {
        ADC2_ComparatorHandler = callback;
    }
}

void __attribute__ ( ( weak ) ) ADC2_ComparatorCallback (enum ADC2_CMP comparator)
{ 
    (void)comparator;
} 



