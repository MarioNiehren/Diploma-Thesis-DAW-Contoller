/***************************************************************************//**
 * @defgroup				MotorFader	Motorized Fader
 * @brief						This module can be used to program an Fader with DC-Motor.
 *
 * motorizedFader.h
 *
 *  Created on: Sep 4, 2023
 *      Author: Mario Niehren
 ******************************************************************************/
#ifndef INC_FADER_MOTORIZEDFADER_H_
#define INC_FADER_MOTORIZEDFADER_H_

#include "main.h"

#include <math.h>
#include <wiper.h>
#include "motorDC.h"
#include "pidController.h"
#include "tscFader.h"

#define NUMBER_OF_FADERS 8

typedef struct
{
  Wiper_structTd  whiper;
  motorDC_TypeDef motor;
  pidController_TypeDef pidControl;
  tscFader_TypeDef touchSensor;
  uint16_t targetValue;

}motorizedFader_TypeDef;

void motorizedFader_initWhiper_LimitsMinMax(motorizedFader_TypeDef* fader, uint16_t valueMin, uint16_t valueMax);
void motorizedFader_initWhiper_MuxAddress(motorizedFader_TypeDef* fader, uint8_t muxAddress);
void motorizedFader_init_Whiper_ADC(motorizedFader_TypeDef* fader, ADC_HandleTypeDef* handle);
void motorizedFader_initMotor_PWM(motorizedFader_TypeDef* fader, TIM_HandleTypeDef* handle, uint16_t pwmChannel);


void motorizedFader_initPID_KpKiKd(motorizedFader_TypeDef* fader, double Kp, double Ki, double Kd);
void motorizedFader_initPID_ValueLimitMinMax(motorizedFader_TypeDef* fader, int valueMin, int valueMax);
void motorizedFader_initPID_TauLowPass(motorizedFader_TypeDef* fader, double tauLowPass);
void motorizedFader_initPID_SampleTime(motorizedFader_TypeDef* fader, uint16_t sampleTime);
void motorizedFader_initPID_Hysteresis(motorizedFader_TypeDef* fader, uint16_t hystThresh);
void motorizedFader_initPID_StructDefault(motorizedFader_TypeDef* fader);

void motorizedFader_initTouchSensor_MuxAddress(motorizedFader_TypeDef* fader, uint8_t muxAddress);
void motorizedFader_initTouchSensor_Threshold(motorizedFader_TypeDef* fader, uint16_t threshValue);

void motorizedFader_start(motorizedFader_TypeDef* fader);

void motorizedFader_update(motorizedFader_TypeDef* fader, uint8_t numFader);
void motorizedFader_moveFaderToValue(motorizedFader_TypeDef* fader, uint16_t value);
void motorizedFader_moveFaderToValueFixedCCR(motorizedFader_TypeDef* fader, uint16_t target, uint16_t CCR);
uint16_t motorizedFader_returnWhiperValue(motorizedFader_TypeDef* fader);

#endif /* INC_FADER_MOTORIZEDFADER_H_ */
