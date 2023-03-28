#include <xc.h>
#include <stdlib.h>
#include <math.h>
#include "timer.h"
#include "pic32conf.h"

void Timer2Setup(bool TimerOn32bit, TypeBTimerPreScalers_t Prescaler, uint32_t PR2val)
{
    if (TimerOn32bit == true)
        T2CONbits.T32 = 1; // 32 bit timer operation
    else if (TimerOn32bit == false)
        T2CONbits.T32 = 0; // 16 bit timer operation
    
    T2CONbits.TCKPS = Prescaler; // Define pre-scaler
    PR2 = PR2val; // PR2 value
    TMR2 = 0; // Reset timer T2 count register
}

void Timer2Start(void)
{
    T2CONbits.TON = 1; // Start the timer
}

void Timer2Stop(void)
{
    T2CONbits.TON = 0; // Stop the timer
}

int Timer2GetEOC(void)
{
    return IFS0bits.T2IF;
}

void Timer2ClearEOC(void)
{
    IFS0bits.T2IF=0; // Reset interrupt flag
}

void Timer3Setup(TypeBTimerPreScalers_t Prescaler, uint32_t PR3val)
{
    T3CONbits.TCKPS = Prescaler; // Define pre-scaler
    PR3 = PR3val; // PR3 value
    TMR3 = 0; // Reset timer T3 count register
    
    IPC3bits.T3IP = 2;		// Interrupt Priority
    IFS0bits.T3IF = 0;		// Reset Timer 3 Interrupt Flag
    IEC0bits.T3IE = 1;		// Enable Timer 3 Interrupt
}

void Timer3Start(void)
{
    T3CONbits.TON = 1; // Start the timer
}

void Timer3Stop(void)
{
    T3CONbits.TON = 0; // Stop the timer
}

int Timer3GetEOC(void)
{
    return IFS0bits.T3IF;
}

void Timer3ClearEOC(void)
{
    IFS0bits.T3IF=0; // Reset interrupt flag
}


/**
 * \brief Computes the configuration parameters (Prescaler and PR3 value) to configure 
 * a Type B Timer to generate events at frequency Freq_Hz (in Hz)
 * 
 * \param Freq_Hz       Desired frequency, in Hz
 * \param pPreScaler     Prescaler index (bits to be programmed in PSCK)
 * \param pPRxVal        Value for PRx 
 */
int TypeBTimer16bitGetConfigFromFreq(uint32_t Freq_Hz, TypeBTimerPreScalers_t *pPreScaler, uint32_t *pPRxVal)
{
    
}



/**
 * \brief Sets the counting frequency of a Type B Timer
 * 
 * If the requested frequency is not possible, the Timer registers are not 
 * changed and the function returns NOK. When the requested frequency is possible,
 * the Timer<i>x</i> (*x*=2..5) registers are set accordingly and the function 
 * returns OK. 
 * 
 * If some time number is not implemented, TypeBTimerSetFreq returns NOK
 * 
 * \param TimerNo   Number of Timer to configure
 * \param Freq_Hz   Requested frequency (in Hz)
 * \return          OK: sucess; NOK: failure
 */
int TypeBTimer16bitSetFreq(TypeBTimerNo_t TimerNo, uint32_t Freq_Hz)
{
    
    int PRx, kpre, TCKPS, list[]={1,2,4,8,16,32,64,256};
    
    kpre = PBCLK_F_HZ/((65535-1)*Freq_Hz);
    
    for(int i = 0; i <= 7; i++)
    {
        if (kpre < list[i])
        {
            kpre = list[i];
            TCKPS = i;
            break;
        }
    }
    
    PRx = (PBCLK_F_HZ/kpre)/10-1;
    
    switch(TimerNo){
        case Timer2:
            Timer2Setup(false,TCKPS,PRx);
            Timer2Start();
            break;
        case Timer3:
            Timer3Setup(TCKPS,PRx);
            Timer3Start();
            break;
    }
    return 0;
}
