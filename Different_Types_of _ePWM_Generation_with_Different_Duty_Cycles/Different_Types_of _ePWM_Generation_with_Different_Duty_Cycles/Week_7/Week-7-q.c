/*
 Write a program to generate a sine wave at buffered DAC output, sine wave with a frequency of 0.1 Hz

 and a sampling time of 100us. The sampled sine value is loaded onto DAC at a rate of 1 ms.

 */

#include "F28x_Project.h"

#include "math.h"

#define PI 3.14159265358979323846

extern interrupt void cpu_timer0_isr(void);

//extern interrupt void epwm3_isr(void);     // interrupt using epwm3 (for 100us)

void Initdaca(void);
int counter=0;

Uint16 DAC_input = 100;

float theta = 0, sinevalue, sineshifted,modindex=1;

float sinefreq = 0.1;


void main(void)

{

    InitSysCtrl();

    DINT;

    InitPieCtrl();

    IER = 0x0000;

    IFR = 0x0000;

    InitPieVectTable();


    Initdaca();


    EALLOW;

    PieVectTable.TIMER0_INT = &cpu_timer0_isr;

    EDIS;


    InitCpuTimers();

    ConfigCpuTimer(&CpuTimer0,200,100);     // generates interrupt every 100us

    CpuTimer0Regs.TCR.all = 0x4000;

    IER |= M_INT1;

    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

    EINT;

    ERTM;


    while(1)

    {

        while(DAC_input<4095)

        {

             DacaRegs.DACVALS.bit.DACVALS = DAC_input;

             while(CpuTimer0.InterruptCount<10);         // so until 10 timer0 interrupts or 1ms is passed the DACVALS is not changed

             CpuTimer0.InterruptCount=0;

//             DAC_input=DAC_input*sineshifted;

        }

        DAC_input=100;

    }

}

void Initdaca(void)

{

    EALLOW;


    DacaRegs.DACCTL.bit.DACREFSEL = 1;

    DacaRegs.DACCTL.bit.LOADMODE = 0;

    DacaRegs.DACOUTEN.bit.DACOUTEN = 1;

    DacaRegs.DACVALS.bit.DACVALS = DAC_input;

    DELAY_US(10);


    EDIS;

}


extern interrupt void cpu_timer0_isr(void)

{

   theta = theta + (2.0 * PI * sinefreq * 100e-6);

   sinevalue = sin(theta);

   sineshifted = 0.5 + (modindex * sinevalue)/2.0;

   DAC_input = 4096*sineshifted;

   CpuTimer0.InterruptCount++;

   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}







