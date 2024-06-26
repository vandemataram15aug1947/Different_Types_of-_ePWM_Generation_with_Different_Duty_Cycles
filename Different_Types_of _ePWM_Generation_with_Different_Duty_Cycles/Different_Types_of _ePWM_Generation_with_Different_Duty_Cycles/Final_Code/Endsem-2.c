/*

Create EPWM 1A with freq 10 KHZ interrupt. Generate the given Vx with it.

*/


#include "F28x_Project.h"

//#include "math.h"
//
//#define PI 3.14159265358979323846

void Gpio_select(void);

void Setup_ePWM(void);

void InitDaca(void);

void ConfigureADC(void);

void SetupADCSoftware(void);

//extern interrupt void epwm3_isr(void);     // interrupt using epwm3

extern interrupt void epwm1_isr(void);     // interrupt using epwm1

//float theta = 0, sinevalue, sineshifted, modindex = 0.1;
//
//int sinefreq = 40;

int x = 0;
int Vx = 0;

int AdcaResult0;


void main(void)

{

    InitSysCtrl();

    DINT;

    Gpio_select();

    Setup_ePWM();

    InitDaca();

    ConfigureADC();

    SetupADCSoftware();

    InitPieCtrl();

    IER = 0x0000;

    IFR = 0x0000;

    InitPieVectTable();

    EALLOW;

    CpuSysRegs.PCLKCR2.bit.EPWM1=1;        // used for synchronization
    CpuSysRegs.PCLKCR2.bit.EPWM2=1;
    CpuSysRegs.PCLKCR2.bit.EPWM3=1;

    EDIS;

//    EALLOW;
//    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
//    EDIS;
//
//    EALLOW;
//    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
//    EDIS;


    EALLOW;

    PieVectTable.EPWM1_INT = &epwm1_isr;

    EDIS;


    IER |= M_INT3;

    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;        // interrupt of PIEIER3.1 is set for epwm1 interrupt

    EINT;

    ERTM;


    while(1)

    {
        DacaRegs.DACVALS.bit.DACVALS = Vx;
        AdcaResult0 = (AdcaResultRegs.ADCRESULT0);

    }

}


void Gpio_select(void)

{

//    EALLOW;
//
//
//    GpioCtrlRegs.GPAMUX1.bit.GPIO4=1;
//
//    GpioCtrlRegs.GPAMUX1.bit.GPIO5=1;
//
//    GpioCtrlRegs.GPAMUX1.bit.GPIO6=1;
//
//    GpioCtrlRegs.GPAMUX1.bit.GPIO7=1;
//
//
//    EDIS;

    EALLOW;


    GpioCtrlRegs.GPAMUX1.bit.GPIO0=1;            // Setting up EPWM 1A, 1B, 2A, 2B, 3A and 3B

    GpioCtrlRegs.GPAMUX1.bit.GPIO1=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO2=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO3=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO4=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO5=1;

    //    GpioCtrlRegs.GPAMUX2.bit.GPIO31=0;          //Pin 31 Set as GPIO
    //
    //    GpioCtrlRegs.GPBMUX1.bit.GPIO34=0;          //Pin 34 Set as GPIO
    //
    //    GpioCtrlRegs.GPADIR.bit.GPIO31=1;           //Set as Output
    //
    //    GpioCtrlRegs.GPBDIR.bit.GPIO34=1;           //Set as Output


    EDIS;

}


void Setup_ePWM(void)

