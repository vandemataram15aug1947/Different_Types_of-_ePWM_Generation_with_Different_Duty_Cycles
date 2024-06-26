/*

Write a program to change the DAC input value every 2 sec and subsequent analog conversion.

The DAC input should be initialised to 100 and changed in steps of 500 every 2 sec.

 */


#include "F28x_Project.h"

extern interrupt void cpu_timer0_isr(void);

void Initdaca(void);

int counter=0;

Uint16 DAC_input = 100;


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

    ConfigCpuTimer(&CpuTimer0,200,2000000);

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

             while(CpuTimer0.InterruptCount==0);

             CpuTimer0.InterruptCount=0;

             DAC_input=DAC_input+500;

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

   CpuTimer0.InterruptCount++;

   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}



