
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
const int SampFreq = 10;        /**< Sampling frequency (in Hz) */
const int PWMFreq = 30000;       /**< PWM frequency (in Hz) */

/**************************************************************
     *
     * Definition of global variables
     *
     */

char dir; // direction inserted by user
int rpm; // rpm inserted by user
int ang = 0; // angle based on the motor direction
int imp; // encoder impulses
int speed = 0; // motor actual speed

bool flag = true;

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
    PWMsetDutyCycle(16);            // 10rpm - 60% duty ; 50rpm - 85% duty
    
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
        //printf("\r\r>> ");
        if(GetChar(&in) == UART_SUCCESS)
        {
            switch(in)
            {
                case('p'):
                    PORTEbits.RE1 = 1;
                    break;
                case('s'):
                    PORTEbits.RE1 = 0;
                    break;
                case('v'):
                    flag = false;
                    change_vel();
                    break;
                case('d'):
                    flag = false;
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
    ang = ang % 360;
    
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
    printf("\33[2K");
    if(flag == true)
        printf("\r\rRPM: %2d  ANGLE: %3d",speed ,ang);
        
    IFS0bits.T3IF = 0; // Reset interruption flag
}

/********************************************************************
* Function:     change_vel()
* Overview:     Get motor speed from user
* Note:		 	No notes
********************************************************************/
void change_vel()
{
    char rpm1,rpm2;
    printf("\n\rBelow 10 will stop the motor\n\rChoose velocity: ");
    while(GetChar(&rpm1) != UART_SUCCESS);
    printf("%c",rpm1);
    if(rpm1 != '0')
        while(GetChar(&rpm2) != UART_SUCCESS);
    printf("%c",rpm2);
    
    rpm = atoi(&rpm1);
    if(rpm > 50)
        rpm = 50;
    else if(rpm < 10)
    {
        speed = 0;
        ang = 0;
        PWMsetDutyCycle(50); // stop the motor
    }
    menu();
}

/********************************************************************
* Function:     change_dir()
* Overview:     Get motor direction from user
* Note:		 	No notes
********************************************************************/
void change_dir()
{
    printf("\n\rDir (+/-) = ");
    while(GetChar(&dir) != UART_SUCCESS);
    printf("%s\n",&dir);
    menu();
}

/********************************************************************
* Function:     menu()
* Overview:     show the user menu
* Note:		 	No notes
********************************************************************/
void menu()
{
    printf("\rMenu: \n");
    printf("\rp - start the circuit\n\rs - stop the circuit\n\rv - motor velocity\n\rd - motor direction\n\r");
    flag = true;
}