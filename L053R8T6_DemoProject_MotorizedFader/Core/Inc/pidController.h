/*
 * pidController.h
 *
 *  Created on: Sep 5, 2023
 *      Author: Mario Niehren
 */

#ifndef INC_PERIPHERALS_FADER_PIDCONTROLLER_H_
#define INC_PERIPHERALS_FADER_PIDCONTROLLER_H_

#include "main.h"
#include "timer.h"

typedef struct
{
  double Kp;
  double Ki;
  double Kd;
  uint16_t setpoint;
  int  value_min;
  int  value_max;
  uint16_t  sampleTime;
  uint16_t sample;
  uint16_t sample_old;
  double tauLowPass;
  double proportional;
  double integrator;
  double integrator_min;
  double integrator_max;
  double differentiator;
  double error;
  double error_old;
  double output;
  uint16_t hysteresis;
  bool blocked;
  timer_setup_structTd timer;
  timer_setup_structTd timerHyst;
}pidController_TypeDef;

void pidController_setValueMax(pidController_TypeDef* pid, int value);
void pidController_setValueMin(pidController_TypeDef* pid, int value);
void pidController_setKp(pidController_TypeDef* pid, double value);
void pidController_setKi(pidController_TypeDef* pid, double value);
void pidController_setKd(pidController_TypeDef* pid, double value);
void pidController_setTarget(pidController_TypeDef* pid, uint16_t setpoint);
void pidController_setLowPass(pidController_TypeDef* pid, double value);
void pidController_setSampleTime(pidController_TypeDef* pid, uint16_t value);
void pidController_setHysteresis(pidController_TypeDef* pid, uint16_t value);

void pidController_init(pidController_TypeDef* pid);

double pidController_compute(pidController_TypeDef* pid, uint16_t sample);
void pidController_reset(pidController_TypeDef* pid);

#endif /* INC_PERIPHERALS_FADER_PIDCONTROLLER_H_ */
