/**! 
 * \file  mainProj0.c
 * \author Pedro Fonseca <pf@ua.pt>
 *
 * \date 26 January 2023, 17:18
 * 
 * \brief Demo program of Project 0 
 * 
 * \mainpage 
 * 
 * Example of a program for Project 0 of Electronic Instrumentation Systems. 
 * 
 * The code will read a voltage signal in AN0 pin at a sampling frequency 
 * SampFreq, deliver that value as the argument to the function 
 * transferFunction() and generate a PWM signal, where the duty-cycle is 
 * controller by the output of transferFunction() and the frequency is PWMfreq. 
 * Both SampFreq and PWMfreq are defined as constants in the main(). 
 * 
 * This example is distributed as a main file (in mainProj0.c) and the header
 * files of the modules used in the project: 
 * - timer.h
 * - analog.h
 * - PWM.h
 * - uart.h 
 * - pic32conf.h
 */

/* Configuration bits */
#include "config_bits.h"
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#include "uart.h"
#include "pic32conf.h"
#include "timer.h" 
#include "adc.h"
#include "pwm.h"

/**
 * \brief Example. 
 * 
 * The example provided is a "dummy" transfer funtion, that only performs a 
 * scale change: converts an input voltage in the range [0V..2V] applied to a 
 * ADC in the range [0V..3.3V] to a value for 0 to 100. 
 * 
 * \param inVal     Value returned by the ADC
 * \return          Converted value in the range 0..100 for a input
 */
uint16_t pwm_direct(uint16_t inVal);

/**
 * Average of n samples
 * 
 * n is defined as a const int in the function body. 
 * 
 * @param inVal     value read in the analog input (0 to 1023)
 * @return      average of the last n input values, scaled for a range 0..100.
 */
float avgNSamples(uint16_t inVal);

/**
 * \brief Example of a transfer function. 
 * 
 * The example provided is a "dummy" transfer funtion, that only performs a 
 * scale change: converts an input voltage in the range [0V..2V] applied to a 
 * ADC in the range [0V..3.3V] to a value for 0 to 3V3. 
 * 
 * \param inVal     Value returned by the ADC
 * \return          Converted value in the range 0..3V3 for a input
 */
float adc_direct(uint16_t inVal);

int main(void) {

    /**************************************************************
     *
     * Definition of constants 
     *
     */
    const int SampFreq = 200;       /**< Sampling frequency (in Hz) */
    const int PWMFreq = 2000;       /**< PWM frequency (in Hz) */

    /************************************************************** 
     * 
     * Configuration section
     * 
     */

    /* Configure UART */
    UartInit(PBCLK_F_HZ, 115200);

    printf("SIE - Project 0 demo \n\r");
    printf("%s, %s\r\n", __DATE__, __TIME__);

    TRISAbits.TRISA3 = 0;
    LATAbits.LATA3 = 1;
    
    /*
     * Set Timer3 to run at required sampling frequency 
     */
    TypeBTimer16bitSetFreq(Timer3, SampFreq);
    
    /*
     * ADC Configuration 
     * 
     * Source: Chan 0, Source: Timer3 
     */
    ADCconfig(0, SrcTimer3, 0);
    
    ADCon();
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
    
    uint16_t res, PWMval;
    float adc_value, avg;
    while (1) {
        while(IFS0bits.T3IF == 0);
                
        /* Read ADC */
        res = ADCReadRetentive();
        
        adc_value = adc_direct(res);   // Convert to 0..3.3V 
                   
        /* Compute output val */
        avg = avgNSamples(res);
                
        PWMval = pwm_direct(res);     //formula para a obter o dutycycle de 0 a 100 
                
        printf("\rAVG: %1.2f - ADC VALUE: %1.2f - PWM VALUE : %d",avg, adc_value, PWMval);
        /* Set output */
        PWMsetDutyCycle(PWMval);
        
        IFS0bits.T3IF = 0;
        
        /* Toggle control pin at sampling frequency */
        LATAbits.LATA3 = !LATAbits.LATA3;
    }
}


/**
 * Direct transfer function 
 * 
 * "Echoes" the input value to the output, adjusting the output range to 0..100
 * 
 * @param inVal     value read in the analog input (0 to 1023)
 * @return          input value, range adjusted to 0..100
 */
uint16_t pwm_direct(uint16_t inVal)
{
    return inVal*100/1024; 
}

/**
 * Average of n samples
 * 
 * n is defined as a const int in the function body. 
 * 
 * @param inVal     value read in the analog input (0 to 1023)
 * @return      average of the last n input values, scaled for a range 0..100.
 */
float avgNSamples(uint16_t inVal) {
    
#define nSamples 10
    
    float val; 
    
    static float Samples[nSamples];
    static int pointer; 
    
    /*
     * Move pointer to next position 
     */
    pointer = (pointer+1) % nSamples; 
    
    /* Update Samples array */
    Samples[pointer] = inVal*100/3.3; 
    
    /* Compute the average */
    val = 0; 
    int i; 
    for(i=0; i< nSamples; i++){
        val += Samples[i];
    }
   
    return val/nSamples; 
}

/**
 * Direct transfer function 
 * 
 * "Echoes" the input value to the output, adjusting the output range to 0..3V3
 * 
 * @param inVal     value read in the analog input (0 to 1023)
 * @return          input value, range adjusted to 0..3V3
 */
float adc_direct(uint16_t inVal)
{
    return inVal*3.3/1024;
}