/***************************************************************************//**
 * @defgroup        TB6612FNG_Source    Source Code
 * @brief           Study this part for details about this module.
 *
 * @addtogroup      TB6612FNG
 * @{
 *
 * @addtogroup      TB6612FNG_Source
 * @{
 *
 * @file            TB6612FNG_MotorDriver.c
 *
 * @date            Sep 4, 2023
 * @author          Mario Niehren
 ******************************************************************************/

#include <TB6612FNG_MotorDriver.h>

/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize the Motor Driver.
 * @{
 ******************************************************************************/

/* Description in .h */
void MotorDriver_init_PinIn1(TB6612FNGMotorDriver_structTd* Motor, GPIO_TypeDef* GPIO, uint16_t Pin)
{
  /** @internal     1.  link GPIO and save Pin number to the Motor structure */
  Motor->GPIOIn1 = GPIO;
  Motor->PinIn1 = Pin;
}

/* Description in .h */
void MotorDriver_init_PinIn2(TB6612FNGMotorDriver_structTd* Motor, GPIO_TypeDef* GPIO, uint16_t Pin)
{
  /** @internal     1.  link GPIO and save Pin number to the Motor structure */
  Motor->GPIOIn2 = GPIO;
  Motor->PinIn2 = Pin;
}

/* Description in .h */
void MotorDriver_init_PinSTBY(TB6612FNGMotorDriver_structTd* Motor, GPIO_TypeDef* GPIO, uint16_t Pin)
{
  /** @internal     1.  link GPIO and save Pin number to the Motor structure */
  Motor->GPIOSTBY = GPIO;
  Motor->PinSTBY = Pin;
}

/* Description in .h */
void MotorDriver_init_PWM(TB6612FNGMotorDriver_structTd* Motor, TIM_HandleTypeDef* htim, uint16_t Channel)
{
  /** @internal     1.  link htim and save channel to the Motor structure */
  Motor->htim = htim;
  Motor->channel = Channel;
}

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Start
 * @brief     Use these functions to process the motor driver
 * @{
 ******************************************************************************/

/* Description in .h */
void MotorDriver_start_PWM(TB6612FNGMotorDriver_structTd* Motor)
{
  TIM_HandleTypeDef* htim = Motor->htim;
  uint16_t channel = Motor->channel;
  uint16_t* Buffer = &Motor->DMABufferCCR;
  uint16_t Length = 1;

  HAL_TIM_PWM_Start_IT(htim, channel);
}

/** @} ************************************************************************/
/* end of name "Start"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Control
 * @brief     Use these functions to control a motor on the driver.
 * @{
 ******************************************************************************/

/** @cond *//* Function Prototypes */
void writePin_In1In2STBY(TB6612FNGMotorDriver_structTd* Motor, GPIO_PinState StateIn1, GPIO_PinState StateIn2, GPIO_PinState StateSTBY);
/** @endcond *//* Function Prototypes */

/* Description in .h */
void MotorDriver_move_ClockWise(TB6612FNGMotorDriver_structTd* Motor, uint16_t CCR)
{
  TB6612FNGMotorDriver_enums CurrentMode = Motor->Mode;
  uint16_t OldCCR = Motor->DMABufferCCR;

  /** @internal     1.  If motor is not in CW mode, set the pins as required by
   *                    @ref TB6612FNG_Datasheet "Datasheet" for CW mode. */
  if(CurrentMode != MOTORDRIVER_DIRECTION_CW)
  {
    writePin_In1In2STBY(Motor, GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_SET);
    Motor->Mode = MOTORDRIVER_DIRECTION_CW;
  }

  /** @internal     2.  If CCR changed, set the new CCR value to the DMA
   *                    buffer. */
  if(CCR != OldCCR)
  {
    __HAL_TIM_SetCompare(Motor->htim, Motor->channel, CCR);
    Motor->DMABufferCCR = CCR;
  }
}

/* Description in .h */
void MotorDriver_move_CounterClockWise(TB6612FNGMotorDriver_structTd* Motor, uint16_t CCR)
{
  TB6612FNGMotorDriver_enums CurrentMode = Motor->Mode;
  uint16_t OldCCR = Motor->DMABufferCCR;

  /** @internal     1.  If motor is not in CCW mode, set the pins as required by
   *                    @ref TB6612FNG_Datasheet "Datasheet" for CCW mode. */
  if(CurrentMode != MOTORDRIVER_DIRECTION_CCW)
  {
    writePin_In1In2STBY(Motor, GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET);
    Motor->Mode = MOTORDRIVER_DIRECTION_CCW;
  }

  /** @internal     2.  If CCR changed, set the new CCR value to the DMA
   *                    buffer. */
  if(CCR != OldCCR)
  {
    __HAL_TIM_SetCompare(Motor->htim, Motor->channel, CCR);
    Motor->DMABufferCCR = CCR;
  }
}

/* Description in .h */
void MotorDriver_stop(TB6612FNGMotorDriver_structTd* Motor)
{
  TB6612FNGMotorDriver_enums CurrentMode = Motor->Mode;

  /** @internal     1.  If motor is not in stop mode, set the pins as required
   *                    by @ref TB6612FNG_Datasheet "Datasheet" for stop
   *                    mode. */
  if(CurrentMode != MOTORDRIVER_STOPED)
  {
    writePin_In1In2STBY(Motor, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET);
    Motor->Mode = MOTORDRIVER_STOPED;
  }
}

/* Description in .h */
void MotorDriver_stop_ShortBreak(TB6612FNGMotorDriver_structTd* Motor)
{
  TB6612FNGMotorDriver_enums CurrentMode = Motor->Mode;

  /** @internal     1.  If motor is not in short brake mode, set the pins as
   *                    required by @ref TB6612FNG_Datasheet "Datasheet" for
   *                    short brake mode. */
  if(CurrentMode != MOTORDRIVER_SHORTBRAKED)
  {
    writePin_In1In2STBY(Motor, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET);
    Motor->Mode = MOTORDRIVER_SHORTBRAKED;
  }
}

/* Description in .h */
void MotorDriver_standby(TB6612FNGMotorDriver_structTd* Motor)
{
  TB6612FNGMotorDriver_enums CurrentMode = Motor->Mode;

  /** @internal     1.  If motor is not in standby mode, set the pins as
   *                    required by @ref TB6612FNG_Datasheet "Datasheet" for
   *                    standby mode. */
  if(CurrentMode != MOTORDRIVER_STANDBY)
  {
    writePin_In1In2STBY(Motor, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET);
    Motor->Mode = MOTORDRIVER_STANDBY;
  }
}

/**
 * @brief     Helper function to write Pin In1, In2 and STBY to control the
 *            driver.
 * @param     Motor     pointer to the users motor structure of the motor used
 *                      with this IC.
 * @param     StateIn1  GPIO_PIN_SET or GPIO_PIN_RESET
 * @param     StateIn2  GPIO_PIN_SET or GPIO_PIN_RESET
 * @param     StateSTBY GPIO_PIN_SET or GPIO_PIN_RESET
 * @return    none
 */
void writePin_In1In2STBY(TB6612FNGMotorDriver_structTd* Motor, GPIO_PinState StateIn1, GPIO_PinState StateIn2, GPIO_PinState StateSTBY)
{
  HAL_GPIO_WritePin(Motor->GPIOIn1, Motor->PinIn1, StateIn1);
  HAL_GPIO_WritePin(Motor->GPIOIn2, Motor->PinIn2, StateIn2);
  HAL_GPIO_WritePin(Motor->GPIOSTBY, Motor->PinSTBY, StateSTBY);
}
/** @} ************************************************************************/
/* end of name "Control"
 ******************************************************************************/


/**@}*//* end of defgroup "TB6612FNG_Source" */
/**@}*//* end of defgroup "TB6612FNG" */
