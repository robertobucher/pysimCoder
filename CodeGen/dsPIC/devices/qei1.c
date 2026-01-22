/**
 * QEI1 Generated Driver Source File 
 * 
 * @file      qei1.c
 *            
 * @ingroup   qeidriver
 *            
 * @brief     This is the generated driver source file for QEI1 driver
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

#include <qei1.h>


// Section: Driver Interface

const struct QEI_INTERFACE QEI1 = {
    .Initialize = &QEI1_Initialize,
    .Deinitialize = &QEI1_Deinitialize,
    .Enable = &QEI1_Enable,
    .Disable = &QEI1_Disable,
    .PositionCountRead = &QEI1_PositionCountRead,
    .PositionCount16bitRead = &QEI1_PositionCount16bitRead,
    .PositionCountWrite = &QEI1_PositionCountWrite,
    .ModuloRangeSet = &QEI1_ModuloRangeSet,
    .PhaseInputSwappedGet = &QEI1_PhaseInputSwappedGet,
    .PhaseInputSwappedSet = &QEI1_PhaseInputSwappedSet,
    .PositionCaptureEnable = &QEI1_PositionCaptureEnable,
    .PositionCaptureDisable = &QEI1_PositionCaptureDisable,
    .PositionCaptureGet = &QEI1_PositionCaptureGet,
    .PositionCapture16bitGet = &QEI1_PositionCapture16bitGet,
    .PositionCaptureSet = &QEI1_PositionCaptureSet,
    .CounterModeSet = &QEI1_CounterModeSet,
    .IMVGatedValueSet = &QEI1_IMVGatedValueSet
};

// Section: QEI1 Module APIs

void QEI1_Initialize(void)
{
    /* CCM Quadrature Encoder mode; GATEN disabled; CNTPOL disabled; INTDIV 1:1; IMV Index match occurs when QEBx = 0 and QEAx = 0; PIMOD Free running mode; SIDL disabled; ON disabled; */
    QEI1CON = 0x0UL;
    /* QEAPOL disabled; QEBPOL disabled; IDXPOL disabled; HOMPOL disabled; SWPAB disabled; OUTFNC disabled; QFDIV 1:50000000; FLTREN enabled; QCAPEN disabled; HCAPEN disabled; */
    QEI1IOC = 0x4000UL;
    /* IDXIEN disabled; IDXIRQ No index event has occured; HOMIEN disabled; HOMIRQ No home event has occured; VELOVIEN disabled; VELOVIRQ No overflow has occured; PCIIEN disabled; PCIIRQ POSxCNT was not reinitialized; POSOVIEN disabled; POSOVIRQ No overflow has occured; PCLEQIEN disabled; PCLEQIRQ POSxCNT less than QEIxLEC; PCHEQIEN disabled; PCHEQIRQ POSxCNT less than QEIxGEC; */
    QEI1STAT = 0x0UL;
    /* POSCNT 0x0; */
    POS1CNT = 0x0UL;
    /* POSHLD 0x0; */
    POS1HLD = 0x0UL;
    /* VELCNT 0x0; */
    VEL1CNT = 0x0UL;
    /* VELHLD 0x0; */
    VEL1HLD = 0x0UL;
    /* INTTMR 0x0; */
    INT1TMR = 0x0UL;
    /* INTHLD 0x0; */
    INT1HLD = 0x0UL;
    /* INDXCNT 0x0; */
    INDX1CNT = 0x0UL;
    /* INDXHLD 0x0; */
    INDX1HLD = 0x0UL;
    /* QEIGEC 0; */
    QEI1GEC = 0x0UL;
    /* QEILEC 0x0; */
    QEI1LEC = 0x0UL;
}

void QEI1_Deinitialize(void)
{
    QEI1CON = 0x0UL;
    QEI1IOC = 0x0UL;
    QEI1STAT = 0x0UL;
    POS1CNT = 0x0UL;
    POS1HLD = 0x0UL;
    VEL1CNT = 0x0UL;
    VEL1HLD = 0x0UL;
    INT1TMR = 0x0UL;
    INT1HLD = 0x0UL;
    INDX1CNT = 0x0UL;
    INDX1HLD = 0x0UL;
    QEI1GEC = 0x0UL;
    QEI1LEC = 0x0UL;
}




