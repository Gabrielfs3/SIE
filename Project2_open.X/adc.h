/**
 * \file:   analog.h
 * \author: Pedro Fonseca <pf@ua.pt>
 *
 * \date 29 January 2023, 14:32
 * 
 * \brief Module for handling PIC32 Analog-to-Digital Converter
 *  
 */

#ifndef __analog__H_
#define __analog__H_


#include <xc.h>
#include <stdint.h>

/* Data types and structures */

/**
 * Values for the Trigger Event Source
 */
typedef enum {
    SrcManual = 0b000, /**< Conversion starts manually (clearing SAMP bit) */
    SrcInt0 = 0b001, /**< Conversion starts on active transition of INT0 */
    SrcTimer3 = 0b010, /**< Conversion starts on End-of-Count of Timer 3 */
    SrcAuto = 0b111 /**< Conversion runs continuously */
} ADCTriggerSrc_t;


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
void ADCconfig(uint8_t SourceChannel, ADCTriggerSrc_t ConversionTriggerSource, uint8_t SampleTime);

/**
 * \brief Turns ADC module off
 */
void ADCoff(void);

/**
 * \brief Turns ADC module on
 */
void ADCon(void);

/**
 * 
 * \brief Reads ADC in manual mode. 
 * Starts conversion and returns conversion value
 */
uint16_t ADCReadManual(int pin);

#endif
