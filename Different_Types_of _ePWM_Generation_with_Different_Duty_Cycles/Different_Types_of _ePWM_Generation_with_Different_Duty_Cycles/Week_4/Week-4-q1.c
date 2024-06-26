 /*Generate a 2 kHz PWM signal with 60% duty using ePWM channel 3A.
 Use up-down counting mode. Show the result in the oscilloscope. Generate its complementary signal on ePWM 3B. */


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


    GpioCtrlRegs.GPAMUX1.bit.GPIO4=1;  // for EPMW3A

    GpioCtrlRegs.GPAMUX1.bit.GPIO5=1;  // for EPMW3B


    EDIS;


}


void Setup_ePWM(void)

{

    EPwm3Regs.TBCTL.bit.CLKDIV=1;   // 100/2 MHz

    EPwm3Regs.TBCTL.bit.HSPCLKDIV=1;   // 100/2*2 MHz = 25 MHz

    EPwm3Regs.TBCTL.bit.CTRMODE= 2;   // for up-down count mode

    EPwm3Regs.AQCTLA.all=0x0060;    // 0000 0000 0110 0000

    EPwm3Regs.AQCTLB.all=0x0090;    // 0000 0000 1001 0000

    EPwm3Regs.TBPRD=25000;     // As T = 2*TBPRD*(1/f)  for up-down count mode

    EPwm3Regs.CMPA.bit.CMPA =25000*0.6;  // for 60% duty cycle

}



