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
#include <string.h>

typedef enum
{
  ENC_RisingEdge,
  ENC_FallingEdge
}ENC_Flag_TypeDef;

typedef enum
{
  ENCODER_Interrupt_A,
  ENCODER_Interrupt_B,
  ENCODER_NoInterrupt
}ENC_InterruptDetect_TypeDef;

typedef struct
{
  GPIO_TypeDef* PortA;
  uint16_t PinA;
  GPIO_TypeDef* PortB;
  uint16_t PinB;

  ENC_InterruptDetect_TypeDef Interrupt;

  ENC_Flag_TypeDef FlagA;
  ENC_Flag_TypeDef FlagB;
  GPIO_PinState StateA;
  GPIO_PinState StateB;

  uint8_t TicksLeft;
  uint8_t TicksRight;
}ENC_TypeDef;

/***************************************************************************************************
 * Functions to initialize.
 ***************************************************************************************************/
void Encoder_init_PinA(ENC_TypeDef* ENC, GPIO_TypeDef* Port, uint16_t Pin);
void Encoder_init_PinB(ENC_TypeDef* ENC, GPIO_TypeDef* Port, uint16_t Pin);

/***************************************************************************************************
 * Functions to compute encoders.
 ***************************************************************************************************/
void Encoders_compute(ENC_TypeDef* ENC);

/***************************************************************************************************
 * Interrupt management. Call this function in GPIO-Interrupt handler.
 ***************************************************************************************************/
void Encoder_manage_Interrupt(ENC_TypeDef* ENC, uint16_t GPIO_Pin);

/**@}*//* end of defgroup "RotaryEncoder_Header" */
/**@}*//* end of defgroup "RotaryEncoder" */

#endif /* MN_INTERFACE_RotaryEncoder_H */
