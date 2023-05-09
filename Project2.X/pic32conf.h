/**
 * \file:   pic32conf.h
 * \author: Pedro Fonseca <pf@ua.pt>
 *
 * \date 02 February 2023, 08:38
 */

#ifndef __pic32conf__H_
#define __pic32conf__H_

#include <stdint.h>

extern const long int PBCLK_F_HZ;       /**< Peripheral Bus Clock frequency */


extern const uint8_t OK;                /**< Result is OK */
extern const uint8_t NOK;               /**< Result is not OK */

// Configure the external interrupts on RE8
void INTconfig();

#endif