{

    //ePWM 1 configuration  ---> triangular wave with 10 KHz. Used to generating interrupt at 10 KHz rate.


    EPwm1Regs.TBCTL.bit.CLKDIV = 1;

    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 1;     // f = 25 MHz

    EPwm1Regs.TBCTL.bit.CTRMODE = 2;       //  up-down count mode

    EPwm1Regs.TBPRD = 2500/2;               // f of epwm4 is 10 KHz

    EPwm1Regs.AQCTLA.all = 0x0090;         // unnecessary

    EPwm1Regs.ETSEL.bit.INTSELCMP = 0;

    EPwm1Regs.ETSEL.bit.INTEN = 1;

    EPwm1Regs.ETSEL.bit.INTSEL = 1;        // interrupt generated when TBCTR = 0

    EPwm1Regs.ETPS.bit.INTPRD = 1;

    EPwm1Regs.ETCLR.bit.INT = 1;


    //ePWM 2 configuration    ----> f = 50 Hz or T = 20 ms


    EPwm2Regs.TBCTL.bit.CLKDIV = 4;

    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 5;     // f = 100/(10*16) MHz

    EPwm2Regs.TBCTL.bit.CTRMODE = 2;       //  up-down count mode

    EPwm2Regs.TBPRD = 6250;               // f of epwm2 is 50 Hz

    EPwm2Regs.AQCTLA.all = 0x0012;         //   0000 0000 0110 0000

    EPwm2Regs.CMPA.bit.CMPA = 0.425*6250;


//    EPwm2Regs.ETSEL.bit.INTSELCMP = 0;
//
//    EPwm2Regs.ETSEL.bit.INTEN = 1;
//
//    EPwm2Regs.ETSEL.bit.INTSEL = 4;
//
//    EPwm2Regs.ETPS.bit.INTPRD = 1;
//
//    EPwm2Regs.ETCLR.bit.INT = 1;

    EPwm2Regs.TBPHS.bit.TBPHS = 0.175*6250;     // 1.75ms phase shifted to the left

    EPwm2Regs.TBCTL.bit.PHSEN = 1;

    EPwm2Regs.TBCTL.bit.SYNCOSEL = 0;

    EPwm2Regs.TBCTL.bit.PHSDIR = 1;            //count up after phase synchronized

    EPwm2Regs.ETSEL.bit.SOCAEN = 1;

    EPwm2Regs.ETCLR.bit.SOCA = 1;

    EPwm2Regs.ETPS.bit.SOCAPRD = 1;            // SoC should happen in first event itself

    EPwm2Regs.ETSEL.bit.SOCASEL = 4;           //Soc will happen at TBCTR = CMPA when increamenting

//    EPwm2Regs.ETSEL.bit.SOCASEL = 2;           //Soc will happen at avg

    //EPwm2Regs.ETSEL.bit.SOCASEL = 5;           //Soc will happen at peak



    //ePWM 3 configuration    ----> f = 50 Hz or T = 20 ms but soc in the down count time


    EPwm3Regs.TBCTL.bit.CLKDIV = 4;

    EPwm3Regs.TBCTL.bit.HSPCLKDIV = 5;     // f = 100/(10*16) MHz

    EPwm3Regs.TBCTL.bit.CTRMODE = 2;       //  up-down count mode

    EPwm3Regs.TBPRD = 6250;               // f of epwm2 is 50 Hz

    EPwm3Regs.AQCTLA.all = 0x0012;         //   0000 0000 0110 0000

    EPwm3Regs.CMPA.bit.CMPA = 0.425*6250;


    //    EPwm2Regs.ETSEL.bit.INTSELCMP = 0;
    //
    //    EPwm2Regs.ETSEL.bit.INTEN = 1;
    //
    //    EPwm2Regs.ETSEL.bit.INTSEL = 4;
    //
    //    EPwm2Regs.ETPS.bit.INTPRD = 1;
    //
    //    EPwm2Regs.ETCLR.bit.INT = 1;

    EPwm3Regs.TBPHS.bit.TBPHS = 0.175*6250;     // 1.75ms phase shifted to the left

    EPwm3Regs.TBCTL.bit.PHSEN = 1;

    EPwm3Regs.TBCTL.bit.SYNCOSEL = 0;

    EPwm3Regs.TBCTL.bit.PHSDIR = 1;            //count up after phase synchronized

    EPwm3Regs.ETSEL.bit.SOCAEN = 1;

    EPwm3Regs.ETCLR.bit.SOCA = 1;

    EPwm3Regs.ETPS.bit.SOCAPRD = 1;            // SoC should happen in first event itself

//    EPwm3Regs.ETSEL.bit.SOCASEL = 4;           //Soc will happen at TBCTR = CMPA when increamenting

    //    EPwm2Regs.ETSEL.bit.SOCASEL = 2;           //Soc will happen at avg

    EPwm3Regs.ETSEL.bit.SOCASEL = 5;           //Soc will happen when TBCTR = CMPA when decreamenting

}

void InitDaca(void)

{

    EALLOW;


    DacaRegs.DACCTL.bit.DACREFSEL = 1;

    DacaRegs.DACCTL.bit.LOADMODE = 0;

    DacaRegs.DACOUTEN.bit.DACOUTEN = 1;

    DacaRegs.DACVALS.bit.DACVALS = Vx;

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

extern interrupt void epwm1_isr(void)

{

    x = x + 1;

    if(x<=50){
        Vx = 10;
    }
    if(x>=50 && x<100){
        Vx = 0;
    }
    if(x>= 100 && x<180){
        Vx = -5;
    }
    if(x>=180){
        Vx = 0;
    }
    if(x == 200){
        x = 0;
    }


    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

    EPwm1Regs.ETCLR.bit.INT = 1;

}


