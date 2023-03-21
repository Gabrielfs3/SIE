/* Configuration bits */
#include "config_bits.h"
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/attribs.h>

#include "uart.h"
#include "pic32conf.h"
#include "timer.h"
#include "pwm.h"

uint16_t pwm_direct(uint16_t inVal);

int main(void) {

    /**************************************************************
     *
     * Definition of constants 
     *
     */
    const int SampFreq = 100;       /**< Sampling frequency (in Hz) */
    const int PWMFreq = 100000;       /**< PWM frequency (in Hz) */

    /************************************************************** 
     * 
     * Configuration section
     * 
     */

    /* Configure UART */
    UartInit(PBCLK_F_HZ, 115200);
    printf("%s, %s\r\n", __DATE__, __TIME__);
    
    // RE7 as digital input
    TRISEbits.TRISE7 = 1;

    /*
     * Set Timer3 to run at required sampling frequency 
     */
    TypeBTimer16bitSetFreq(Timer3, SampFreq);
    
    /*
     * Configure PWM
     *
     * PWM frequency is PWMFreq 
     */
    PWMconfigFreq(PWMFreq);
    
    /*
     * Print the system configuration 
     *
     */
    printf("Sampling freq: %d\r\nPWM freq.: %d\r\n\n",SampFreq, PWMFreq);
    
    /****************************************************************
     * 
     * Main cycle
     */
    while (1);
}

uint16_t pwm_direct(uint16_t inVal)
{
    return inVal*100/1024; 
}