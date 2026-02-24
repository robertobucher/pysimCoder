/**
 * ADC1 Generated Driver Source File
 * 
 * @file      adc1.c
 *            
 * @ingroup   adcdriver
 *            
 * @brief     This is the generated driver source file for ADC1 driver        
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
#include <adc1.h>

// Section: File specific functions

static void (*ADC1_ChannelHandler)(enum ADC1_CHANNEL channel, uint16_t adcVal) = NULL;
static void (*ADC1_Result32BitChannelHandler)(enum ADC1_CHANNEL channel, uint32_t adcVal) = NULL;
static void (*ADC1_ComparatorHandler)(enum ADC1_CMP comparator) = NULL;

// Section: File specific data type definitions

/**
 @ingroup  adcdriver
 @enum     ADC1_PWM_TRIG_SRCS
 @brief    Defines the PWM ADC TRIGGER sources available for the module to use.
*/
enum ADC1_PWM_TRIG_SRCS {
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
static const struct ADC_MULTICORE adc1Multicore = {
    .ChannelSoftwareTriggerEnable           = &ADC1_ChannelSoftwareTriggerEnable,
    .ChannelSoftwareTriggerDisable          = &ADC1_ChannelSoftwareTriggerDisable,
    .SampleCountGet                         = NULL,
    .SampleCountStatusGet                   = NULL,
    .ChannelTasks                           = &ADC1_ChannelTasks, 
    .ComparatorTasks                        = NULL,
    .IndividualChannelInterruptEnable       = &ADC1_IndividualChannelInterruptEnable,
    .IndividualChannelInterruptDisable      = &ADC1_IndividualChannelInterruptDisable,
    .IndividualChannelInterruptFlagClear    = &ADC1_IndividualChannelInterruptFlagClear,
    .IndividualChannelInterruptPrioritySet  = &ADC1_IndividualChannelInterruptPrioritySet,
    .ChannelCallbackRegister                = &ADC1_ChannelCallbackRegister,
    .Result32BitChannelCallbackRegister     = &ADC1_Result32BitChannelCallbackRegister,
    .ComparatorCallbackRegister             = &ADC1_ComparatorCallbackRegister,
    .CorePowerEnable                        = NULL,
    .SharedCorePowerEnable                  = &ADC1_SharedCorePowerEnable,
    .CoreCalibration                        = NULL,
    .SharedCoreCalibration                  = &ADC1_SharedCoreCalibration,
    .PWMTriggerSourceSet                    = &ADC1_PWMTriggerSourceSet
};

//Defines an object for ADC_INTERFACE.

const struct ADC_INTERFACE ADC1 = {
    .Initialize             = &ADC1_Initialize,
    .Deinitialize           = &ADC1_Deinitialize,
    .Enable                 = &ADC1_Enable,
    .IsReady                = &ADC1_IsReady,
    .Disable                = &ADC1_Disable,
    .SoftwareTriggerEnable  = &ADC1_SoftwareTriggerEnable,
    .SoftwareTriggerDisable = &ADC1_SoftwareTriggerDisable,
    .ChannelSelect          = NULL, 
    .ConversionResultGet    = &ADC1_ConversionResultGet,
    .SecondaryAccumulatorGet= NULL,
    .IsConversionComplete   = &ADC1_IsConversionComplete,
    .ResolutionSet          = NULL,
    .InterruptEnable        = NULL,
    .InterruptDisable       = NULL,
    .InterruptFlagClear     = NULL,
    .InterruptPrioritySet   = NULL,
    .CommonCallbackRegister = NULL,
    .Tasks                  = NULL,
    .adcMulticoreInterface = &adc1Multicore,
};

// Section: Driver Interface Function Definitions

void ADC1_Initialize(void)
{
    //CALCNT Wait for 2 activity free ADC clock cycles; BUFEN disabled; TSTEN disabled; ON enabled; STNDBY disabled; RPTCNT 1 ADC clock cycles between triggers; CALRATE Every second; ACALEN disabled; CALREQ Calibration cycle is not requested; 
    AD1CON = (uint32_t)0x8000UL & ~_AD1CON_ON_MASK;
    //DATAOVR 0x0; 
    AD1DATAOVR = 0x0UL;
    //CH0RDY disabled; CH1RDY disabled; CH2RDY disabled; CH3RDY disabled; CH4RDY disabled; CH5RDY disabled; CH6RDY disabled; CH7RDY disabled; 
    AD1STAT = 0x0UL;
    //CH0RRDY disabled; CH1RRDY disabled; CH2RRDY disabled; CH3RRDY disabled; CH4RRDY disabled; CH5RRDY disabled; CH6RRDY disabled; CH7RRDY disabled; 
    AD1RSTAT = 0x0UL;
    //CH0CMP disabled; CH1CMP disabled; CH2CMP disabled; CH3CMP disabled; CH4CMP disabled; CH5CMP disabled; CH6CMP disabled; CH7CMP disabled; 
    AD1CMPSTAT = 0x0UL;
    //CH0TRG disabled; CH1TRG disabled; CH2TRG disabled; CH3TRG disabled; CH4TRG disabled; CH5TRG disabled; CH6TRG disabled; CH7TRG disabled; 
    AD1SWTRG = 0x0UL;
    //TRG1SRC PWM1 Trigger1; MODE Single sample initiated by TRG1SRC[4:0] trigger; TRG2SRC Triggers are disabled; ACCNUM 4 samples, 13 bits result; SAMC 0.5 TAD; IRQSEL enabled; EIEN disabled; TRG1POL disabled; PINSEL AD1AN0; NINSEL disabled; FRAC Integer; DIFF disabled; 
    AD1CH0CON1 = 0x200004UL;
    //TRG1SRC PWM1 Trigger1; MODE Single sample initiated by TRG1SRC[4:0] trigger; TRG2SRC Triggers are disabled; ACCNUM 4 samples, 13 bits result; SAMC 0.5 TAD; IRQSEL enabled; EIEN disabled; TRG1POL disabled; PINSEL AD1AN1; NINSEL disabled; FRAC Integer; DIFF disabled; 
    AD1CH1CON1 = 0x1200004UL;
    //TRG1SRC PWM1 Trigger1; MODE Single sample initiated by TRG1SRC[4:0] trigger; TRG2SRC Triggers are disabled; ACCNUM 4 samples, 13 bits result; SAMC 0.5 TAD; IRQSEL enabled; EIEN disabled; TRG1POL disabled; PINSEL AD1AN2; NINSEL disabled; FRAC Integer; DIFF disabled; 
    AD1CH2CON1 = 0x2200004UL;
    //TRG1SRC PWM1 Trigger1; MODE Single sample initiated by TRG1SRC[4:0] trigger; TRG2SRC Triggers are disabled; ACCNUM 4 samples, 13 bits result; SAMC 0.5 TAD; IRQSEL enabled; EIEN disabled; TRG1POL disabled; PINSEL AD1AN3; NINSEL disabled; FRAC Integer; DIFF disabled; 
    AD1CH3CON1 = 0x3200004UL;
    //ADCMPCNT disabled; CMPMOD NONE; CMPCNTMOD disabled; CMPVAL enabled; ACCBRST disabled; ACCRO disabled; 
    AD1CH0CON2 = 0x20000000UL;
    //ADCMPCNT disabled; CMPMOD NONE; CMPCNTMOD disabled; CMPVAL enabled; ACCBRST disabled; ACCRO disabled; 
    AD1CH1CON2 = 0x20000000UL;
    //ADCMPCNT disabled; CMPMOD NONE; CMPCNTMOD disabled; CMPVAL enabled; ACCBRST disabled; ACCRO disabled; 
    AD1CH2CON2 = 0x20000000UL;
    //ADCMPCNT disabled; CMPMOD NONE; CMPCNTMOD disabled; CMPVAL enabled; ACCBRST disabled; ACCRO disabled; 
    AD1CH3CON2 = 0x20000000UL;
    //
    AD1CH0RES = 0x0UL;
    //
    AD1CH1RES = 0x0UL;
    //
    AD1CH2RES = 0x0UL;
    //
    AD1CH3RES = 0x0UL;
    //CNT 0x0; 
    AD1CH0CNT = 0x0UL;
    //CNT 0x0; 
    AD1CH1CNT = 0x0UL;
    //CNT 0x0; 
    AD1CH2CNT = 0x0UL;
    //CNT 0x0; 
    AD1CH3CNT = 0x0UL;
    //CMPLO 0x0; 
    AD1CH0CMPLO = 0x0UL;
    //CMPLO 0x0; 
    AD1CH1CMPLO = 0x0UL;
    //CMPLO 0x0; 
    AD1CH2CMPLO = 0x0UL;
    //CMPLO 0x0; 
    AD1CH3CMPLO = 0x0UL;
    //CMPHI 0x0; 
    AD1CH0CMPHI = 0x0UL;
    //CMPHI 0x0; 
    AD1CH1CMPHI = 0x0UL;
    //CMPHI 0x0; 
    AD1CH2CMPHI = 0x0UL;
    //CMPHI 0x0; 
    AD1CH3CMPHI = 0x0UL;

    ADC1_ChannelCallbackRegister(&ADC1_ChannelCallback);
    ADC1_Result32BitChannelCallbackRegister(&ADC1_Result32BitChannelCallback);
    ADC1_ComparatorCallbackRegister(&ADC1_ComparatorCallback);
    
    
    // ADC Mode change to run mode
    ADC1_SharedCorePowerEnable();
}

void ADC1_Deinitialize (void)
{
    AD1CONbits.ON = 0U;
    
    AD1CON = 0x480000UL;
    AD1DATAOVR = 0x0UL;
    AD1STAT = 0x0UL;
    AD1RSTAT = 0x0UL;
    AD1CMPSTAT = 0x0UL;
    AD1SWTRG = 0x0UL;
    AD1CH0CON1 = 0x0UL;
    AD1CH1CON1 = 0x0UL;
    AD1CH2CON1 = 0x0UL;
    AD1CH3CON1 = 0x0UL;
    AD1CH0CON2 = 0x1UL;
    AD1CH1CON2 = 0x1UL;
    AD1CH2CON2 = 0x1UL;
    AD1CH3CON2 = 0x1UL;
    AD1CH0RES = 0x0UL;
    AD1CH1RES = 0x0UL;
    AD1CH2RES = 0x0UL;
    AD1CH3RES = 0x0UL;
    AD1CH0CNT = 0x0UL;
    AD1CH1CNT = 0x0UL;
    AD1CH2CNT = 0x0UL;
    AD1CH3CNT = 0x0UL;
    AD1CH0CMPLO = 0x0UL;
    AD1CH1CMPLO = 0x0UL;
    AD1CH2CMPLO = 0x0UL;
    AD1CH3CMPLO = 0x0UL;
    AD1CH0CMPHI = 0x0UL;
    AD1CH1CMPHI = 0x0UL;
    AD1CH2CMPHI = 0x0UL;
    AD1CH3CMPHI = 0x0UL;
}

void ADC1_SharedCorePowerEnable (void) 
{
    AD1CONbits.ON = 1U;   
    while(AD1CONbits.ADRDY == 0U)
    {
    }
}

void ADC1_SharedCoreCalibration (void) 
{
    AD1CONbits.CALREQ = 1U;    
    while(AD1CONbits.CALRDY == 0U)
    {
    }  
}

static uint16_t ADC1_TriggerSourceValueGet(enum ADC_PWM_INSTANCE pwmInstance, enum ADC_PWM_TRIGGERS triggerNumber)
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

void ADC1_PWMTriggerSourceSet(enum ADC1_CHANNEL channel, enum ADC_PWM_INSTANCE pwmInstance, enum ADC_PWM_TRIGGERS triggerNumber)
{
    uint16_t adcTriggerValue;
    adcTriggerValue= ADC1_TriggerSourceValueGet(pwmInstance, triggerNumber);
    switch(channel)
    {
        case ADC1_Channel0:
                AD1CH0CON1bits.TRG1SRC = adcTriggerValue;
                break;
        case ADC1_Channel1:
                AD1CH1CON1bits.TRG1SRC = adcTriggerValue;
                break;
        case ADC1_Channel2:
                AD1CH2CON1bits.TRG1SRC = adcTriggerValue;
                break;
        case ADC1_Channel3:
                AD1CH3CON1bits.TRG1SRC = adcTriggerValue;
                break;
        default:
                break;
    }
}

void ADC1_ChannelCallbackRegister(void(*callback)(enum ADC1_CHANNEL channel, uint16_t adcVal))
{
    if(NULL != callback)
    {
        ADC1_ChannelHandler = callback;
    }
}

void __attribute__ ( ( weak ) ) ADC1_ChannelCallback (enum ADC1_CHANNEL channel, uint16_t adcVal)
{ 
    (void)channel;
    (void)adcVal;
} 

void ADC1_Result32BitChannelCallbackRegister(void(*callback)(enum ADC1_CHANNEL channel, uint32_t adcVal))
{
    if(NULL != callback)
    {
        ADC1_Result32BitChannelHandler = callback;
    }
}

void __attribute__ ( ( weak ) ) ADC1_Result32BitChannelCallback (enum ADC1_CHANNEL channel, uint32_t adcVal)
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
void __attribute__ ( ( __interrupt__, weak ) ) _AD1CH0Interrupt ( void )
{
    uint32_t valADC1_Channel0;
    //Read the ADC value from the ADCBUF
    valADC1_Channel0 = AD1CH0DATA;

    if(NULL != ADC1_ChannelHandler)
    {
        (*ADC1_ChannelHandler)(ADC1_Channel0, valADC1_Channel0);
    }
    if(NULL != ADC1_Result32BitChannelHandler)
    {
        (*ADC1_Result32BitChannelHandler)(ADC1_Channel0, valADC1_Channel0);
    }

    //clear the ADC1_Channel0 interrupt flag
    IFS4bits.AD1CH0IF = 0U;
}

/* cppcheck-suppress misra-c2012-8.4
*
* (Rule 8.4) REQUIRED: A compatible declaration shall be visible when an object or 
* function with external linkage is defined
*
* Reasoning: Interrupt declaration are provided by compiler and are available
* outside the driver folder
*/
void __attribute__ ( ( __interrupt__, weak ) ) _AD1CH1Interrupt ( void )
{
    uint32_t valADC1_Channel1;
    //Read the ADC value from the ADCBUF
    valADC1_Channel1 = AD1CH1DATA;

    if(NULL != ADC1_ChannelHandler)
    {
        (*ADC1_ChannelHandler)(ADC1_Channel1, valADC1_Channel1);
    }
    if(NULL != ADC1_Result32BitChannelHandler)
    {
        (*ADC1_Result32BitChannelHandler)(ADC1_Channel1, valADC1_Channel1);
    }

    //clear the ADC1_Channel1 interrupt flag
    IFS4bits.AD1CH1IF = 0U;
}

/* cppcheck-suppress misra-c2012-8.4
*
* (Rule 8.4) REQUIRED: A compatible declaration shall be visible when an object or 
* function with external linkage is defined
*
* Reasoning: Interrupt declaration are provided by compiler and are available
* outside the driver folder
*/
void __attribute__ ( ( __interrupt__, weak ) ) _AD1CH2Interrupt ( void )
{
    uint32_t valADC1_Channel2;
    //Read the ADC value from the ADCBUF
    valADC1_Channel2 = AD1CH2DATA;

    if(NULL != ADC1_ChannelHandler)
    {
        (*ADC1_ChannelHandler)(ADC1_Channel2, valADC1_Channel2);
    }
    if(NULL != ADC1_Result32BitChannelHandler)
    {
        (*ADC1_Result32BitChannelHandler)(ADC1_Channel2, valADC1_Channel2);
    }

    //clear the ADC1_Channel2 interrupt flag
    IFS5bits.AD1CH2IF = 0U;
}

/* cppcheck-suppress misra-c2012-8.4
*
* (Rule 8.4) REQUIRED: A compatible declaration shall be visible when an object or 
* function with external linkage is defined
*
* Reasoning: Interrupt declaration are provided by compiler and are available
* outside the driver folder
*/
void __attribute__ ( ( __interrupt__, weak ) ) _AD1CH3Interrupt ( void )
{
    uint32_t valADC1_Channel3;
    //Read the ADC value from the ADCBUF
    valADC1_Channel3 = AD1CH3DATA;

    if(NULL != ADC1_ChannelHandler)
    {
        (*ADC1_ChannelHandler)(ADC1_Channel3, valADC1_Channel3);
    }
    if(NULL != ADC1_Result32BitChannelHandler)
    {
        (*ADC1_Result32BitChannelHandler)(ADC1_Channel3, valADC1_Channel3);
    }

    //clear the ADC1_Channel3 interrupt flag
    IFS5bits.AD1CH3IF = 0U;
}


void __attribute__ ( ( weak ) ) ADC1_ChannelTasks (enum ADC1_CHANNEL channel)
{
    uint32_t adcVal;
    
    switch(channel)
    {   
        case ADC1_Channel0:
            if((bool)AD1STATbits.CH0RDY == 1U)
            {
                //Read the ADC value from the ADCBUF
                adcVal = AD1CH0DATA;

                if(NULL != ADC1_ChannelHandler)
                {
                    (*ADC1_ChannelHandler)(channel, adcVal);
                }
                if(NULL != ADC1_Result32BitChannelHandler)
                {
                    (*ADC1_Result32BitChannelHandler)(channel, adcVal);
                }
            }
            break;
        case ADC1_Channel1:
            if((bool)AD1STATbits.CH1RDY == 1U)
            {
                //Read the ADC value from the ADCBUF
                adcVal = AD1CH1DATA;

                if(NULL != ADC1_ChannelHandler)
                {
                    (*ADC1_ChannelHandler)(channel, adcVal);
                }
                if(NULL != ADC1_Result32BitChannelHandler)
                {
                    (*ADC1_Result32BitChannelHandler)(channel, adcVal);
                }
            }
            break;
        case ADC1_Channel2:
            if((bool)AD1STATbits.CH2RDY == 1U)
            {
                //Read the ADC value from the ADCBUF
                adcVal = AD1CH2DATA;

                if(NULL != ADC1_ChannelHandler)
                {
                    (*ADC1_ChannelHandler)(channel, adcVal);
                }
                if(NULL != ADC1_Result32BitChannelHandler)
                {
                    (*ADC1_Result32BitChannelHandler)(channel, adcVal);
                }
            }
            break;
        case ADC1_Channel3:
            if((bool)AD1STATbits.CH3RDY == 1U)
            {
                //Read the ADC value from the ADCBUF
                adcVal = AD1CH3DATA;

                if(NULL != ADC1_ChannelHandler)
                {
                    (*ADC1_ChannelHandler)(channel, adcVal);
                }
                if(NULL != ADC1_Result32BitChannelHandler)
                {
                    (*ADC1_Result32BitChannelHandler)(channel, adcVal);
                }
            }
            break;
        default:
            break;
    }            
}

void ADC1_ComparatorCallbackRegister(void(*callback)(enum ADC1_CMP comparator))
{
    if(NULL != callback)
    {
        ADC1_ComparatorHandler = callback;
    }
}

void __attribute__ ( ( weak ) ) ADC1_ComparatorCallback (enum ADC1_CMP comparator)
{ 
    (void)comparator;
} 



