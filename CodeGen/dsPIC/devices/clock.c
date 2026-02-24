
/**
 * CLOCK Generated Driver Source File 
 * 
 * @file      clock.c
 *            
 * @ingroup   clockdriver
 *            
 * @brief     This is the generated source file for CLOCK driver
 *
 * @version   PLIB Version 1.3.1
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

// Section: Includes
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <clock.h>
#include <clock_types.h>

#define PLL1FOUT_SOURCE         0x5U
#define PLL2VCODIV_SOURCE       0x8U 

// Section: Static Variables


void CLOCK_Initialize(void)
{
    /*  
        System Clock Source                             :  PLL1 Out output
        System/Generator 1 frequency (Fosc)             :  100.02442 MHz
        
        Clock Generator 2 frequency                     : 8 MHz
        Clock Generator 3 frequency                     : 8 MHz
        Clock Generator 6 frequency                     : 100.02442 MHz
        
        PLL 1 frequency                                 : 320 MHz
        PLL 1 VCO Out frequency                         : 800 MHz
        PLL 2 frequency                                 : 200 MHz
        PLL 2 VCO Out frequency                         : 166.666667 MHz

    */
    OSCCFGbits.POSCMD = 0x1U;
    
    OSCCTRLbits.POSCEN = 1U;
#ifndef __MPLAB_DEBUGGER_SIMULATOR
    while(OSCCTRLbits.POSCRDY == 0U){};
#endif
    
    //If CLK GEN 1 (system clock) is using a PLL, switch to FRC to avoid risk of over-clocking the CPU while changing PLL settings
    if((CLK1CONbits.COSC >= PLL1FOUT_SOURCE) && (CLK1CONbits.COSC <= PLL2VCODIV_SOURCE))
    {
        CLK1CONbits.NOSC = 1U; //FRC as source 
        CLK1CONbits.OSWEN = 1U;
#ifndef __MPLAB_DEBUGGER_SIMULATOR
        while(CLK1CONbits.OSWEN == 1U){};
#endif
    }
    
    // NOSC Primary Oscillator; OE enabled; SIDL disabled; ON enabled; BOSC Serial Test Mode clock (PGC); FSCMEN disabled; DIVSWEN disabled; OSWEN disabled; EXTCFSEL disabled; EXTCFEN disabled; FOUTSWEN disabled; RIS disabled; PLLSWEN disabled; 
    PLL1CON = 0x9300UL;
    // POSTDIV2 1x divide; POSTDIV1 5x divide; PLLFBDIV 200; PLLPRE 1; 
    PLL1DIV = 0x100C829UL;
    //Enable PLL Input and Feedback Divider update
    PLL1CONbits.PLLSWEN = 1U;
#ifndef __MPLAB_DEBUGGER_SIMULATOR
    while (PLL1CONbits.PLLSWEN == 1){};
#endif
    PLL1CONbits.FOUTSWEN = 1U;
#ifndef __MPLAB_DEBUGGER_SIMULATOR
    while (PLL1CONbits.FOUTSWEN == 1U){};
#endif
    //enable clock switching
    PLL1CONbits.OSWEN = 1U; 
#ifndef __MPLAB_DEBUGGER_SIMULATOR 
    //wait for switching
    while(PLL1CONbits.OSWEN == 1U){}; 
    //wait for clock to be ready
    while(OSCCTRLbits.PLL1RDY == 0U){};    
#endif
    
    //Configure VCO Divider
    // INTDIV 1; 
    VCO1DIV = 0x10000UL;
    //enable PLL VCO divider
    PLL1CONbits.DIVSWEN = 1U;
#ifndef __MPLAB_DEBUGGER_SIMULATOR     
    //wait for setup complete
    while(PLL1CONbits.DIVSWEN == 1U){}; 
#endif
    //Clearing ON shuts down oscillator when no downstream clkgen or peripheral is requesting the clock
    PLL1CONbits.ON = 0U;
    // NOSC FRC Oscillator; OE enabled; SIDL disabled; ON enabled; BOSC Serial Test Mode clock (PGC); FSCMEN disabled; DIVSWEN disabled; OSWEN disabled; EXTCFSEL disabled; EXTCFEN disabled; FOUTSWEN disabled; RIS disabled; PLLSWEN disabled; 
    PLL2CON = 0x9100UL;
    // POSTDIV2 1x divide; POSTDIV1 5x divide; PLLFBDIV 125; PLLPRE 1; 
    PLL2DIV = 0x1007D29UL;
    //Enable PLL Input and Feedback Divider update
    PLL2CONbits.PLLSWEN = 1U;
