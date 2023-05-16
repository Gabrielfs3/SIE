/**
 * \file:   PWM.h
 * \author: Pedro Fonseca <pf@ua.pt>
 *
 * \date January 29, 2023, 6:11 PM
 * 
 * \brief Module to generate PWM signals. 
 * 
 * This PWM module uses Timer2 and generates output on OC2 pin. PWM is 
 * configured with PWMconfigFreq() or PWMconfigPR2(), which must be called before using the PWM signal. 
 * PWM duty-cycle is defined by calling PWMsetDutyCycle(). 
 */

#ifndef __PWM__H_
#define __PWM__H_


#include <xc.h>
#include <stdint.h>
#include "timer.h"

/**
 * \brief Configures Timer2 and OC2 module to generate a PWM signal.
 * 
 * Configuration of PWM module and Timer2 for generating a PWM signal with
 * duty-cycle defined by PWMsetDutyCycle() by setting the timer prescaler 
 * and PR2 register.. 
 * 
 * PWMconfigPR2() performs the required initialisations and turns 
 * OC2 module on. Timer2Start() must be called afterwards for starting Timer2. 
 * 
 * @param T2Prescaler       Prescaler value for Timer2
 * @param T2PR2Val          Value for PR2 register
 */
void PWMconfigPR2(TypeBTimerPreScalers_t T2Prescaler, uint16_t T2PR2Val);

/**
 * \brief Configures Timer2 and OC2 module to generate a PWM signal.
 * 
 * Configures PWM module and Timer2 for generating a PWM signal with
 * duty-cycle defined by PWMsetDutyCycle() and a frequency Freq (in Hz). 
 * 
 * PWMconfigFreq() performs the required initialisations and turns 
 * OC2 module on. Timer2Start() must be called afterwards for starting Timer2. 
 * 
 * @param Freq      PWM Frequency
 */
void PWMconfigFreq(uint16_t Freq);


/**
 * \brief Sets the duty-cycle of PWM output. 
 * 
 * @param DutyCycle     Duty-Cycle value (0 to 100)
 */
void PWMsetDutyCycle(float DutyCycle);

#endif
