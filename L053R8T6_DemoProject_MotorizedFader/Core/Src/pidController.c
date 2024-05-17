/*
 * pidController.c
 *
 *  Created on: Sep 5, 2023
 *      Author: Mario Niehren
 */

#include <pidController.h>

/*
 *  BEGIN: Functions for initialization and Setup
 */
void pidController_setValueMax(pidController_TypeDef* pid, int value)
{
  pid->value_max = value;
}

void pidController_setValueMin(pidController_TypeDef* pid, int value)
{
  pid->value_min = value;
}

void pidController_setKp(pidController_TypeDef* pid, double value)
{
  pid->Kp = value;
}

void pidController_setKi(pidController_TypeDef* pid, double value)
{
  pid->Ki = value;
}

void pidController_setKd(pidController_TypeDef* pid, double value)
{
  pid->Kd = value;
}

void pidController_setHysteresis(pidController_TypeDef* pid, uint16_t value)
{
  pid->hysteresis = value;
}

void pidController_setLowPass(pidController_TypeDef* pid, double value)
{
  pid->tauLowPass = value;
}

void pidController_setSampleTime(pidController_TypeDef* pid, uint16_t value)
{
  pid->sampleTime = value;
}

void pidController_init(pidController_TypeDef* pid)
{
  pid->differentiator = 0.0;
  pid->error_old = 0.0;
  pid->integrator = 0.0;
  pid->output = 0.0;
  pid->sample_old = 0.00;
}

void pidController_setTarget(pidController_TypeDef* pid, uint16_t setpoint)
{
  pid->setpoint = setpoint;
}
/*
 *  END: Functions for initialization and Setup
 */

/*
 *  BEGIN: Functions to compute PID
 */
bool check_hysteresis(pidController_TypeDef* pid);
void reset_outputIfTimerEllapsed(pidController_TypeDef* pid);
void reset_hysteresis(pidController_TypeDef* pid);
void compute_error(pidController_TypeDef* pid);
void compute_proportional(pidController_TypeDef* pid);
void compute_integrator(pidController_TypeDef* pid);
void compute_differentiator(pidController_TypeDef* pid);
void compute_output(pidController_TypeDef* pid);
void store_errorAndSample(pidController_TypeDef* pid);

double pidController_compute(pidController_TypeDef* pid, uint16_t sample)
{
  pid->timer.threshold = pid->sampleTime;
  pid->sample = sample;
  if(timer_check_TimerElapsed(&pid->timer) && (pid->blocked == false))
  {
    /* check hysteresis */
    if(check_hysteresis(pid) == true)
    {
      reset_outputIfTimerEllapsed(pid);
    }
    else
    {
      reset_hysteresis(pid);
      compute_error(pid);
      compute_proportional(pid);
      compute_integrator(pid);
      compute_differentiator(pid);
      compute_output(pid);
      store_errorAndSample(pid);
    }
  }
  return pid->output;
}

bool check_hysteresis(pidController_TypeDef* pid)
{
  bool returnValue = false;
  if((pid->sample <= (pid->setpoint + pid->hysteresis)) && (pid->sample >= (pid->setpoint - pid->hysteresis)))
  {
    returnValue = true;
  }
  return returnValue;
}


void reset_outputIfTimerEllapsed(pidController_TypeDef* pid)
{
  pid->timerHyst.threshold = 0x05;
  if(timer_check_TimerElapsed(&pid->timerHyst) == true)
  {
    pid->output = 0.00f;
  }
}

void reset_hysteresis(pidController_TypeDef* pid)
{
  timer_reset(&pid->timerHyst);
}
void compute_error(pidController_TypeDef* pid)
{
  pid->error = pid->setpoint - pid->sample;
}
void compute_proportional(pidController_TypeDef* pid)
{
  pid->proportional = pid->Kp * pid->error;
}

/*
 * BEGIN: Functions for the integrator
 */
void compute_integratorAntiWindup(pidController_TypeDef* pid);
void clampIntegrator(pidController_TypeDef* pid);
void compute_integrator(pidController_TypeDef* pid)
{
  pid->integrator = pid->integrator + 0.5 * pid->Ki * pid->sampleTime * (pid->error + pid->error_old);
  compute_integratorAntiWindup(pid);
  clampIntegrator(pid);
}
void compute_integratorAntiWindup(pidController_TypeDef* pid)
{
  if(pid->value_max > pid->proportional)
    {
      pid->integrator_max = pid->value_max + pid->proportional;
    }
    else
    {
      pid->integrator_max = 0.0;
    }
    if(pid->value_min < pid->proportional)
    {
      pid->integrator_min = pid->value_min - pid->proportional;
    }
    else
    {
      pid->integrator_min = 0.0;
    }
}
void clampIntegrator(pidController_TypeDef* pid)
{
  if(pid->integrator > pid->integrator_max)
  {
    pid->integrator = pid->integrator_max;
  }
  else if(pid->integrator < pid->integrator_min)
  {
    pid->integrator = pid->integrator_min;
  }
}
/*
 * END: Functions for the integrator
 */

/*
 * BEGIN: Functions for the differentiator
 */
double compute_Numerator_differentiatorPortion(pidController_TypeDef* pid);
double compute_Numerator_lowPassPortion(pidController_TypeDef* pid);
void compute_differentiator(pidController_TypeDef* pid)
{
  pid->differentiator = (compute_Numerator_differentiatorPortion(pid) + compute_Numerator_lowPassPortion(pid))
                      / (2.0 * pid->tauLowPass + pid->sampleTime);
}

double compute_Numerator_differentiatorPortion(pidController_TypeDef* pid)
{
  double returnValue = 0.00;
  returnValue = 2.0 * pid->Kd * (pid->sample - pid->sample_old);
  return returnValue;
}

double compute_Numerator_lowPassPortion(pidController_TypeDef* pid)
{
  double returnValue = 0.00;
  returnValue = (2.0 * pid->tauLowPass - pid->sampleTime) * pid->differentiator;
  return returnValue;
}

/*
 * END: Functions for the differentiator
 */

/*
 * BEGIN: Functions for the pid output
 */
void clamp_output(pidController_TypeDef* pid);
void compute_output(pidController_TypeDef* pid)
{
  pid->output = pid->proportional + pid->integrator + pid->differentiator;
  clamp_output(pid);
}
void clamp_output(pidController_TypeDef* pid)
{
  if(pid->output > pid->value_max)
  {
    pid->output = pid->value_max;
  }
  else if(pid->output < pid->value_min)
  {
    pid->output = pid->value_min;
  }
}
/*
 * END: Functions for the pid output
 */

/*
 * BEGIN: Functions for value storage
 */
void store_errorAndSample(pidController_TypeDef* pid)
{
  pid->error_old = pid->error;
  pid->sample_old = pid->sample;
}
/*
 * END: Functions for value storage
 */

/*
 *  BEGIN: Functions to compute PID
 */

void pidController_reset(pidController_TypeDef* pid)
{
  pid->sample = 0x00;
  pid->sample_old = 0x00;
  pid->proportional = 0.00;
  pid->integrator = 0.00;
  pid->integrator_min = 0.00;
  pid->integrator_max = 0.00;
  pid->differentiator = 0.00;
  pid->error = 0.00;
  pid->error_old = 0.00;
  pid->output = 0.00;
}
