/*

 Generate a sine triangle PWM with carrier frequency of 5 kHz.

 The modulating signal has a frequency of 40 Hz. Assuming a modulation index of 0.8 and sampling time of

 10us, generate the PWM in EPWM4A using ePWM interrupt. Generate the complementary pulse on EPWM4B.

 */


#include "F28x_Project.h"

#include "math.h"

#define PI 3.14159265358979323846

void Gpio_select(void);

void Setup_ePWM(void);

extern interrupt void epwm3_isr(void);

float theta = 0, sinevalue, sineshifted, modindex = 0.8;

int sinefreq = 40;


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

    EALLOW;


    GpioCtrlRegs.GPAMUX1.bit.GPIO4=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO5=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO6=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO7=1;


    EDIS;

}


void Setup_ePWM(void)

{

    //ePWM 4 configuration  ---> triangular carrier wave


    EPwm4Regs.TBCTL.bit.CLKDIV = 0;

    EPwm4Regs.TBCTL.bit.HSPCLKDIV = 0;

    EPwm4Regs.TBCTL.bit.CTRMODE = 2;

    EPwm4Regs.TBPRD = 10000;

    EPwm4Regs.AQCTLA.all = 0x0090;

    EPwm4Regs.DBCTL.bit.OUT_MODE = 3;

    EPwm4Regs.DBCTL.bit.POLSEL = 2;


    //ePWM 3 configuration    ----> this is used to generate the interrupt at 10 us


    EPwm3Regs.TBCTL.bit.CLKDIV = 0;

    EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0;

    EPwm3Regs.TBCTL.bit.CTRMODE = 0;

    EPwm3Regs.TBPRD = 1000;

    EPwm3Regs.AQCTLA.all = 0x0012;

    EPwm3Regs.CMPA.bit.CMPA = 500;


    EPwm3Regs.ETSEL.bit.INTSELCMP = 0;

    EPwm3Regs.ETSEL.bit.INTEN = 1;

    EPwm3Regs.ETSEL.bit.INTSEL = 4;

    EPwm3Regs.ETPS.bit.INTPRD = 1;

    EPwm3Regs.ETCLR.bit.INT = 1;

}


extern interrupt void epwm3_isr(void)

{

    theta = theta + (2.0 * PI * sinefreq * 10e-6);

    sinevalue = sin(theta);

    sineshifted = 0.5 + (modindex * sinevalue)/2.0;

    EPwm4Regs.CMPA.bit.CMPA = sineshifted * EPwm4Regs.TBPRD;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

    EPwm3Regs.ETCLR.bit.INT = 1;

}

