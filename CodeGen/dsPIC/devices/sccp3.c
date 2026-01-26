/**
 * SCCP3 Generated Driver Source File
 * 
 * @file 	  sccp3.c
 * 
 * @ingroup   pwmdriver
 * 
 * @brief 	  This is the generated driver source file for SCCP3 driver
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
#include <sccp3.h>

// Section: File specific functions

static void (*SCCP3_PWMHandler)(void) = NULL;

// Section: Driver Interface

const struct PWM_INTERFACE PWM3 = {
    .Initialize          = &SCCP3_PWM_Initialize,
    .Deinitialize        = &SCCP3_PWM_Deinitialize,
    .Enable              = &SCCP3_PWM_Enable,
    .Disable             = &SCCP3_PWM_Disable,
    .PeriodSet           = &SCCP3_PWM_PeriodSet,
    .DutyCycleSet        = &SCCP3_PWM_DutyCycleSet,
    .SoftwareTriggerSet  = &SCCP3_PWM_SoftwareTriggerSet,
    .DeadTimeSet         = NULL,
    .OutputModeSet       = NULL,
    .CallbackRegister = &SCCP3_PWM_CallbackRegister,
    .Tasks               = &SCCP3_PWM_Tasks
};

// Section: SCCP3 Module APIs

void SCCP3_PWM_Initialize (void)
{
    // MOD Dual Edge Compare, Buffered(PWM); CCSEL disabled; T32 16 Bit; TMRPS 1:1; CLKSEL Standard Speed Peripheral Clock; TMRSYNC disabled; SIDL disabled; ON disabled; SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; OPS Each Time Base Period Match; RTRGEN disabled; OPSSRC Timer Interrupt Event; 
    CCP3CON1 = 0x5UL;
    // ASDG disabled; SSDG disabled; ASDGM disabled; PWMRSEN disabled; ICS ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN enabled; OENSYNC disabled; 
    CCP3CON2 = 0x1000000UL;
    // PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; PSSBDF Tri-state; POLBDF disabled; 
    CCP3CON3 = 0x0UL;
    // ICOV disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; RAWIP disabled; RBWIP disabled; TMRLWIP disabled; TMRHWIP disabled; PRLWIP disabled; 
    CCP3STAT = 0x0UL;
    // TMRL 0x0; TMRH 0x0; 
    CCP3TMR = 0x0UL;
    // PRL 5000; PRH 0; 
    CCP3PR = 0x1388UL;
    // BUFL 0x0; BUFH 0x0; 
    CCP3BUF = 0x0UL;
    // CMPA 0; 
    CCP3RA = 0x0UL;
    // CMPB 2500; 
    CCP3RB = 0x9C4UL;
    SCCP3_PWM_CallbackRegister(&SCCP3_PWM_Callback);
    
    CCP3CON1bits.ON = 1; //Enable Module
}

void SCCP3_PWM_Deinitialize (void)
{
    CCP3CON1bits.ON = 0;
    
    CCP3CON1 = 0x0UL;
    CCP3CON2 = 0x1000000UL;
    CCP3CON3 = 0x0UL;
    CCP3STAT = 0x0UL;
    CCP3TMR = 0x0UL;
    CCP3PR = 0xFFFFFFFFUL;
    CCP3BUF = 0x0UL;
    CCP3RA = 0x0UL;
    CCP3RB = 0x0UL;
}

void SCCP3_PWM_Enable( void )
{
    CCP3CON1bits.ON = 1;
}


void SCCP3_PWM_Disable( void )
{
    CCP3CON1bits.ON = 0;
}

void SCCP3_PWM_PeriodSet(size_t periodCount)
{
    CCP3PR = periodCount;
}

void SCCP3_PWM_DutyCycleSet(size_t dutyCycleCount)
{
    CCP3RB = dutyCycleCount;
}

void SCCP3_PWM_SoftwareTriggerSet( void )
{
    CCP3STATbits.TRSET = 1;
}

void SCCP3_PWM_CallbackRegister(void (*handler)(void))
{
    if(NULL != handler)
    {
        SCCP3_PWMHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP3_PWM_Callback ( void )
{ 

} 


void SCCP3_PWM_Tasks( void )
{    
    if(IFS1bits.CCT3IF == 1)
    {
        // SCCP3 callback function 
        if(NULL != SCCP3_PWMHandler)
        {
            (*SCCP3_PWMHandler)();
        }
        IFS1bits.CCT3IF = 0;
    }
}
/**
 End of File
*/
