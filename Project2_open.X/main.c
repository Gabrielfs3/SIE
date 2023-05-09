
#include "config_bits.h"
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/attribs.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "uart.h"
#include "pic32conf.h"
#include "adc.h"
#include "timer.h"
#include "pwm.h"

/**************************************************************
     *
     * Definition of constants 
     *
     */
const int SampFreq = 10;        /**< Sampling frequency (in Hz) */
const int PWMFreq = 20000;       /**< PWM frequency (in Hz) */

/**************************************************************
     *
     * Definition of global variables
     *
     */

bool flag = true; // just a print flag for the menu.

static const float Ki = 0.00001;
static const float Kp = 0.008;

/**************************************************************
     *
     * Definition of functions
     *
     */
void choose_pwm();
void menu();
void pi_controller(int set_temp, float temp);
int choose_temp();

int main(void){
    /************************************************************** 
     * 
     * Configuration section
     * 
     */

    /* Configure UART */
    UartInit(PBCLK_F_HZ, 115200);
    printf("\n\n\r%s, %s\r\n", __DATE__, __TIME__);
    
    /* Set Interrupt Controller for multi-vector mode */
    INTCONSET = _INTCON_MVEC_MASK;
            
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
    PWMsetDutyCycle(100);
    
    /*
     * ADC Configuration 
     * 
     * Source: Chan 0, Source: Timer3 
     */
    ADCconfig(0, SrcTimer3, 16);
    
    ADCon();
    
    /*
     * ADC Configuration 
     * 
     * Source: Chan 1, Source: Timer3 
     */
    ADCconfig(1, SrcTimer3, 16);
    
    ADCon();
    
    /*
     * Enable Interrupt Exceptions
     *
     */
    //__builtin_enable_interrupts();
    
    /*
     * Print the system configuration 
     *
     */
    printf("Sampling freq: %d\r\nPWM freq: %d\r\n\n",SampFreq, PWMFreq);
    
    //menu();
    
    
    /****************************************************************
     * 
     * Main cycle
     */
    char in;
    uint16_t aux;
    float r;
    float adc1, adc2, temp1, temp2;
    int duty=0;
    int i;
    
    while (1)
    {
        while(IFS0bits.T3IF = 0);
        //printf("\r\r>> ");
        aux = ADCReadManual(0);
        adc1 = aux*3.3/1023;
        r = 3.3*330/adc1 - 330;
        temp1 = 1/(log(r/330)/3250 + (1/(25+273.15))) - 272.45;

        aux = ADCReadManual(1);
        adc2 = aux*3.3/1023;
        temp2 = temp1 + 24.376*adc2 + 0.5452;
        
        printf("\r%2.1f,%2.1f\n", temp1, temp2);
    }
    return 0;
}