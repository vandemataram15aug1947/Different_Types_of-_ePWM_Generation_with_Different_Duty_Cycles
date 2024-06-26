/* Generate a 1 kHz PWM signal with 30% duty using ePWM channel 4A

Use up counting mode. Generate its complementary signal on ePWM 4B */


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


    GpioCtrlRegs.GPAMUX1.bit.GPIO6=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO7=1;


    EDIS;


}


void Setup_ePWM(void)

{

    EPwm4Regs.TBCTL.bit.CLKDIV=1;

    EPwm4Regs.TBCTL.bit.HSPCLKDIV=1;

    EPwm4Regs.TBCTL.bit.CTRMODE=0;

    EPwm4Regs.AQCTLA.all=0x0012;

    EPwm4Regs.AQCTLB.all=0x0021;

    EPwm4Regs.TBPRD=25000;

    EPwm4Regs.CMPA.bit.CMPA =25000*0.3;

}



