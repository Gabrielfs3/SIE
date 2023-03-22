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

int rpm; // rpm inserted by user
int ang = 0; // angle based on the motor direction
int imp; // encoder impulses
int speed = 0; // motor actual speed
char dir;

/**************************************************************
     *
     * Definition of functions
     *
     */

uint16_t pwm_direct(uint16_t inVal);
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
    printf("%s, %s\r\n", __DATE__, __TIME__);
    
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
    while (1)
    {
        char in;
        printf("\r>> ");
        if(GetChar(&in) == UART_SUCCESS)
        {
            switch(in)
            {
                case('p'):
                    printf("p");
                    PORTEbits.RE1 = 1;
                    break;
                case('s'):
                    printf("s");
                    PORTEbits.RE1 = 0;
                    break;
                case('v'):
                    printf("v");
                    change_vel();
                    break;
                case('d'):
                    printf("d");
                    change_dir();
                    break;
            }
        }
        else
        {
            
        }
    }
    return 0;
}

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

// Timer 3 Interrupt - 10 Hz
void __ISR (_TIMER_3_VECTOR) T3ISR(void)
{
    speed = imp * 60 * SampFreq /360;
    imp = 0; // reset impulse counter
    
    IFS0bits.T3IF = 0; // Reset interruption flag
}

uint16_t pwm_direct(uint16_t inVal)
{
    return inVal*100/1024; 
}

void change_vel()
{
    char rpm1,rpm2;
    printf("\n\r'0' will stop the motor\n\rRPM = ");
    while(GetChar(&rpm1) != UART_SUCCESS);
    while(GetChar(&rpm2) != UART_SUCCESS);
    rpm = atoi(&rpm1);
    if(rpm > 50)
        rpm = 50;
    else if(rpm < 10)
        rpm = 0;
    printf("%d\n",rpm);
    menu();
}

void change_dir()
{
    printf("\n\rDir (+/-) = ");
    while(GetChar(&dir) != UART_SUCCESS);
    printf("%s\n",&dir);
    menu();
}

void menu()
{
    system("clear");
    printf("\rMenu: \n");
    printf("\rp - start the circuit\n\rs - stop the circuit\n\rv - motor velocity\n\rd - motor direction\n\r");
}