/*

Write a program that takes a digital input of 1000.  The output of the DAC is given as an input to ADC

which convert the analog value obtained from DAC back to digital. Use ePWM interrupt as trigger source

for SoC in ADC. Use DACOUTA and ADCA2. Connect the hardware DACOUT pin (i.e DACOUTA-J3 30th pin) to the

ADC (ADCA2-J3 29th pin) on the launchpad via a connector (JP5). Students can see the result of the ADC

in the expression window.

Also use the end of conversion ADC interrupt to blink a LED and store the ADC Result.

Note: JP5 is a connector on the launchpad. Use the connector to short J3 29 and 30 pins

 */


#include "F28x_Project.h"

void Gpio_select(void);

void Setup_ePWM(void);

void ConfigureADC(void);

void SetupADCSoftware(void);

void InitDaca(void);

extern interrupt void adca2_isr(void);

Uint16 DAC_input = 1000;

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


    EALLOW;

    PieVectTable.ADCA2_INT = &adca2_isr;

    EDIS;


    IER |= M_INT10;

    PieCtrlRegs.PIEIER10.bit.INTx2 = 1;


    Gpio_select();

    Setup_ePWM();

    InitDaca();

    ConfigureADC();

    SetupADCSoftware();

    EINT;

    ERTM;


    while(1)

    {

        DacaRegs.DACVALS.bit.DACVALS = DAC_input;

    }

}


void Gpio_select(void)

{

    EALLOW;


    GpioCtrlRegs.GPAMUX1.bit.GPIO4=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO5=1;

    GpioCtrlRegs.GPAMUX2.bit.GPIO31=0;


    GpioCtrlRegs.GPADIR.bit.GPIO31=1;


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

    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;

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

    AdcaRegs.ADCINTSEL1N2.bit.INT2SEL = 0;

    AdcaRegs.ADCINTSEL1N2.bit.INT2E = 1;

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;


    EDIS;

}


extern interrupt void adca2_isr(void)

{

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;

    if(1 == AdcaRegs.ADCINTOVF.bit.ADCINT2)

    {

        AdcaRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;

        AdcaRegs.ADCINTOVFCLR.bit.ADCINT2 = 1;

    }

    AdcaResult0 = AdcaResultRegs.ADCRESULT0;

    GpioDataRegs.GPATOGGLE.bit.GPIO31=1;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;

}
