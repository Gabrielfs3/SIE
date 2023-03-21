/*
 * File:   pic32conf.c
 * Author: Pedro Fonseca <pf@ua.pt>
 *
 * Date 02 February 2023, 08:37
 */

 
 #include "pic32conf.h"
  
 const uint8_t OK = 0;
 const uint8_t NOK = 1;
  
  
 const long int PBCLK_F_HZ = 40000000;
 
void INTconfig()
{
    TRISEbits.TRISE8 = 1;   // Set RE8 as input
    INTCONbits.INT1EP = 1;  // Generate interrupts on {rising edge - 1 falling edge - 0}
    
    IPC1bits.INT1IP = 2;    // Interrupt Priority
    IFS0bits.INT1IF = 0;    // Reset Int1 Interrupt Flag
    IEC0bits.INT1IE = 1;    // Enable Int1 Interrupt    
}