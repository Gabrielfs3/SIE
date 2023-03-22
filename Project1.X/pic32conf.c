#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "pic32conf.h"
  
const uint8_t OK = 0;
const uint8_t NOK = 1;
  
  
const long int PBCLK_F_HZ = 40000000;
 
void INTconfig()
{
    TRISEbits.TRISE8 = 1;   // Set RE8 as input
    INTCONbits.INT1EP = 1;  // Generate interrupts
    
    IPC1bits.INT1IP = 2;    // Interrupt Priority
    IFS0bits.INT1IF = 0;    // Reset Int1 Interrupt Flag
    IEC0bits.INT1IE = 1;    // Enable Int1 Interrupt
}