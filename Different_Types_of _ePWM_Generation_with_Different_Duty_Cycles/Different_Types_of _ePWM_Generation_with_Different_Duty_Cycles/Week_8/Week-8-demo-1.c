/*

Write a program that takes a ramp digital as an input. The ramp signal varies from 0 to 4095 every 1 sec.

The output of the DAC is given as an input to ADC which convert the analog value obtained from DAC back to digital.

Use ePWM interrupt as trigger source for SoC in ADC. Use DACOUTA and ADCA2.

Connect the hardware DACOUT pin (i.e DACOUTA-J3 30th pin) to the ADC (ADCA2-J3 29th pin) on the launchpad via a connector (JP5).

Students can see the result of the ADC in the expression window.

Note: JP5 is a connector on the launchpad. Use the connector to short J3 29 and 30 pins

 */


#include "F28x_Project.h"

void Gpio_select(void);

void Setup_ePWM(void);d

void ConfigureADC(void);

void SetupADCSoftware(void);

void InitDaca(void);

extern interrupt void epwm3_isr(void);

Uint16 DAC_input = 100;

Uint16 AdcaResult0;


void main(void)

{

    InitSysCtrl();

    DINT;

    InitPieCtrl();

    IER = 0x0000;

    IFR = 0x0000;

    InitPieVectTable();


    EALLOW;

    CpuSysRegs.PCLKCR2.bit.EPWM1=1;

    EDIS;


    Gpio_select();

    Setup_ePWM();

    InitDaca();

    ConfigureADC();

    SetupADCSoftware();


    EALLOW;

    PieVectTable.EPWM3_INT = &epwm3_isr;

    EDIS;


    IER |= M_INT3;

    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;

    EINT;

    ERTM;


    while(1)

    {

        DacaRegs.DACVALS.bit.DACVALS = DAC_input;

        AdcaResult0 = AdcaResultRegs.ADCRESULT0;

    }

}


void Gpio_select(void)

{

    EALLOW;


    GpioCtrlRegs.GPAMUX1.bit.GPIO4=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO5=1;


    EDIS;

}


void Setup_ePWM(void)

{

    EPwm3Regs.TBCTL.bit.CLKDIV=7;

    EPwm3Regs.TBCTL.bit.HSPCLKDIV=7;

    EPwm3Regs.TBCTL.bit.CTRMODE=0;

    EPwm3Regs.AQCTLA.all=0x0012;

    EPwm3Regs.TBPRD=55804;

    EPwm3Regs.CMPA.bit.CMPA =55804/2;


    EPwm3Regs.ETSEL.bit.SOCAEN=1;

    EPwm3Regs.ETSEL.bit.SOCASEL=1;

    EPwm3Regs.ETPS.bit.SOCAPRD=1;

    EPwm3Regs.ETCLR.bit.SOCA=1;


    EPwm3Regs.ETSEL.bit.INTEN = 1;

    EPwm3Regs.ETSEL.bit.INTSEL = 1;

    EPwm3Regs.ETPS.bit.INTPRD = 1;

    EPwm3Regs.ETCLR.bit.INT = 1;

}


void InitDaca(void)

{

    EALLOW;


    DacaRegs.DACCTL.bit.DACREFSEL = 1;

    DacaRegs.DACCTL.bit.LOADMODE = 0;

    DacaRegs.DACOUTEN.bit.DACOUTEN = 1;

    DacaRegs.DACVALS.bit.DACVALS = DAC_input;

    DELAY_US(10);


    EDIS;

}


void ConfigureADC(void)

{

    EALLOW;


    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;

    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;


    EDIS;


    DELAY_US(1000);

}


void SetupADCSoftware(void)

{

    Uint16 acqps;


    if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION)

    {

    acqps = 14;

    }

    else

    {

    acqps = 63; //320ns

    }


    EALLOW;


    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 2;

    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps;

    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 9;


    EDIS;

}


extern interrupt void epwm3_isr(void)

{

    DAC_input = DAC_input + 2;

    if(DAC_input>4095)

        DAC_input = 0;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

    EPwm3Regs.ETCLR.bit.INT = 1;

}