#ifndef __MPLAB_DEBUGGER_SIMULATOR
    while (PLL2CONbits.PLLSWEN == 1){};
#endif
    PLL2CONbits.FOUTSWEN = 1U;
#ifndef __MPLAB_DEBUGGER_SIMULATOR
    while (PLL2CONbits.FOUTSWEN == 1U){};
#endif
    //enable clock switching
    PLL2CONbits.OSWEN = 1U; 
#ifndef __MPLAB_DEBUGGER_SIMULATOR 
    //wait for switching
    while(PLL2CONbits.OSWEN == 1U){}; 
    //wait for clock to be ready
    while(OSCCTRLbits.PLL2RDY == 0U){};    
#endif
    
    //Configure VCO Divider
    // INTDIV 3; 
    VCO2DIV = 0x30000UL;
    //enable PLL VCO divider
    PLL2CONbits.DIVSWEN = 1U;
#ifndef __MPLAB_DEBUGGER_SIMULATOR     
    //wait for setup complete
    while(PLL2CONbits.DIVSWEN == 1U){}; 
#endif
    //Clearing ON shuts down oscillator when no downstream clkgen or peripheral is requesting the clock
    PLL2CONbits.ON = 0U;
    
    // NOSC PLL1 Out output; OE enabled; SIDL disabled; ON enabled; BOSC Backup FRC Oscillator; FSCMEN disabled; DIVSWEN disabled; OSWEN disabled; EXTCFSEL External clock fail detection module #1; EXTCFEN disabled; RIS disabled; SLEEPDLY 8 sys_clk delay; 
    CLK1CON = 0x29580UL;
    // FRACDIV 307; INTDIV 1; 
    CLK1DIV = 0x19980UL;
    //enable divide factors
    CLK1CONbits.DIVSWEN = 1U; 
    //wait for divide factors to get updated
#ifndef __MPLAB_DEBUGGER_SIMULATOR 
    while(CLK1CONbits.DIVSWEN == 1U){};
#endif
    //enable clock switching
    CLK1CONbits.OSWEN = 1U;
#ifndef __MPLAB_DEBUGGER_SIMULATOR    
    //wait for clock switching complete
    while(CLK1CONbits.OSWEN == 1U){};
#endif
    
    // NOSC FRC Oscillator; OE enabled; SIDL disabled; ON enabled; BOSC Backup FRC Oscillator; FSCMEN disabled; DIVSWEN disabled; OSWEN disabled; EXTCFSEL External clock fail detection module #1; EXTCFEN disabled; RIS disabled; SLEEPDLY 8 sys_clk delay; 
    CLK2CON = 0x29181UL;
    //enable clock switching
    CLK2CONbits.OSWEN = 1U;
#ifndef __MPLAB_DEBUGGER_SIMULATOR    
    //wait for clock switching complete
    while(CLK2CONbits.OSWEN == 1U){};
#endif
    
    // NOSC Backup FRC Oscillator; OE enabled; SIDL disabled; ON enabled; BOSC FRC Oscillator; FSCMEN disabled; DIVSWEN disabled; OSWEN disabled; EXTCFSEL External clock fail detection module #1; EXTCFEN disabled; RIS disabled; SLEEPDLY 8 sys_clk delay; 
    CLK3CON = 0x19282UL;
    //enable clock switching
    CLK3CONbits.OSWEN = 1U;
#ifndef __MPLAB_DEBUGGER_SIMULATOR    
    //wait for clock switching complete
    while(CLK3CONbits.OSWEN == 1U){};
#endif
    
    // NOSC PLL1 Out output; OE enabled; SIDL disabled; ON enabled; BOSC Backup FRC Oscillator; FSCMEN disabled; DIVSWEN disabled; OSWEN disabled; EXTCFSEL External clock fail detection module #1; EXTCFEN disabled; RIS disabled; SLEEPDLY 8 sys_clk delay; 
    CLK6CON = 0x29580UL;
    // FRACDIV 307; INTDIV 1; 
    CLK6DIV = 0x19980UL;
    //enable divide factors
    CLK6CONbits.DIVSWEN = 1U; 
    //wait for divide factors to get updated
#ifndef __MPLAB_DEBUGGER_SIMULATOR 
    while(CLK6CONbits.DIVSWEN == 1U){};
#endif
    //enable clock switching
    CLK6CONbits.OSWEN = 1U;
#ifndef __MPLAB_DEBUGGER_SIMULATOR    
    //wait for clock switching complete
    while(CLK6CONbits.OSWEN == 1U){};
#endif
    
    
    
}





