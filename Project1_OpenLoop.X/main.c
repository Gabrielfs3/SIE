#include "config_bits.h"
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/attribs.h>

#include "uart.h"
#include "pic32conf.h"
#include "timer.h"
#include "pwm.h"

/**************************************************************
     *
     * Definition of constants 
     *
     */
const int SampFreq = 10;       /**< Sampling frequency (in Hz) */
const int PWMFreq = 30000;       /**< PWM frequency (in Hz) */

/**************************************************************
     *
     * Definition of global variables
     *
     */
int ang = 0;    // angle based on the motor direction
int imp;        // encoder impulses
int speed = 0;  // motor actual speed

/**************************************************************
     *
     * Definition of functions
     *
     */

void change_vel();
void change_dir();
void menu();

int main(void){
    /************************************************************** 
     * 
     * Configuration section
     * 
     */

    /* Configure UART */
    UartInit(PBCLK_F_HZ, 115200);
    
    /* Set Interrupt Controller for multi-vector mode */
    INTCONSET = _INTCON_MVEC_MASK;
    
    /* RE0 as digital input - pin 37 */
    TRISEbits.TRISE0 = 1;
    
    /*
     * Set RE1 as digital output - pin 36
     * Start it ON
     */
    TRISEbits.TRISE1 = 0;
    PORTEbits.RE1 = 1;
    
    /*
     * Set Timer3 to run at required sampling frequency 
     */
    TypeBTimer16bitSetFreq(Timer3, SampFreq);
    
    /*
     * Configure PWM and stop the motor
     *
     * PWM frequency is PWMFreq
     */
    PWMconfigFreq(PWMFreq);
    PWMsetDutyCycle(0);   
    
    /*
     * Configure external interrupts
     *
     * No args.
     */
    INTconfig();
    
    /*
     * Enable Interrupt Exceptions
     *
     */
    __builtin_enable_interrupts();
    
    /****************************************************************
     * 
     * Main cycle
     */
    while (1);
    
    return 0;
}

/********************************************************************
* Overview:     External Interruptions
* Input:		Pin 2
* Note:         Usage of the Encoder CHA to get the angle and the impulses
********************************************************************/
void __ISR (_EXTERNAL_1_VECTOR) IntISR(void)
{
    if(PORTEbits.RE0 == 1)
    // angle increment of 1 degree if rotating clockwise    
        ang++; 
    else 
    // angle dencrement of 1 degree if rotating anti-clockwise
        ang--; 
    
    // 
    ang %= 360;
    
    // increment impulse counter
    imp++; 
    
    IFS0bits.INT1IF = 0;    // Reset Int1 Interrupt Flag
}

/********************************************************************
* Overview:     Timer 3 Interrupt
* Note:         Calculate the speed with the impulses of CHA
********************************************************************/
void __ISR (_TIMER_3_VECTOR) T3ISR(void)
{
    // calculate real motor speed using the inpulse counter from CHA
    speed = imp * 60 * SampFreq /360;
    // reset impulse counter
    imp = 0; 
    
    printf("RPM: %d",speed);
    
    IFS0bits.T3IF = 0; // Reset interruption flag
}
