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
    TRISDbits.TRISD0 = 1;   // Set RD0 as input
    ODCDbits.ODCD0=1;//ODC enable
    INTCONbits.INT0EP = 1;  // Generate interrupts
    
    IFS0bits.INT0IF = 0;    // Reset Int0 Interrupt Flag
    IPC0bits.INT0IP = 2;    // Interrupt Priority
    IEC0bits.INT0IE = 1;    // Enable Int0 Interrupt
}