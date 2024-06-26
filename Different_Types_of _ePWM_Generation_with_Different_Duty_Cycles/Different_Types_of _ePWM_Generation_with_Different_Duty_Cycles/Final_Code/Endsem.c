/*

Generate EPWM 1A with 10 KHz signal

*/


#include "F28x_Project.h"

//#include "math.h"

//#define PI 3.14159265358979323846

void Gpio_select(void);

void Setup_ePWM(void);

void ConfigureADC(void);

void SetupADCSoftware(void);    // *********************************************In the real code no SetupADCSoftware

void InitDaca(void);

//void InitDacb(void);

extern interrupt void epwm1_isr(void);

//Uint16 DAC_input = 100;

//Uint16 AdcaResult0;

//float theta = 0, sinevalue, sineshifted, modindex = 1;

//float sinefreq = 0.1;     // 0.1 Hz

//int epwm_3_int_count = 0;

int x =0;

//int x=0,y=0,f=0;
//float w,z,j;
//float p=0,Voscaled,q,AdcaResult0;
//float Vo,Vo_pre = 0,Vin,Vin_pre =0;
//double R,C,Ts;


void main(void)

{

    InitSysCtrl();

    DINT;

    InitPieCtrl();

    IER = 0x0000;

    IFR = 0x0000;

    InitPieVectTable();


    EALLOW;

    CpuSysRegs.PCLKCR2.bit.EPWM1=1;    // *************************************In real code this is present outside EALLOW
    CpuSysRegs.PCLKCR2.bit.EPWM2=1;
    CpuSysRegs.PCLKCR2.bit.EPWM3=1;

    EDIS;

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;


    Gpio_select();

    Setup_ePWM();

    InitDaca();

//    InitDacb();

    ConfigureADC();

    SetupADCSoftware();          //************************************************not present in real code


    EALLOW;

    PieVectTable.EPWM1_INT = &epwm1_isr;

    EDIS;


    IER |= M_INT3;

    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;

    EINT;

    ERTM;


    while(1)

    {
//        AdcaResult0 = AdcaResultRegs.ADCRESULT0;
//        DacaRegs.DACVALS.bit.DACVALS = AdcaResult0;

    }

}


void Gpio_select(void)

