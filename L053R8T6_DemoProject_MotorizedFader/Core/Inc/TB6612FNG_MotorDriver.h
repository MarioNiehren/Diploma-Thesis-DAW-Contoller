/***************************************************************************//**
 * @defgroup        TB6612FNG   TB6612FNG Dual DC motor driver
 * @brief           This module can be used to control the TB6612 driver for
 *                  DC motors.
 *
 * # How to use:
 * @todo: Add how to use
 *
 * # Links:
 * @anchor          TB6612FNG_Datasheet [Datasheet](https://toshiba.semicon-storage.com/info/TB6612FNG_datasheet_en_20141001.pdf?did=10660&prodName=TB6612FNG)
 *
 * @defgroup        TB6612FNG_Header      Header
 * @brief           Study this part for a quick overview of this module.
 *
 * @addtogroup      MotorFader
 * @{
 *
 * @addtogroup      TB6612FNG
 * @{
 *
 * @addtogroup      TB6612FNG_Header
 * @{
 *
 * @file            TB6612FNG_MotorDriver.h
 *
 * @date            Sep 4, 2023
 * @author          Mario Niehren
 ******************************************************************************/

#ifndef INC_TB6612FNG_MOTORDC_H_
#define INC_TB6612FNG_MOTORDC_H_

#include "stm32l0xx_hal.h"


/***************************************************************************//**
 * @name      Structures and Enumerations
 * @brief     This structure is used to store all data for the motor driver.
 * @{
 ******************************************************************************/
typedef enum
{
  MOTORDRIVER_DIRECTION_CW  = 1,
  MOTORDRIVER_DIRECTION_CCW = 2,
  MOTORDRIVER_STOPED        = 3,
  MOTORDRIVER_SHORTBRAKED   = 4,
  MOTORDRIVER_STANDBY       = 5,
}TB6612FNGMotorDriver_enums;

typedef struct
{
  TB6612FNGMotorDriver_enums Mode;  /**<  Current mode of the motor on the
                                    driver */

  TIM_HandleTypeDef*  htim;   /**<  pointer to the HAL-timer handle */
  uint16_t  channel;          /**<  Used channel of the timer */

  GPIO_TypeDef* GPIOIn1;
  uint16_t      PinIn1;
  GPIO_TypeDef* GPIOIn2;
  uint16_t      PinIn2;
  GPIO_TypeDef* GPIOSTBY;     /**<  GPIO Port for the STBY Pin. This is shared
                                    with the other motor on the IC! */
  uint16_t      PinSTBY;      /**<  GPIO Pin for the STBY Pin. This is shared
                                    with the other motor on the IC! */

  uint16_t      DMABufferCCR;    /**<  CCR Buffer to prepare PWM values for the
                                    DMA transfer*/

}TB6612FNGMotorDriver_structTd;
/** @} ************************************************************************/
/* end of name "Structures and Enumerations"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize the Motor Driver.
 * @{
 ******************************************************************************/

/**
 * @brief     Link the GPIO Port and Pin used for IN1 of the device.
 *            This pin has to be an output, the name of the function refers to
 *            the name scheme of the driver IC.
 * @param     Motor     pointer to the users motor structure of the motor used
 *                      with this IC.
 * @param     GPIO      port of the In 1 Pin
 * @param     Pin       number of In 1
 * @return    none
 */
void MotorDriver_init_PinIn1(TB6612FNGMotorDriver_structTd* Motor, GPIO_TypeDef* GPIO, uint16_t Pin);

/**
 * @brief     Link the GPIO Port and Pin used for IN2 of the device.
 *            This pin has to be an output, the name of the function refers to
 *            the name scheme of the driver IC.
 * @param     Motor     pointer to the users motor structure of the motor used
 *                      with this IC.
 * @param     GPIO      port of the In 2 Pin
 * @param     Pin       number of In 2
 * @return    none
 */
void MotorDriver_init_PinIn2(TB6612FNGMotorDriver_structTd* Motor, GPIO_TypeDef* GPIO, uint16_t Pin);

/**
 * @brief     Link the GPIO Port and Pin used for STBY of the device.
 *            This pin has to be an output, the name of the function refers to
 *            the name scheme of the driver IC.
 * @warning   This Pin is shared for both motors initialized on one IC! That
 *            means, that the other motor will also be affected if the STBY Pin
 *            is active.
 * @param     Motor     pointer to the users motor structure of the motor used
 *                      with this IC.
 * @param     GPIO      port of the STBY Pin
 * @param     Pin       number of STBY
 * @return    none
 */
