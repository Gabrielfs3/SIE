#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "pwm.h"
#include "pic32conf.h"

void PWMconfigPR2(TypeBTimerPreScalers_t T2Prescaler, uint16_t T2PR2Val)
{    
    T2CONbits.TCKPS = T2Prescaler; // Define pre-scaler
    PR2 = T2PR2Val; // PR2 value
    TMR2 = 0; // Reset timer T2 count register
    OC2CONbits.OCM = 6; // OCM = 0b110 : OC2 in PWM mode,
    OC2CONbits.OCTSEL = 0; // Timer 2 is clock source of OCM
    OC2RS = 0; // Compute OC1xRS value
    OC2CONbits.ON = 1;     // Enable OC2
}

void PWMconfigFreq(uint16_t Freq)
{
    TypeBTimer16bitSetFreq(Timer2, Freq);
    
    TRISDbits.TRISD1 = 0;
    PORTDbits.RD1 = 1;
    
    OC2CONbits.OCM = 6; // OCM = 0b110 : OC2 in PWM mode,
    OC2CONbits.OCTSEL = 0; // Timer 2 is clock source of OCM
    OC2RS = 0;
    OC2CONbits.ON = 1;     // Enable OC
   
}

void PWMsetDutyCycle(int DutyCycle)
{
    if(DutyCycle >= 0 && DutyCycle <= 100)
    {
        OC2RS = ((PR2 + 1) * DutyCycle) / 100;
    }
}