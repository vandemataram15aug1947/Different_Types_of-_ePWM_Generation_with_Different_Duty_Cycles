/*

Configure ePWM1 to generate a 2 kHz sine triangle PWM (symmetric carrier)
Take modulation index as 0.8. Use Timer Interrupt to execute the program.

 */


#include "F28x_Project.h"

#include "math.h"

#define PI 3.14159265358979323846

void Gpio_select(void);

void Setup_ePWM(void);

//extern interrupt void epwm1_isr(void);      // ePWM1 used as interrupt

extern interrupt void cpu_timer0_isr(void);

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

//    PieVectTable.EPWM1_INT = &epwm1_isr;   // ePWM1
    PieVectTable.TIMER0_INT = &cpu_timer0_isr;

    EDIS;

    InitCpuTimers();

    ConfigCpuTimer(&CpuTimer0,200,10);    // 200 MHz = CPU frequency and 10 = 10 us interrupt period

    CpuTimer0Regs.TCR.all = 0x4000;

    IER |= M_INT1;

//    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;     // enable group-3 and interrupt-1
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

    EINT;

    ERTM;


    while(1)

    {


    }

}


void Gpio_select(void)

{

    EALLOW;


//    GpioCtrlRegs.GPAMUX1.bit.GPIO4=1;
//
//    GpioCtrlRegs.GPAMUX1.bit.GPIO5=1;
//
//    GpioCtrlRegs.GPAMUX1.bit.GPIO6=1;
//
//    GpioCtrlRegs.GPAMUX1.bit.GPIO7=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO0=1;


    EDIS;

}


void Setup_ePWM(void)

{

    //ePWM 1 configuration  ---> triangular carrier wave


    EPwm1Regs.TBCTL.bit.CLKDIV = 0;     // 100 MHz

    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;   // 100 MHz

    EPwm1Regs.TBCTL.bit.CTRMODE = 2;     // up-down count mode

    EPwm1Regs.TBPRD = 25000;             //  time period of triangle wave = 2 KHS

    EPwm1Regs.AQCTLA.all = 0x0060;       // 0000 0000 0110 0000

//    EPwm1Regs.DBCTL.bit.OUT_MODE = 3;
//
//    EPwm1Regs.DBCTL.bit.POLSEL = 2;


    //ePWM 1 configuration    ----> this is used to generate the interrupt at 10 us


//    EPwm1Regs.TBCTL.bit.CLKDIV = 0;
//
//    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;
//
//    EPwm1Regs.TBCTL.bit.CTRMODE = 0;
//
//    EPwm1Regs.TBPRD = 1000;
//
//    EPwm1Regs.AQCTLA.all = 0x0012;
//
//    EPwm1Regs.CMPA.bit.CMPA = 500;
//
//
//    EPwm1Regs.ETSEL.bit.INTSELCMP = 0;
//
//    EPwm1Regs.ETSEL.bit.INTEN = 1;
//
//    EPwm1Regs.ETSEL.bit.INTSEL = 4;
//
//    EPwm1Regs.ETPS.bit.INTPRD = 1;
//
//    EPwm1Regs.ETCLR.bit.INT = 1;

}


//extern interrupt void epwm1_isr(void)
//
//{
//
//    theta = theta + (2.0 * PI * sinefreq * 10e-6);
//
//    sinevalue = sin(theta);
//
//    sineshifted = 0.5 + (modindex * sinevalue)/2.0;
//
//    EPwm4Regs.CMPA.bit.CMPA = sineshifted * EPwm4Regs.TBPRD;
//
//    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
//
//    EPwm1Regs.ETCLR.bit.INT = 1;
//
//}

extern interrupt void cpu_timer0_isr(void)
{
    theta = theta + (2.0 * PI * sinefreq * 10e-6);

    sinevalue = sin(theta);

    sineshifted = 0.5 + (modindex * sinevalue)/2.0;

    EPwm1Regs.CMPA.bit.CMPA = sineshifted * EPwm1Regs.TBPRD;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

//    EPwm1Regs.ETCLR.bit.INT = 1;
}



