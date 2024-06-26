/*

blink both LEDs with 1sec ON and 1sec OFF using epwm3

*/


#include "F28x_Project.h"

void Gpio_select(void);

void Setup_ePWM(void);

extern interrupt void epwm3_isr(void);

int x = 0;


void main(void)

{

    InitSysCtrl();

    DINT;

    Gpio_select();

    Setup_ePWM();

    InitPieCtrl();

    IER = 0x0000;

    IFR = 0x0000;

    InitPieVectTable();


    EALLOW;

    PieVectTable.EPWM3_INT = &epwm3_isr;

    EDIS;


    IER |= M_INT3;

    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;

    EINT;

    ERTM;

    while(1)

    {


    }

}


void Gpio_select(void)

{

//    EALLOW;
//
//
//    GpioCtrlRegs.GPAMUX1.bit.GPIO6=1;
//
//    GpioCtrlRegs.GPAMUX1.bit.GPIO7=1;
//
//
//    EDIS;

    EALLOW;

    GpioCtrlRegs.GPAMUX2.bit.GPIO31=0;          //Pin 31 Set as GPIO

    GpioCtrlRegs.GPBMUX1.bit.GPIO34=0;          //Pin 34 Set as GPIO

    GpioCtrlRegs.GPADIR.bit.GPIO31=1;           //Set as Output

    GpioCtrlRegs.GPBDIR.bit.GPIO34=1;           //Set as Output

    EDIS;

}


void Setup_ePWM(void)

{

    EPwm3Regs.TBCTL.bit.CLKDIV=7;

    EPwm3Regs.TBCTL.bit.HSPCLKDIV=7;    // epwm clock = 25 MHz

    EPwm3Regs.TBCTL.bit.CTRMODE=2;      // up-down counter mode

    EPwm3Regs.AQCTLA.all=0x0009;        // 0000 0000 0000 1001

    EPwm3Regs.TBPRD=27902;              // So the full time period is 1 sec

//    EPwm4Regs.CMPA.bit.CMPA =25;
//
//
//    EPwm4Regs.DBCTL.bit.OUT_MODE=3;
//
//    EPwm4Regs.DBCTL.bit.POLSEL=2;
//
//    EPwm4Regs.DBFED.all=40;
//
//    EPwm4Regs.DBRED.all=50;

//    EPwm4Regs.ETSEL.bit.INTSELCMP = 0;

    EPwm3Regs.ETSEL.bit.INTEN = 1;   // enable interrupt

    EPwm3Regs.ETSEL.bit.INTSEL = 3;  // interrupt generated when tbctr = 0 or tbprd

    EPwm3Regs.ETPS.bit.INTPRD = 1;   // to generate interrupt at the first instance itself

    EPwm3Regs.ETCLR.bit.INT = 1;     // to generate further interrupt pulses




}

extern interrupt void epwm3_isr(void)
{
    x = x + 1;
    GpioDataRegs.GPATOGGLE.bit.GPIO31=1;    //Toggle LED D10

    GpioDataRegs.GPBTOGGLE.bit.GPIO34=1;    //Toggle LED D09

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

    EPwm3Regs.ETCLR.bit.INT = 1;
}


