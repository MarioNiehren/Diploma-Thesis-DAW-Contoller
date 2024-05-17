/*
 * motorizedFader.c
 *
 *  Created on: Sep 4, 2023
 *      Author: Mario Niehren
 *
 */

#include <motorizedFader.h>


/*
 * BEGIN: Functions for initialization
 */
void motorizedFader_initWhiper_LimitsMinMax(motorizedFader_TypeDef* fader, uint16_t valueMin, uint16_t valueMax)
{
  whiper_init_valueLimitsMinMax(&fader->whiper, valueMin, valueMax);

}
void motorizedFader_initWhiper_MuxAddress(motorizedFader_TypeDef* fader, uint8_t muxAddress)
{
  whiper_init_muxAddress(&fader->whiper, muxAddress);
}

void motorizedFader_init_Whiper_ADC(motorizedFader_TypeDef* fader, ADC_HandleTypeDef* handle)
{
  Wiper_init_ADC(&fader->whiper, fader->whiper.handle);
}

void motorizedFader_initMotor_PWM(motorizedFader_TypeDef* fader, TIM_HandleTypeDef* handle, uint16_t pwmChannel)
{
  motorDC_init_PWM(&fader->motor, handle, pwmChannel);
}


void motorizedFader_initPID_KpKiKd(motorizedFader_TypeDef* fader, double Kp, double Ki, double Kd)
{
  pidController_setKp(&fader->pidControl, Kp);
  pidController_setKi(&fader->pidControl, Ki);
  pidController_setKd(&fader->pidControl, Kd);
}

void motorizedFader_initPID_ValueLimitMinMax(motorizedFader_TypeDef* fader, int valueMin, int valueMax)
{
  pidController_setValueMin(&fader->pidControl, valueMin);
  pidController_setValueMax(&fader->pidControl, valueMax);
}

void motorizedFader_initPID_TauLowPass(motorizedFader_TypeDef* fader, double tauLowPass)
{
  pidController_setLowPass(&fader->pidControl, tauLowPass);
}
void motorizedFader_initPID_StructDefault(motorizedFader_TypeDef* fader)
{
  pidController_init(&fader->pidControl);
}

void motorizedFader_initTouchSensor_MuxAddress(motorizedFader_TypeDef* fader, uint8_t muxAddress)
{
  tscFader_initMuxAddress(&fader->touchSensor, muxAddress);
}

void motorizedFader_initTouchSensor_Threshold(motorizedFader_TypeDef* fader, uint16_t threshValue)
{
  tscFader_initThreshold(&fader->touchSensor, threshValue);
}

void motorizedFader_initPID_SampleTime(motorizedFader_TypeDef* fader, uint16_t sampleTime)
{
  pidController_setSampleTime(&fader->pidControl, sampleTime);
}

void motorizedFader_initPID_Hysteresis(motorizedFader_TypeDef* fader, uint16_t hystThresh)
{
  pidController_setHysteresis(&fader->pidControl, hystThresh);
}
/*
 * END: Functions for initialization
 */

/*
 * BEGIN: Functions to start fader components
 */
bool sharedComponentsStarted = false;
void motorizedFader_start(motorizedFader_TypeDef* fader)
{
  motorDC_startPwmTimer(&fader->motor);
  if(sharedComponentsStarted == false)
  {
    whiper_start(&fader->whiper);
    tscFader_startMux(&fader->whiper);
    tscFader_Discharge();
    tscFader_start_IT();
    sharedComponentsStarted = true;
  }
}
/*
 * END: Functions to start fader components
 */

/*
 * BEGIN: Functions to update faders
 */

void resetAndBlockPidIfFaderTouched(motorizedFader_TypeDef* fader);
void update_MuxedWhiperAndTouch(motorizedFader_TypeDef* fader, uint8_t numFader);
void motorizedFader_update(motorizedFader_TypeDef* fader, uint8_t numFader)
{
  for(int i = 0; i < numFader; i++)
  {
    calculate_SmoothValue(&fader->whiper);
    resetAndBlockPidIfFaderTouched(&fader[i]);
    fader[i].pidControl.output = pidController_compute(&fader[i].pidControl, fader[i].whiper.ValueSmooth);
  }
  update_MuxedWhiperAndTouch(fader, numFader);
}

uint8_t muxWhiper_AddressCounter = 0x00;
uint8_t muxTouch_AddressCounter = 0x00;
void update_MuxedWhiperAndTouch(motorizedFader_TypeDef* fader, uint8_t numFader)
{
  if(whiper_checkIfInterrupted(&fader->whiper) == true)
  {
    whiper_stop_IT(&fader->whiper);
    whiper_updateSample(&fader[muxWhiper_AddressCounter].whiper);
    muxWhiper_AddressCounter ++;
    if(muxWhiper_AddressCounter == numFader)
    {
      muxWhiper_AddressCounter = 0;
    }
    whiper_resetInterrupt(&fader->whiper);
    whiper_start(&fader->whiper);
  }
  if(tscFader_checkIfInterrupted() == true)
  {
    tscFader_stop_IT();
    tscFader_updateState(&fader[muxTouch_AddressCounter].touchSensor);
    muxTouch_AddressCounter ++;
    if(muxTouch_AddressCounter == numFader)
    {
      muxTouch_AddressCounter = 0;
    }
    tscFader_resetInterrupt();
    tscFader_Discharge();
    tscFader_start_IT();
  }
}

void resetAndBlockPidIfFaderTouched(motorizedFader_TypeDef* fader)
{
  if((fader->touchSensor.state == tsl_touched) && (fader->pidControl.blocked == false))
  {
    pidController_reset(&fader->pidControl);
    fader->pidControl.blocked = true;
  }
  else if((fader->touchSensor.state == tsl_released) && (fader->pidControl.blocked == true))
  {
    fader->pidControl.blocked = false;
  }
}
/*
 * END: Functions to update faders
 */

/*
 * BEGIN: Functions to access faders
 */
void motorizedFader_moveFaderToValue(motorizedFader_TypeDef* fader, uint16_t value)
{
  pidController_setTarget(&fader->pidControl, value);

  int newCCR = round(fader->pidControl.output);

  if(newCCR > 0 && (fader->touchSensor.state == tsl_released))
  {
    motorDC_moveUp(&fader->motor, newCCR);
  }
  else if(newCCR < 0 && (fader->touchSensor.state == tsl_released))
  {
    uint16_t newCCR_amount = -1 * newCCR;
    motorDC_moveDown(&fader->motor, newCCR_amount);
  }
  else
  {
    motorDC_stopMoving(&fader->motor);
  }
}

void motorizedFader_moveFaderToValueFixedCCR(motorizedFader_TypeDef* fader, uint16_t target, uint16_t CCR)
{
  if((fader->whiper.ValueSmooth < target) && (fader->touchSensor.state == tsl_released))
  {
    motorDC_moveUp(&fader->motor, CCR);
  }
  else if((fader->whiper.ValueSmooth > target) && (fader->touchSensor.state == tsl_released))
  {
    motorDC_moveDown(&fader->motor, CCR);
  }
  else
  {
    motorDC_stopMoving(&fader->motor);
  }
}


uint16_t motorizedFader_returnWhiperValue(motorizedFader_TypeDef* fader)
{
  uint16_t returnValue = fader->whiper.ValueSmooth;
  return returnValue;
}

/*
 * BEGIN: Functions to access faders
 */
