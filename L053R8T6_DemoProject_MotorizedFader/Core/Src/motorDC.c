/*
 * motorDC.c
 *
 *  Created on: Sep 4, 2023
 *      Author: Mario
 */

#include <motorDC.h>



void motorDC_init_PWM(motorDC_TypeDef* motor, TIM_HandleTypeDef* handle, uint16_t pwmChannel)
{

  motor->pwm.handle = handle;
  motor->pwm.channel = pwmChannel;

}

void motorDC_startPwmTimer(motorDC_TypeDef* motor)
{
  HAL_TIM_PWM_Start(motor->pwm.handle, motor->pwm.channel);
}

void motorDC_moveUp(motorDC_TypeDef* motor, uint16_t pwmCCR)
{
  motor->pwm.CCR = pwmCCR;
  motor->direction = moveUp;
  if(motor->direction_old != motor->direction)
  {
    motor->direction_old = moveUp;
  }

  if(motor->pwm.CCR_old != motor->pwm.CCR)
  {
    __HAL_TIM_SET_COMPARE(motor->pwm.handle, motor->pwm.channel, motor->pwm.CCR);
    motor->pwm.CCR_old = motor->pwm.CCR;
  }

}

void motorDC_moveDown(motorDC_TypeDef* motor, uint16_t pwmCCR)
{
  motor->pwm.CCR = pwmCCR;
  motor->direction = moveDown;
  if(motor->direction_old != motor->direction)
  {
    motor->direction_old = moveDown;
  }
  if(motor->pwm.CCR_old != motor->pwm.CCR)
  {
    __HAL_TIM_SET_COMPARE(motor->pwm.handle, motor->pwm.channel, motor->pwm.CCR);
    motor->pwm.CCR_old = motor->pwm.CCR;
  }
}

void motorDC_stopMoving(motorDC_TypeDef* motor)
{
  if(motor->pwm.CCR_old != 0x00)
  {
    motor->pwm.CCR = 0x00;
    __HAL_TIM_SET_COMPARE(motor->pwm.handle, motor->pwm.channel, motor->pwm.CCR);
    motor->pwm.CCR_old = 0x00;
  }
}
