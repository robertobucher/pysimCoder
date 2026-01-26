/**
 * SCCP2 Generated Driver Source File
 * 
 * @file 	  sccp2.c
 * 
 * @ingroup   pwmdriver
 * 
 * @brief 	  This is the generated driver source file for SCCP2 driver
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
#include <sccp2.h>

// Section: File specific functions

static void (*SCCP2_PWMHandler)(void) = NULL;

// Section: Driver Interface

const struct PWM_INTERFACE PWM2 = {
    .Initialize          = &SCCP2_PWM_Initialize,
    .Deinitialize        = &SCCP2_PWM_Deinitialize,
    .Enable              = &SCCP2_PWM_Enable,
    .Disable             = &SCCP2_PWM_Disable,
    .PeriodSet           = &SCCP2_PWM_PeriodSet,
    .DutyCycleSet        = &SCCP2_PWM_DutyCycleSet,
    .SoftwareTriggerSet  = &SCCP2_PWM_SoftwareTriggerSet,
    .DeadTimeSet         = NULL,
    .OutputModeSet       = NULL,
    .CallbackRegister = &SCCP2_PWM_CallbackRegister,
    .Tasks               = &SCCP2_PWM_Tasks
};

// Section: SCCP2 Module APIs

void SCCP2_PWM_Initialize (void)
{
    // MOD Dual Edge Compare, Buffered(PWM); CCSEL disabled; T32 16 Bit; TMRPS 1:1; CLKSEL Standard Speed Peripheral Clock; TMRSYNC disabled; SIDL disabled; ON disabled; SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; OPS Each Time Base Period Match; RTRGEN disabled; OPSSRC Timer Interrupt Event; 
    CCP2CON1 = 0x5UL;
    // ASDG disabled; SSDG disabled; ASDGM disabled; PWMRSEN disabled; ICS ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN enabled; OENSYNC disabled; 
    CCP2CON2 = 0x1000000UL;
    // PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; PSSBDF Tri-state; POLBDF disabled; 
    CCP2CON3 = 0x0UL;
    // ICOV disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; RAWIP disabled; RBWIP disabled; TMRLWIP disabled; TMRHWIP disabled; PRLWIP disabled; 
    CCP2STAT = 0x0UL;
    // TMRL 0x0; TMRH 0x0; 
    CCP2TMR = 0x0UL;
    // PRL 5000; PRH 0; 
    CCP2PR = 0x1388UL;
    // BUFL 0x0; BUFH 0x0; 
    CCP2BUF = 0x0UL;
    // CMPA 0; 
    CCP2RA = 0x0UL;
    // CMPB 2500; 
    CCP2RB = 0x9C4UL;
    SCCP2_PWM_CallbackRegister(&SCCP2_PWM_Callback);
    
    CCP2CON1bits.ON = 1; //Enable Module
}

void SCCP2_PWM_Deinitialize (void)
{
    CCP2CON1bits.ON = 0;
    
    CCP2CON1 = 0x0UL;
    CCP2CON2 = 0x1000000UL;
    CCP2CON3 = 0x0UL;
    CCP2STAT = 0x0UL;
    CCP2TMR = 0x0UL;
    CCP2PR = 0xFFFFFFFFUL;
    CCP2BUF = 0x0UL;
    CCP2RA = 0x0UL;
    CCP2RB = 0x0UL;
}

void SCCP2_PWM_Enable( void )
{
    CCP2CON1bits.ON = 1;
}


void SCCP2_PWM_Disable( void )
{
    CCP2CON1bits.ON = 0;
}

void SCCP2_PWM_PeriodSet(size_t periodCount)
{
    CCP2PR = periodCount;
}

void SCCP2_PWM_DutyCycleSet(size_t dutyCycleCount)
{
    CCP2RB = dutyCycleCount;
}

void SCCP2_PWM_SoftwareTriggerSet( void )
{
    CCP2STATbits.TRSET = 1;
}

void SCCP2_PWM_CallbackRegister(void (*handler)(void))
{
    if(NULL != handler)
    {
        SCCP2_PWMHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP2_PWM_Callback ( void )
{ 

} 


void SCCP2_PWM_Tasks( void )
{    
    if(IFS1bits.CCT2IF == 1)
    {
        // SCCP2 callback function 
        if(NULL != SCCP2_PWMHandler)
        {
            (*SCCP2_PWMHandler)();
        }
        IFS1bits.CCT2IF = 0;
    }
}
/**
 End of File
*/
