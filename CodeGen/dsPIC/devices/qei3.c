/**
 * QEI3 Generated Driver Source File 
 * 
 * @file      qei3.c
 *            
 * @ingroup   qeidriver
 *            
 * @brief     This is the generated driver source file for QEI3 driver
 *
 * @skipline @version   PLIB Version 1.0.5
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

#include <qei3.h>


// Section: Driver Interface

const struct QEI_INTERFACE QEI3 = {
    .Initialize = &QEI3_Initialize,
    .Deinitialize = &QEI3_Deinitialize,
    .Enable = &QEI3_Enable,
    .Disable = &QEI3_Disable,
    .PositionCountRead = &QEI3_PositionCountRead,
    .PositionCount16bitRead = &QEI3_PositionCount16bitRead,
    .PositionCountWrite = &QEI3_PositionCountWrite,
    .ModuloRangeSet = &QEI3_ModuloRangeSet,
    .PhaseInputSwappedGet = &QEI3_PhaseInputSwappedGet,
    .PhaseInputSwappedSet = &QEI3_PhaseInputSwappedSet,
    .PositionCaptureEnable = &QEI3_PositionCaptureEnable,
    .PositionCaptureDisable = &QEI3_PositionCaptureDisable,
    .PositionCaptureGet = &QEI3_PositionCaptureGet,
    .PositionCapture16bitGet = &QEI3_PositionCapture16bitGet,
    .PositionCaptureSet = &QEI3_PositionCaptureSet,
    .CounterModeSet = &QEI3_CounterModeSet,
    .IMVGatedValueSet = &QEI3_IMVGatedValueSet
};

// Section: QEI3 Module APIs

void QEI3_Initialize(void)
{
    /* CCM Quadrature Encoder mode; GATEN disabled; CNTPOL disabled; INTDIV 1:1; IMV Index match occurs when QEBx = 0 and QEAx = 0; PIMOD Free running mode; SIDL disabled; ON disabled; */
    QEI3CON = 0x0UL;
    /* QEAPOL disabled; QEBPOL disabled; IDXPOL disabled; HOMPOL disabled; SWPAB disabled; OUTFNC disabled; QFDIV 1:50000000; FLTREN enabled; QCAPEN disabled; HCAPEN disabled; */
    QEI3IOC = 0x4000UL;
    /* IDXIEN disabled; IDXIRQ No index event has occured; HOMIEN disabled; HOMIRQ No home event has occured; VELOVIEN disabled; VELOVIRQ No overflow has occured; PCIIEN disabled; PCIIRQ POSxCNT was not reinitialized; POSOVIEN disabled; POSOVIRQ No overflow has occured; PCLEQIEN disabled; PCLEQIRQ POSxCNT less than QEIxLEC; PCHEQIEN disabled; PCHEQIRQ POSxCNT less than QEIxGEC; */
    QEI3STAT = 0x0UL;
    /* POSCNT 0x0; */
    POS3CNT = 0x0UL;
    /* POSHLD 0x0; */
    POS3HLD = 0x0UL;
    /* VELCNT 0x0; */
    VEL3CNT = 0x0UL;
    /* VELHLD 0x0; */
    VEL3HLD = 0x0UL;
    /* INTTMR 0x0; */
    INT3TMR = 0x0UL;
    /* INTHLD 0x0; */
    INT3HLD = 0x0UL;
    /* INDXCNT 0x0; */
    INDX3CNT = 0x0UL;
    /* INDXHLD 0x0; */
    INDX3HLD = 0x0UL;
    /* QEIGEC 0; */
    QEI3GEC = 0x0UL;
    /* QEILEC 0x0; */
    QEI3LEC = 0x0UL;
}

void QEI3_Deinitialize(void)
{
    QEI3CON = 0x0UL;
    QEI3IOC = 0x0UL;
    QEI3STAT = 0x0UL;
    POS3CNT = 0x0UL;
    POS3HLD = 0x0UL;
    VEL3CNT = 0x0UL;
    VEL3HLD = 0x0UL;
    INT3TMR = 0x0UL;
    INT3HLD = 0x0UL;
    INDX3CNT = 0x0UL;
    INDX3HLD = 0x0UL;
    QEI3GEC = 0x0UL;
    QEI3LEC = 0x0UL;
}