void MotorDriver_init_PinSTBY(TB6612FNGMotorDriver_structTd* Motor, GPIO_TypeDef* GPIO, uint16_t Pin);

/**
 * @brief     Link the TIM handle and channel used for PWM.
 *
 * #How to setup PWM in CubeMX:
 * @todo      Setup
 * PWM-Freuqency = (TIM-CLK / Counter)
 * @param     Motor     pointer to the users motor structure of the motor used
 *                      with this IC.
 * @param     htim      pointer to the HAL-handle for the timer used for PWM
 * @param     channel   of the timer used for PWM. Use the hal  HAL-defines.
 *                      They have the following format: TIM_CHANNEL_x, where
 *                      x is the channel number (1 - 4).
 * @return    none
 */
void MotorDriver_init_PWM(TB6612FNGMotorDriver_structTd* Motor, TIM_HandleTypeDef* htim, uint16_t Channel);

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Start
 * @brief     Use these functions to start the motor driver
 * @{
 ******************************************************************************/

/**
 * @brief     Call this function once for each initialized Motor. This will
 *            start the PWM timer with DMA.
 * @param     Motor     pointer to the users motor structure of the motor used
 *                      with this IC.
 * @return    none
 */
void MotorDriver_start_PWM(TB6612FNGMotorDriver_structTd* Motor);

/** @} ************************************************************************/
/* end of name "Start"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Control
 * @brief     Use these functions to control a motor on the driver.
 * @{
 ******************************************************************************/

/**
 * @brief     Call this function to move the motor clock wise. The speed is
 *            controlled by the CCR (Capture Compare Register), that tells the
 *            PWM timer, how many clock cycles should be high during one period.
 * @param     Motor     pointer to the users motor structure of the motor used
 *                      with this IC.
 * @param     CCR       register value. This can be from 0 to the value of ARR
 *                      (Auto Reload Register). ARR is the value set up in the
 *                      Counter Period field in Cube MX.
 * @return    none
 */
void MotorDriver_move_ClockWise(TB6612FNGMotorDriver_structTd* Motor, uint16_t CCR);

/**
 * @brief     Call this function to move the motor counter clock wise. The speed
 *            is controlled by the CCR (Capture Compare Register), that tells
 *            the PWM timer, how many clock cycles should be high during one
 *            period.
 * @param     Motor     pointer to the users motor structure of the motor used
 *                      with this IC.
 * @param     CCR       register value. This can be from 0 to the value of ARR
 *                      (Auto Reload Register). ARR is the value set up in the
 *                      Counter Period field in Cube MX.
 * @return    none
 */
void MotorDriver_move_CounterClockWise(TB6612FNGMotorDriver_structTd* Motor, uint16_t CCR);

/**
 * @brief     Call this function to stop the motor without active brake. The
 *            output of the driver will be on high impedance (OFF). The CCR
 *            DMA buffer will not be affected.
 * @param     Motor     pointer to the users motor structure of the motor used
 *                      with this IC.
 * @return    none
 */
void MotorDriver_stop(TB6612FNGMotorDriver_structTd* Motor);

/**
 * @brief     Call this function to stop the motor with a short brake. The
 *            output of the driver will be short circuited to stop the motor
 *            immediately.
 * @param     Motor     pointer to the users motor structure of the motor used
 *                      with this IC.
 * @return    none
 */
void MotorDriver_stop_ShortBreak(TB6612FNGMotorDriver_structTd* Motor);

/**
 * @brief     Call this function to set the driver to standby. Be aware that
 *            this will also affect the other motor connected to this IC.
 * @param     Motor     pointer to the users motor structure of the motor used
 *                      with this IC.
 * @return    none
 */
void MotorDriver_standby(TB6612FNGMotorDriver_structTd* Motor);

/** @} ************************************************************************/
/* end of name "Control"
 ******************************************************************************/

/**@}*//* end of defgroup "TB6612FNG_Header" */
/**@}*//* end of defgroup "TB6612FNG" */
/**@}*//* end of defgroup "MotorFader" */

#endif /* INC_TB6612FNG_MOTORDC_H_ */
