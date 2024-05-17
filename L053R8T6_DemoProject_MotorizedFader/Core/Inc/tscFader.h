/*
 * tscFader.h
 *
 *  Created on: Sep 6, 2023
 *      Author: Mario Niehren
 */

#ifndef INC_PERIPHERALS_FADER_TSCFADER_H_
#define INC_PERIPHERALS_FADER_TSCFADER_H_

#include "gpio.h"
#include "tsc.h"
#include <stdbool.h>
typedef enum
{
  tsl_touched = 1,
  tsl_released = 0
}tslFader_State_TypeDef;
typedef struct
{
  tslFader_State_TypeDef state;
  uint8_t muxAddress;
  uint16_t threshold;
}tscFader_TypeDef;

extern bool tsl_user_interruptConvCplt;


void tscFader_initMuxAddress(tscFader_TypeDef* tsc, uint8_t muxAddress);
void tscFader_initThreshold(tscFader_TypeDef* tsc, uint16_t value);

void tscFader_startMux();

void tscFader_Discharge();
void tscFader_start_IT();
void tscFader_stop_IT();
void tscFader_start_IT();
void tscFader_updateState(tscFader_TypeDef* tsc);
tslFader_State_TypeDef tscFader_getState();

bool tscFader_checkIfInterrupted();
void tscFader_resetInterrupt();

#endif /* INC_PERIPHERALS_FADER_TSCFADER_H_ */
