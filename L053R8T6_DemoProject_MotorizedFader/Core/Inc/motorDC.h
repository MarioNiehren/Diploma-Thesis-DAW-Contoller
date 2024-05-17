/*
 * motorDC.h
 *
 *  Created on: Sep 4, 2023
 *      Author: Mario
 */

#ifndef INC_FADER_MOTORDC_H_
#define INC_FADER_MOTORDC_H_

#include "tim.h"

#define NUMBER_OF_MOTORS 8

typedef enum
{
  moveDown = 0x01,
  moveUp = 0x00
}motorDirection_TypeDef;

typedef struct
{
  TIM_HandleTypeDef* handle;
  uint16_t channel;
  uint16_t CCR;
  uint16_t CCR_old;
}pwmMotor_TypeDef;

typedef struct
{
  uint8_t ch74hc4051Address_Down;
  uint8_t ch74hc4051Address_Up;
  motorDirection_TypeDef direction;
  motorDirection_TypeDef direction_old;
  pwmMotor_TypeDef pwm;
}motorDC_TypeDef;


void motorDC_init_PWM(motorDC_TypeDef* motor, TIM_HandleTypeDef* handle, uint16_t pwmChannel);

void motorDC_startPwmTimer(motorDC_TypeDef* motor);

void motorDC_moveUp(motorDC_TypeDef* motor, uint16_t pwmCCR);
void motorDC_moveDown(motorDC_TypeDef* motor, uint16_t pwmCCR);
void motorDC_stopMoving(motorDC_TypeDef* motor);

#endif /* INC_FADER_MOTORDC_H_ */
