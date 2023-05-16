/**
 * \file:   timer.h
 * \author: Pedro Fonseca <pf@ua.pt>
 *
 * \date 26 January 2023, 17:19
 * 
 * \brief Module for configuring the timers of PIC32
 */

#ifndef __timer__H_
#define __timer__H_

#ifndef __UNIT_TESTS__
#include <xc.h>
#else
#include "stubs/pic32dummy.h"
#endif

#include <stdint.h>
#include <stdbool.h>
#include "pic32conf.h"

/*
 * Variables and data structures
 */

/**
 * Values for PIC32 Type B timer prescaler    
 */
typedef enum {
    DIV_1=0, DIV_2, DIV_4, DIV_8, DIV_16, DIV_32, DIV_64, DIV_256
} TypeBTimerPreScalers_t;

/**
 * IDs for TypeB Timer
 */
typedef enum {
    Timer2=2, Timer3, Timer4, Timer5
} TypeBTimerNo_t; 

/**
 * Divider values for the Prescaler register in Type B Timers. 
 */
extern const uint32_t TypeBTimerPreScalerVals[]; 

/*
 * Functions
 */



/**
 * \brief Configures Timer2 registers related the counting period 
 * 
 * \param TimerOn32bit      Sets the timer to 16 bit or 32 bit count (false: 16 bit, true: 32bit).
 * \param Prescaler         Prescaler value
 * \param PR2val            Value for PR2 register
 * 
 * \sa Timer2GetEOC()
 */
void Timer2Setup(bool TimerOn32bit, TypeBTimerPreScalers_t Prescaler, uint32_t PR2val);

/**
 * \brief Starts Timer 2.
 */
void Timer2Start(void);


/**
 * \brief Stops Timer 2
 */
void Timer2Stop(void);

/**
 * \brief Gets EOC flag from Timer2
 */
int Timer2GetEOC(void);

/**
 * \brief Clears EOC flag of Timer2
 */
void Timer2ClearEOC(void);

/**
 * \brief Configures Timer3 registers related the counting period 
 * 
 * \param Prescaler         Prescaler value
 * \param PR3val            Value for PR3 register
 * 
 * \sa Timer3GetEOC()
 */
void Timer3Setup(TypeBTimerPreScalers_t Prescaler, uint32_t PR3val);


/**
 * \brief Starts Timer 3.
 */
void Timer3Start(void);


/**
 * \brief Stops Timer 3
 */
void Timer3Stop(void);



/**
 * \brief Gets EOC flag from Timer3
 */
int Timer3GetEOC(void);

/**
 * \brief Clears EOC flag of Timer3
 */
void Timer3ClearEOC(void);


/**
 * \brief Computes the configuration parameters (Prescaler and PR3 value) to configure 
 * a Type B Timer to generate events at frequency Freq_Hz (in Hz)
 * 
 * \param Freq_Hz       Desired frequency, in Hz
 * \param pPreScaler     Prescaler index (bits to be programmed in PSCK)
 * \param pPRxVal        Value for PRx 
 */
int TypeBTimer16bitGetConfigFromFreq(uint32_t Freq_Hz, TypeBTimerPreScalers_t *pPreScaler, uint32_t *pPRxVal);



/**
 * \brief Sets the counting frequency of a Type B Timer
 * 
 * If the requested frequency is not possible, the Timer registers are not 
 * changed and the function returns NOK. When the requested frequency is possible,
 * the Timer<i>x</i> (*x*=2..5) registers are set accordingly and the function 
 * returns OK. 
 * 
 * If some time number is not implemented, TypeBTimerSetFreq returns NOK
 * 
 * \param TimerNo   Number of Timer to configure
 * \param Freq_Hz   Requested frequency (in Hz)
 * \return          OK: sucess; NOK: failure
 */
int TypeBTimer16bitSetFreq(TypeBTimerNo_t TimerNo, uint32_t Freq_Hz);

#endif
