/**
 * QEI2 Generated Driver Source File 
 * 
 * @file      qei2.c
 *            
 * @ingroup   qeidriver
 *            
 * @brief     This is the generated driver source file for QEI2 driver
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

#include <qei2.h>


// Section: Driver Interface

const struct QEI_INTERFACE QEI2 = {
    .Initialize = &QEI2_Initialize,
    .Deinitialize = &QEI2_Deinitialize,
    .Enable = &QEI2_Enable,
    .Disable = &QEI2_Disable,
    .PositionCountRead = &QEI2_PositionCountRead,
    .PositionCount16bitRead = &QEI2_PositionCount16bitRead,
    .PositionCountWrite = &QEI2_PositionCountWrite,
    .ModuloRangeSet = &QEI2_ModuloRangeSet,
    .PhaseInputSwappedGet = &QEI2_PhaseInputSwappedGet,
    .PhaseInputSwappedSet = &QEI2_PhaseInputSwappedSet,
    .PositionCaptureEnable = &QEI2_PositionCaptureEnable,
    .PositionCaptureDisable = &QEI2_PositionCaptureDisable,
    .PositionCaptureGet = &QEI2_PositionCaptureGet,
    .PositionCapture16bitGet = &QEI2_PositionCapture16bitGet,
    .PositionCaptureSet = &QEI2_PositionCaptureSet,
    .CounterModeSet = &QEI2_CounterModeSet,
    .IMVGatedValueSet = &QEI2_IMVGatedValueSet
};

// Section: QEI2 Module APIs

void QEI2_Initialize(void)
{
    /* CCM Quadrature Encoder mode; GATEN disabled; CNTPOL disabled; INTDIV 1:1; IMV Index match occurs when QEBx = 0 and QEAx = 0; PIMOD Free running mode; SIDL disabled; ON disabled; */
    QEI2CON = 0x0UL;
    /* QEAPOL disabled; QEBPOL disabled; IDXPOL disabled; HOMPOL disabled; SWPAB disabled; OUTFNC disabled; QFDIV 1:50012210.01221001; FLTREN enabled; QCAPEN disabled; HCAPEN disabled; */
    QEI2IOC = 0x4000UL;
    /* IDXIEN disabled; IDXIRQ No index event has occured; HOMIEN disabled; HOMIRQ No home event has occured; VELOVIEN disabled; VELOVIRQ No overflow has occured; PCIIEN disabled; PCIIRQ POSxCNT was not reinitialized; POSOVIEN disabled; POSOVIRQ No overflow has occured; PCLEQIEN disabled; PCLEQIRQ POSxCNT less than QEIxLEC; PCHEQIEN disabled; PCHEQIRQ POSxCNT less than QEIxGEC; */
    QEI2STAT = 0x0UL;
    /* POSCNT 0x0; */
    POS2CNT = 0x0UL;
    /* POSHLD 0x0; */
    POS2HLD = 0x0UL;
    /* VELCNT 0x0; */
    VEL2CNT = 0x0UL;
    /* VELHLD 0x0; */
    VEL2HLD = 0x0UL;
    /* INTTMR 0x0; */
    INT2TMR = 0x0UL;
    /* INTHLD 0x0; */
    INT2HLD = 0x0UL;
    /* INDXCNT 0x0; */
    INDX2CNT = 0x0UL;
    /* INDXHLD 0x0; */
    INDX2HLD = 0x0UL;
    /* QEIGEC 0; */
    QEI2GEC = 0x0UL;
    /* QEILEC 0x0; */
    QEI2LEC = 0x0UL;
}

void QEI2_Deinitialize(void)
{
    QEI2CON = 0x0UL;
    QEI2IOC = 0x0UL;
    QEI2STAT = 0x0UL;
    POS2CNT = 0x0UL;
    POS2HLD = 0x0UL;
    VEL2CNT = 0x0UL;
    VEL2HLD = 0x0UL;
    INT2TMR = 0x0UL;
    INT2HLD = 0x0UL;
    INDX2CNT = 0x0UL;
    INDX2HLD = 0x0UL;
    QEI2GEC = 0x0UL;
    QEI2LEC = 0x0UL;
}




