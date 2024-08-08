/***************************************************************************//**
 * @defgroup        RotaryEncoder   Rotary Encoder
 * @brief           This module can be used to read rotary encoders..
 *
 * # How to use:
 * @todo
 *
 * # Links:
 * @anchor          DataSheetBourns [Data Sheet Bourns Endocer](https://www.bourns.com/docs/product-datasheets/pec11r.pdf)
 *
 * @defgroup        RotaryEncoder_Header  Header
 * @brief           Study this part for a quick overview of this module.
 *
 * @addtogroup      RotaryEncoder
 * @{
 *
 * @addtogroup      RotaryEncoder_Header
 * @{
 *
 * @file            RotaryEncoder.h
 *
 * @date            Nov 21, 2023
 * @author          Mario Niehren
 ******************************************************************************/

#ifndef MN_INTERFACE_RotaryEncoder_H
#define MN_INTERFACE_RotaryEncoder_H

#include "stm32f1xx_hal.h"
#include "timer.h"

typedef enum
{
  ENC_TICK_LEFT = 0x10,
  ENC_TICK_RIGHT = 0x11,
  ENC_NO_TICK = 0x1F,

  ENCODER_INTERRUPT_A = 0x20,
  ENCODER_INTERRUPT_B = 0x21,
  ENCODER_INTERRUPT_A_SCHEDULED = 0x22,
  ENCODER_INTERRUPT_B_SCHEDULED = 0x23,
  ENCODER_NO_INTERRUPT = 0x2F,
}ENC_enum_Td;

typedef struct
{
  GPIO_TypeDef* PortA;
  uint16_t PinA;
  GPIO_TypeDef* PortB;
  uint16_t PinB;

  ENC_enum_Td Interrupt;

  ENC_enum_Td Tick;

  Timer_structTd Timer;
}ENC_TypeDef;

/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize the Rotary Encoder.
 * @{
 ******************************************************************************/

/**
 * @brief     Link Pin A of the rotary encoder to the internal module
 * @param     ENC     pointer to the users rotary encoder data structure
 * @param     Port    used for rotary encoder Pin A
 * @param     Pin     used for rotary encoder Pin A
 * @return    none
 */
void Encoder_init_PinA(ENC_TypeDef* ENC, GPIO_TypeDef* Port, uint16_t Pin);

/**
 * @brief     Link Pin B of the rotary encoder to the internal module
 * @param     ENC     pointer to the users rotary encoder data structure
 * @param     Port    used for rotary encoder Pin B
 * @param     Pin     used for rotary encoder Pin B
 * @return    none
 */
void Encoder_init_PinB(ENC_TypeDef* ENC, GPIO_TypeDef* Port, uint16_t Pin);

/**
 * @brief     Set the debounce time for Pins A and B. This time depends on the
 *            used encoder (see datasheet) and the debounce cirquit.
 * @param     ENC     pointer to the users rotary encoder data structure
 * @param     Time    debounce time in milliseconds
 * @return    none
 */
void Encoder_init_DebounceTimeInMs(ENC_TypeDef* ENC, uint16_t Time);
/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Process
 * @brief     Use these functions to process the Rotary Encoder.
 * @{
 ******************************************************************************/

/**
 * @brief     Call this function periodically in while loop to update the
 *            state of the rotary encoder.
 * @param     ENC     pointer to the users rotary encoder data structure
 * @return    none
 */
void Encoder_update(ENC_TypeDef* ENC);

/**
 * @brief     Call this function in the HAL interrupt handler. Here an example:
 *            @code
 *            void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 *            {
 *              Encoder_manage_Interrupt(&Encoder, GPIO_Pin);
 *            }
 *            @endcode
 * @brief     ENC     pointer to the users rotary encoder data structure
 * @brief     GPIO_Pin  interrupted Pin
 * @return    none
 */
void Encoder_manage_Interrupt(ENC_TypeDef* ENC, uint16_t GPIO_Pin);
/** @} ************************************************************************/
/* end of name "Process"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Interact
 * @brief     Use these functions to interact with the module
 * @{
 ******************************************************************************/
/**
 * @brief     Call this function to get the direction of the rotary encoder.
 *            To avoid missing ticks, it is recommended to call this function
 *            after each Encoder update.
 * @note      A Tick in a specific direction means, that the Encoder was
 *            rotated in one direction. It is up to the user, how to interpret
 *            these ticks in terms of speed or error correction if the encoder
 *            gets turned very quick.
 * @brief     ENC     pointer to the users rotary encoder data structure
 * @return    ENC_TICK_LEFT, ENC_TICK_RIGHT or ENC_NO_TICK
 */
ENC_enum_Td Encoder_get_Tick(ENC_TypeDef* ENC);
/** @} ************************************************************************/
/* end of name "Interact"
 ******************************************************************************/

/**@}*//* end of defgroup "RotaryEncoder_Header" */
/**@}*//* end of defgroup "RotaryEncoder" */

#endif /* MN_INTERFACE_RotaryEncoder_H */
