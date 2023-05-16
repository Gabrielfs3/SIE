
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
bool mode = false; // auto mode off

float error_ant = 0.0, i_ant = 0.0;

/**************************************************************
     *
     * Definition of functions
     *
     */
void choose_pwm();
void menu();
void pi_controller(int set_temp, float temp);
int choose_temp();
float average(float sample);

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
    //PWMsetDutyCycle(100);
    
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
     * Print the system configuration 
     *
     */
    printf("Sampling freq: %d\r\nPWM freq: %d\r\n\n",SampFreq, PWMFreq);
    
    menu();
    
    
    /****************************************************************
     * 
     * Main cycle
     */
    char in;
    uint16_t aux;
    float r;
    float adc1, adc2, temp1, temp2, avg;
    int set_temp = 0;
    
    while (1)
    {
        while(IFS0bits.T3IF = 0);
        //printf("\r\r>> ");
        aux = ADCReadManual(0);
        adc1 = aux*3.3/1023;
        r = 3.3*330/adc1 - 330;
        temp1 = 1/(log(r/330)/3250 + (1/(25+273.15))) - 272.95; // 272.45
        avg = average(temp1);
        
        aux = ADCReadManual(1);
        adc2 = aux*3.3/1023;
        temp2 = avg + 24.376*adc2 - 0.5452;
        
        if(mode = true)
            pi_controller(set_temp,temp2);
        
        if(GetChar(&in) == UART_SUCCESS)
        {
            switch(in)
            {
                case('p'):
                    flag = false;
                    mode = false;
                    choose_pwm();
                    break;
                case('t'):
                    flag = false;
                    mode = true;
                    set_temp = choose_temp();
                    break;
                default:
                    break;
            }
        }
        if(flag == true)
        {
            //printf("\33[2K");
            printf("\r\rthermistor: %2.1f thermopar: %2.0f", temp1, temp2);
        }
    }
    return 0;
}

/********************************************************************
* Function:     pi_controller(int set_temp, float temp)
* Overview:     PI Controller to put the motor at the speed the user typed
* Inputs:       User temperature, Thermocouple temperature
* Note:		 	No notes
********************************************************************/
void pi_controller(int set_temp, float temp)
{
    float error = 0.0, p, i, d;
    float h = 1/10;
    static const float Ki = 0.1; // 0.35
    static const float Kp = 22;
    float Ti = Kp / Ki;
    
    error = set_temp - temp;
    
    p = Kp * error;
    i = h*Kp/Ti*error + i_ant;
        
    float duty = p + i;
       
    if(duty >= 100)
        duty = 100;
    else if(duty <= 0)
        duty = 0;
    
    error_ant = error;
    i_ant = i;
    
    PWMsetDutyCycle(duty);
}

/********************************************************************
* Function:     choose_pwm()
* Overview:     Get the pwm value from user
* Note:		 	No notes
********************************************************************/
void choose_pwm()
{
    char pwm1,pwm2;
    while(GetChar(&pwm1) != UART_SUCCESS);
    if(pwm1 != '0')
        while(GetChar(&pwm2) != UART_SUCCESS);
    
    int duty = atoi(&pwm1);
    printf("\n\rduty: %d\n", duty);
    PWMsetDutyCycle(duty);
    menu();
}

/********************************************************************
* Function:     choose_temp()
* Overview:     Get desired temperature from user
* Output:       The temperature entered (set_temp)
* Note:		 	No notes
********************************************************************/
int choose_temp()
{
    char temp1,temp2,temp3;
    while(GetChar(&temp1) != UART_SUCCESS);
    if(temp1 != '0')
        while(GetChar(&temp2) != UART_SUCCESS);
    if(temp1 == '1')
        while(GetChar(&temp3) != UART_SUCCESS);
    
    int set_temp = atoi(&temp1);
    printf("\n\rchoosen temperature: %d\n", set_temp);
    menu();
    return set_temp;
}

/********************************************************************
* Function:     menu()
* Overview:     show the user menu
* Note:		 	No notes
********************************************************************/
void menu()
{
    printf("\rMenu: \n");
    printf("\rp - choose the pwm\n\rt - choose the temperature\n\r");
    flag = true;
}


/********************************************************************
* Function:     average(float average)
* Overview:     Thermistor filter
* Input:        Thermistor temperature (sampled)
* Output:       Average temperature on 20 samples
* Note:		 	No notes
********************************************************************/
float average(float sample) {
    #define BUFFER_SIZE 20

    static double buffer[BUFFER_SIZE] = {0};
    static int index = 0;
    static int count = 0;
    double sum = 0.0;

    // add the sample to the circular buffer
    buffer[index] = sample;
    index = (index + 1) % BUFFER_SIZE;
    if (count < BUFFER_SIZE) {
        count++;
    }

    // compute the sum of the last BUFFER_SIZE samples
    for (int i = 0; i < count; i++) {
        sum += buffer[(index + i) % BUFFER_SIZE];
    }

    // compute and return the average
    return sum / count;
}