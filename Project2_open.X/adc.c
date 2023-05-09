#include <xc.h>
#include <stdlib.h>
#include "adc.h"

/**
 * \brief Configures analog input 
 * 
 * \param   SourceChannel               Channel to convert (0 to 15)
 * \param   ConversionTriggerSource     Trigger source for the ADC.
 * \param   SampleTime                  Sample time for auto mode (only relevant for Auto mode).
 * \return  Nothing
 * 
 * ADCconfig assumes that all PORT B pins may be used for analog inputs. The 
 * function turns ADC module off, prior to configuration, and turns the module
 * on again, at the end. 
 * 
 * Some configurations are static: 
 * - All 16 pins (AN0 to AN15) configured to analog input.
 * - Format is Integer 16-bit 
 * - Voltage sources are AVdd and AVss
 * - No scanning of multiple inputs 
 * - Number of conversions per interrupt = 1
 * - Buffer fill mode: one 16-word buffer
 * - Always use MUX A
 * - ADC clock source is PB Clock
 * - ADC clock prescaler is set to 7
 */
void ADCconfig(uint8_t SourceChannel, ADCTriggerSrc_t ConversionTriggerSource, uint8_t SampleTime)
{
    AD1CHSbits.CH0SA = SourceChannel;
    TRISB = TRISB | 0xFFFF;
    AD1PCFG = AD1PCFG & 0x0000;
    
    // Disable JTAG interface as it uses a few ADC ports
    DDPCONbits.JTAGEN = 0;
    
    // Initialize ADC module
    // Polling mode, AN0 as input
    // Generic part
    AD1CON1bits.SSRC = ConversionTriggerSource; // Internal counter ends sampling and starts conversion
    AD1CON1bits.CLRASAM = 1; //Stop conversion when 1st A/D converter interrupt is generated and clears ASAM bit automatically
    AD1CON1bits.FORM = 0; // Integer 16 bit output format
    AD1CON2bits.VCFG = 0; // VR+=AVdd; VR-=AVss
    AD1CON2bits.SMPI = 0; // Number (+1) of consecutive conversions, stored in ADC1BUF0...ADCBUF{SMPI}
    AD1CON3bits.ADRC = 1; // ADC uses internal RC clock
    AD1CON3bits.SAMC = SampleTime; // Sample time is 16TAD ( TAD = 100ns)
    
}

void ADCoff(void)
{
    AD1CON1bits.ON = 0;
}

void ADCon(void)
{
    AD1CON1bits.ON = 1;
}

uint16_t ADCReadManual(int pin)
{ 
    AD1CON1bits.ASAM = 1; // Start conversion
    AD1CHSbits.CH0SA = pin; // choose which pin will be read
    
    while(AD1CON1bits.DONE == 0);
    
    return ADC1BUF0;
}
