/*

Write a program to call the ‘sin’ function and scale the sine wave from 0 to 4095
(0 being the minimum value of sine and 4095 being the maximum value of sine) with the frequency of the sine wave being 0.1 Hz.
The ‘sin’ function should be called in steps of 100 msec with a sampling time of 100 microseconds.
The sine wave (in steps) so obtained is given as a DAC input, and the output of DAC (i.e., DACOUTA - J3 30th pin)
is then connected to the ADC (ADCA2 - J3 29th pin) on the launchpad via a connector (JP5). Generate a trigger to
toggle both the LEDs on completion of each cycle.

 */


#include "F28x_Project.h"

#include "math.h"

#define PI 3.14159265358979323846

void Gpio_select(void);

void Setup_ePWM(void);

void ConfigureADC(void);

void SetupADCSoftware(void);

void InitDaca(void);

extern interrupt void epwm3_isr(void);

Uint16 DAC_input = 100;

Uint16 AdcaResult0;

float theta = 0, sinevalue, sineshifted, modindex = 1;

float sinefreq = 0.1;     // 0.1 Hz

int epwm_3_int_count = 0;

int x =0;


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

//        while(epwm_3_int_count < 1000);      // stays in while loop for 1000 us = 1 ms

//        epwm_3_int_count = 0;

        DacaRegs.DACVALS.bit.DACVALS = DAC_input;

        if(DAC_input > 2000 && DAC_input <2400){
            GpioDataRegs.GPATOGGLE.bit.GPIO31=1;    //Toggle LED D10

            GpioDataRegs.GPBTOGGLE.bit.GPIO34=1;    //Toggle LED D09
        }


        AdcaResult0 = AdcaResultRegs.ADCRESULT0;

    }

}


void Gpio_select(void)

{

    EALLOW;


    GpioCtrlRegs.GPAMUX1.bit.GPIO4=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO5=1;

    GpioCtrlRegs.GPAMUX2.bit.GPIO31=0;          //Pin 31 Set as GPIO

    GpioCtrlRegs.GPBMUX1.bit.GPIO34=0;          //Pin 34 Set as GPIO

    GpioCtrlRegs.GPADIR.bit.GPIO31=1;           //Set as Output

    GpioCtrlRegs.GPBDIR.bit.GPIO34=1;           //Set as Output


    EDIS;

}


void Setup_ePWM(void)

{


    EPwm3Regs.TBCTL.bit.CLKDIV=1;    // 100/2 MHz = 50 MHz

    EPwm3Regs.TBCTL.bit.HSPCLKDIV=1;  // 50/2 = 25 MHz

    EPwm3Regs.TBCTL.bit.CTRMODE=0;   // up-count

    EPwm3Regs.AQCTLA.all=0x0012;     //0000 0000 0001 0010

    EPwm3Regs.TBPRD=2500;           // Epwm time period = 100 us

    EPwm3Regs.CMPA.bit.CMPA =2500/2;


    EPwm3Regs.ETSEL.bit.SOCAEN=1;

    EPwm3Regs.ETSEL.bit.SOCASEL=4;    // ADC SOC when TBCTR = 0

    EPwm3Regs.ETPS.bit.SOCAPRD=1;

    EPwm3Regs.ETCLR.bit.SOCA=1;


    EPwm3Regs.ETSEL.bit.INTEN = 1;

    EPwm3Regs.ETSEL.bit.INTSEL = 1;    // epwm interrupt when TBCTR = 0

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
    x = x + 1;

    epwm_3_int_count = epwm_3_int_count + 1;

    theta = theta + (2.0 * PI * sinefreq * 100e-6);

    sinevalue = sin(theta);

    sineshifted = 0.5 + (modindex * sinevalue)/2.0;

    DAC_input = 4096*sineshifted;

//    CpuTimer0.InterruptCount++;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

    /*
    DAC_input = DAC_input + 2;

    if(DAC_input>4095)

        DAC_input = 0;
    */

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

    EPwm3Regs.ETCLR.bit.INT = 1;

}







