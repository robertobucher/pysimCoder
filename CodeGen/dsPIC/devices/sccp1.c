/**
 * SCCP1 Generated Driver Source File
 * 
 * @file 	  sccp1.c
 * 
 * @ingroup   pwmdriver
 * 
 * @brief 	  This is the generated driver source file for SCCP1 driver
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

#include <xc.h>
#include <stddef.h>
#include <sccp1.h>

// Section: File specific functions

static void (*SCCP1_PWMHandler)(void) = NULL;

// Section: Driver Interface

const struct PWM_INTERFACE PWM1 = {
    .Initialize          = &SCCP1_PWM_Initialize,
    .Deinitialize        = &SCCP1_PWM_Deinitialize,
    .Enable              = &SCCP1_PWM_Enable,
    .Disable             = &SCCP1_PWM_Disable,
    .PeriodSet           = &SCCP1_PWM_PeriodSet,
    .DutyCycleSet        = &SCCP1_PWM_DutyCycleSet,
    .SoftwareTriggerSet  = &SCCP1_PWM_SoftwareTriggerSet,
    .DeadTimeSet         = NULL,
    .OutputModeSet       = NULL,
    .CallbackRegister = &SCCP1_PWM_CallbackRegister,
    .Tasks               = &SCCP1_PWM_Tasks
};

// Section: SCCP1 Module APIs

void SCCP1_PWM_Initialize (void)
{
    // MOD Dual Edge Compare, Buffered(PWM); CCSEL disabled; T32 16 Bit; TMRPS 1:1; CLKSEL Standard Speed Peripheral Clock; TMRSYNC disabled; SIDL disabled; ON disabled; SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; OPS Each Time Base Period Match; RTRGEN disabled; OPSSRC Timer Interrupt Event; 
    CCP1CON1 = 0x5UL;
    // ASDG disabled; SSDG disabled; ASDGM disabled; PWMRSEN disabled; ICS ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN enabled; OENSYNC disabled; 
    CCP1CON2 = 0x1000000UL;
    // PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; PSSBDF Tri-state; POLBDF disabled; 
    CCP1CON3 = 0x0UL;
    // ICOV disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; RAWIP disabled; RBWIP disabled; TMRLWIP disabled; TMRHWIP disabled; PRLWIP disabled; 
    CCP1STAT = 0x0UL;
    // TMRL 0x0; TMRH 0x0; 
    CCP1TMR = 0x0UL;
    // PRL 5000; PRH 0; 
    CCP1PR = 0x1388UL;
    // BUFL 0x0; BUFH 0x0; 
    CCP1BUF = 0x0UL;
    // CMPA 0; 
    CCP1RA = 0x0UL;
    // CMPB 2500; 
    CCP1RB = 0x9C4UL;
    SCCP1_PWM_CallbackRegister(&SCCP1_PWM_Callback);
    
    CCP1CON1bits.ON = 1; //Enable Module
}

void SCCP1_PWM_Deinitialize (void)
{
    CCP1CON1bits.ON = 0;
    
    CCP1CON1 = 0x0UL;
    CCP1CON2 = 0x1000000UL;
    CCP1CON3 = 0x0UL;
    CCP1STAT = 0x0UL;
    CCP1TMR = 0x0UL;
    CCP1PR = 0xFFFFFFFFUL;
    CCP1BUF = 0x0UL;
    CCP1RA = 0x0UL;
    CCP1RB = 0x0UL;
}

void SCCP1_PWM_Enable( void )
{
    CCP1CON1bits.ON = 1;
}


void SCCP1_PWM_Disable( void )
{
    CCP1CON1bits.ON = 0;
}

void SCCP1_PWM_PeriodSet(size_t periodCount)
{
    CCP1PR = periodCount;
}

void SCCP1_PWM_DutyCycleSet(size_t dutyCycleCount)
{
    CCP1RB = dutyCycleCount;
}

void SCCP1_PWM_SoftwareTriggerSet( void )
{
    CCP1STATbits.TRSET = 1;
}

void SCCP1_PWM_CallbackRegister(void (*handler)(void))
{
    if(NULL != handler)
    {
        SCCP1_PWMHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP1_PWM_Callback ( void )
{ 

} 


void SCCP1_PWM_Tasks( void )
{    
    if(IFS1bits.CCT1IF == 1)
    {
        // SCCP1 callback function 
        if(NULL != SCCP1_PWMHandler)
        {
            (*SCCP1_PWMHandler)();
        }
        IFS1bits.CCT1IF = 0;
    }
}
/**
 End of File
*/