{

    EALLOW;


    GpioCtrlRegs.GPAMUX1.bit.GPIO4=1;            // Setting up EPWM 3A, 3B, 4A, 4B, 5A and 5B

    GpioCtrlRegs.GPAMUX1.bit.GPIO5=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO6=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO7=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO8=1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO9=1;

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

    EPwm1Regs.TBCTL.bit.CLKDIV=1;

    EPwm1Regs.TBCTL.bit.HSPCLKDIV=1;    // f = 25 MHz

    EPwm1Regs.TBCTL.bit.CTRMODE = 2;   // up-down count

    EPwm1Regs.AQCTLA.all=0x0060;     //0000 0000 0110 0000

    EPwm1Regs.TBPRD=2500/2;           // Epwm3 freq = 10 KHz

//    EPwm3Regs.CMPA.bit.CMPA =2500/2;


//    EPwm3Regs.ETSEL.bit.SOCAEN=1;     // **************************************normally done this way
//
//    EPwm3Regs.ETSEL.bit.SOCASEL=1;    // ADC SOC when TBCTR = 0

//    EPwm1Regs.ETSEL.all = 0x0009;

//    EPwm3Regs.ETPS.bit.SOCAPRD=1;      // ***************************************normally done this way

//    EPwm1Regs.ETPS.all = 0x0005;

//    EPwm3Regs.ETCLR.bit.SOCA=1;        // **************************************normally done this way
//    EPwm1Regs.ETCLR.all = 0x0001;


    EPwm1Regs.ETSEL.bit.INTEN = 1;     // **************************************normally done this way

    EPwm1Regs.ETSEL.bit.INTSEL = 1;    // epwm interrupt when TBCTR = 0

    EPwm1Regs.ETPS.bit.INTPRD = 1;

    EPwm1Regs.ETCLR.bit.INT = 1;


//    EPwm2Regs configuration:

    EPwm2Regs.TBCTL.bit.CLKDIV = 7;

    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 6;

    EPwm2Regs.TBCTL.bit.CTRMODE = 2;    //up-down count mode

    EPwm2Regs.TBPRD = 65104/2;          // Time period = 1sec

    EPwm2Regs.CMPA.bit.CMPA = 0.25*65104;

    EPwm2Regs.AQCTLA.all = 0x0060;       // out=0 when lesser, out=1 when greater

    EPwm2Regs.TBCTL.bit.PHSEN = 0;

    EPwm2Regs.TBCTL.bit.SYNCOSEL = 1;     // used to synchronize


//    EPwm3Regs configuration:

    EPwm5Regs.TBCTL.bit.CLKDIV = 7;

    EPwm5Regs.TBCTL.bit.HSPCLKDIV = 6;

    EPwm5Regs.TBCTL.bit.CTRMODE = 2;       // up-down count mode

    EPwm5Regs.TBPRD = 65104/2;             // time period = 1 sec

    EPwm5Regs.CMPA.bit.CMPA = 0.25*65104;

    EPwm5Regs.AQCTLA.all = 0x0060;

    EPwm5Regs.TBPHS.bit.TBPHS = 0.25*65104;    // phase shifting by 25 ms

    EPwm5Regs.TBCTL.bit.PHSEN = 1;

    EPwm5Regs.TBCTL.bit.SYNCOSEL = 0;

    EPwm5Regs.TBCTL.bit.PHSDIR = 1;            //count up after phase synchronized

    EPwm5Regs.ETSEL.bit.SOCAEN = 1;

    EPwm5Regs.ETCLR.bit.SOCA = 1;

    EPwm5Regs.ETPS.bit.SOCAPRD = 1;            // SoC should happen in first event itself

    //EPwm5Regs.ETSEL.bit.SOCASEL = 4;           //Soc will happen at valley

    EPwm5Regs.ETSEL.bit.SOCASEL = 2;           //Soc will happen at avg

    //EPwm5Regs.ETSEL.bit.SOCASEL = 5;           //Soc will happen at peak


}


void InitDaca(void)

{

    EALLOW;


    DacaRegs.DACCTL.bit.DACREFSEL = 1;

    DacaRegs.DACCTL.bit.LOADMODE = 0;

    DacaRegs.DACOUTEN.bit.DACOUTEN = 1;

//    DacaRegs.DACVALS.bit.DACVALS = DAC_input;

    DELAY_US(10);


    EDIS;

}

//void InitDacb(void)
//
//{
//
//    EALLOW;
//
//
//    DacbRegs.DACCTL.bit.DACREFSEL = 1;
//
//    DacbRegs.DACCTL.bit.LOADMODE = 0;
//
//    DacbRegs.DACOUTEN.bit.DACOUTEN = 1;
//
////    DacaRegs.DACVALS.bit.DACVALS = DAC_input;
//
//    DELAY_US(10);
//
//
//    EDIS;
//
//}


void ConfigureADC(void)

{

    EALLOW;


    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;

    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;


    EDIS;


    DELAY_US(1000);

}


void SetupADCSoftware(void)                  //************************************All this done in ConfigureADC

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


    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 3;           // Done only for this code else it is 2

    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps;

    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 13;        // Done only for this code else it is 9


    EDIS;

}


extern interrupt void epwm3_isr(void)

{
    x = x + 1;

    // square wave generation

    if(x<=20){
        Vin = -10;
    }
    if(x>20){
        Vin = 10;
    }
    if(x == 40){
        x = 0;
    }


    // integrator generation

    w = ((Ts/(2*R*C)));
    z = (Vin + Vin_pre);
    Vo = Vo_pre - (w*z);
    Vo_pre = Vo;
    Vin_pre = Vin;

    Voscaled = (4095*Vo)/5;
    DacbRegs.DACVALS.bit.DACVALS = Voscaled;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    EPwm3Regs.ETCLR.all = 0x0001;

}







