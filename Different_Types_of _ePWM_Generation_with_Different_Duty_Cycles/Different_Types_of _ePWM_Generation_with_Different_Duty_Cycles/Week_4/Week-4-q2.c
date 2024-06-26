
/*Generate a 5 kHz PWM signal with 30% duty using ePWM channel 2A.
Use down counting mode. Configure ePWM3 to generate a similar PWM but 90- degree phase-shifted from that of ePWM2. */


#include "F28x_Project.h"

void Gpio_select(void);

void Setup_ePWM(void);


void main(void)

{

    InitSysCtrl();

    DINT;

    Gpio_select();

    Setup_ePWM();

    while(1)

    {


    }

}


void Gpio_select(void)

{

    EALLOW;


    GpioCtrlRegs.GPAMUX1.bit.GPIO2=1;   // EPWM 2A

    GpioCtrlRegs.GPAMUX1.bit.GPIO4=1;   // EPWM 3A


    EDIS;


}


void Setup_ePWM(void)

{

    // EPwm 2

    EPwm2Regs.TBCTL.bit.CLKDIV=1;

    EPwm2Regs.TBCTL.bit.HSPCLKDIV=1;    // 25 MHz

    EPwm2Regs.TBCTL.bit.CTRMODE=1;      // down counting mode

    EPwm2Regs.AQCTLA.all=0x0082;        // 0000 0000 1000 0010

    EPwm2Regs.TBPRD=5000;

    EPwm2Regs.CMPA.bit.CMPA = 5000*0.3;

    // EPwm 3

    EPwm3Regs.TBCTL.bit.PHSEN = 1;   // enable phase shift

//    maybe you have to write

    EPwm3Regs.TBCTL.bit.CLKDIV=1;

    EPwm3Regs.TBCTL.bit.HSPCLKDIV=1;    // 25 MHz

    EPwm3Regs.TBCTL.bit.CTRMODE=1;      // down counting mode

    EPwm3Regs.TBPHS.all = 2500;  // phase value for 90 degrees phase shift

    EPwm3Regs.AQCTLA.all=0x0082;        // 0000 0000 1000 0010

    EPwm3Regs.TBPRD=5000;

    EPwm3Regs.CMPA.bit.CMPA = 5000*0.3;


}



