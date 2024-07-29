/*
 * RotaryEncoder.c
 *
 *  Created on: Nov 21, 2023
 *      Author: Mario
 */

#include "RotaryEncoder.h"

/***************************************************************************************************
 * BEGIN: Functions to initialize.
 ***************************************************************************************************/
void Encoder_init_PinA(ENC_TypeDef* ENC, GPIO_TypeDef* Port, uint16_t Pin)
{
  ENC->PortA = Port;
  ENC->PinA = Pin;
}

void Encoder_init_PinB(ENC_TypeDef* ENC, GPIO_TypeDef* Port, uint16_t Pin)
{
  ENC->PortB = Port;
  ENC->PinB = Pin;
}
/***************************************************************************************************
 * END: Functions to initialize.
 ***************************************************************************************************/






/***************************************************************************************************
 * BEGIN: Functions to compute encoders.
 ***************************************************************************************************/
void Encoders_compute(ENC_TypeDef* ENC)
{
  if(ENC->Interrupt == ENCODER_Interrupt_A)
    {
      HAL_Delay(2);
      ENC->Interrupt = ENCODER_NoInterrupt;
      ENC->StateA = HAL_GPIO_ReadPin(ENC->PortA, ENC->PinA);

      if (ENC->StateA == GPIO_PIN_SET)
      {
        ENC->FlagA = ENC_RisingEdge;

        if(ENC->StateB == GPIO_PIN_SET)
        {
          ENC->TicksLeft++;
        }
        else if(ENC->StateB == GPIO_PIN_RESET)
        {
          ENC->TicksRight++;
        }
      }
      else
      {
        ENC->FlagA = ENC_FallingEdge;

        if(ENC->StateB == GPIO_PIN_SET)
        {
          ENC->TicksRight++;
        }
        else if(ENC->StateB == GPIO_PIN_RESET)
        {
          ENC->TicksLeft++;
        }
      }
    }
    if(ENC->Interrupt == ENCODER_Interrupt_B)
    {
      HAL_Delay(2);
      ENC->Interrupt = ENCODER_NoInterrupt;
      ENC->StateB = HAL_GPIO_ReadPin(ENC->PortB, ENC->PinB);

      if (ENC->StateB == GPIO_PIN_SET)
      {
        ENC->FlagB = ENC_RisingEdge;

        if(ENC->StateA == GPIO_PIN_SET)
        {
          ENC->TicksRight++;
        }
        else if(ENC->StateA == GPIO_PIN_RESET)
        {
          ENC->TicksLeft++;
        }
      }
      else
      {
        ENC->FlagB = ENC_FallingEdge;

        if(ENC->StateA == GPIO_PIN_SET)
        {
          ENC->TicksLeft++;
        }
        else if(ENC->StateA == GPIO_PIN_RESET)
        {
          ENC->TicksRight++;
        }
      }
    }
}
/***************************************************************************************************
 * END: Functions to compute encoders.
 ***************************************************************************************************/





/***************************************************************************************************
 * BEGIN: Interrupt management. Call this function in GPIO-Interrupt handler.
 ***************************************************************************************************/
void Encoder_manage_Interrupt(ENC_TypeDef* ENC, uint16_t GPIO_Pin)
{
  if(GPIO_Pin == ENC->PinA)
  {
    ENC->Interrupt = ENCODER_Interrupt_A;
  }
  if(GPIO_Pin == ENC->PinB)
  {
    ENC->Interrupt = ENCODER_Interrupt_B;
  }
}
/***************************************************************************************************
 * END: Interrupt management. Call this function in GPIO-Interrupt handler.
 ***************************************************************************************************/
