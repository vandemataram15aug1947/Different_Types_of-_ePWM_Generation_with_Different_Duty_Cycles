/*

Generate a 5 kHz PWM signal with 30% duty using ePWM 4A. Use up down counting mode.

Generate complementary pulse on ePWM 4B with a rising edge delay of 2 us and falling edge delay of 1.6 us

*/


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


    GpioCtrlRegs.GPAMUX1.bit.GPIO6=1;  //Epwm 4A

    GpioCtrlRegs.GPAMUX1.bit.GPIO7=1;  //Epwm 4B


    EDIS;

}


void Setup_ePWM(void)

{

    EPwm4Regs.TBCTL.bit.CLKDIV=1;

    EPwm4Regs.TBCTL.bit.HSPCLKDIV=1;    // 25 MHz epwm clock

    EPwm4Regs.TBCTL.bit.CTRMODE=2;      // up-down counting mode

    EPwm4Regs.AQCTLA.all=0x0060;        // 0000 0000 0110 0000

    EPwm4Regs.TBPRD=2500;

    EPwm4Regs.CMPA.bit.CMPA =2500*0.3;

    // rising edge and falling edge delay obtained for Epwm 4B
    EPwm4Regs.DBCTL.bit.OUT_MODE=3;    // DBCTL fully active (Rising edge delay and falling edge delay available

    EPwm4Regs.DBCTL.bit.POLSEL=2;      // For Active High Complimentary

    EPwm4Regs.DBFED.all=40;

    EPwm4Regs.DBRED.all=50;

}

