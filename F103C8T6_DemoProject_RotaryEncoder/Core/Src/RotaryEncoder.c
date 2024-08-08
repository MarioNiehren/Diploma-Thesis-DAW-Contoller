/***************************************************************************//**
 * @defgroup        RotaryEncoder_Source  Source
 * @brief           Study this part for a details about this module.
 *
 * @addtogroup      RotaryEncoder
 * @{
 *
 * @addtogroup      RotaryEncoder_Source
 * @{
 *
 * @file            RotaryEncoder.c
 *
 * @date            Nov 21, 2023
 * @author          Mario Niehren
 ******************************************************************************/
/*
 * RotaryEncoder.c
 *
 *  Created on: Nov 21, 2023
 *      Author: Mario
 */

#include "RotaryEncoder.h"

/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize the Rotary Encoder.
 * @{
 ******************************************************************************/

/* Description in .h */
void Encoder_init_PinA(ENC_TypeDef* ENC, GPIO_TypeDef* Port, uint16_t Pin)
{
  ENC->PortA = Port;
  ENC->PinA = Pin;
  ENC->Interrupt = ENCODER_NO_INTERRUPT;
}

/* Description in .h */
void Encoder_init_PinB(ENC_TypeDef* ENC, GPIO_TypeDef* Port, uint16_t Pin)
{
  ENC->PortB = Port;
  ENC->PinB = Pin;
  ENC->Interrupt = ENCODER_NO_INTERRUPT;
}

/* Description in .h */
void Encoder_init_DebounceTimeInMs(ENC_TypeDef* ENC, uint16_t Time)
{
  Timer_structTd* Timer = &ENC->Timer;
  Timer_set_ThresholdInMs(Timer, Time);
}


/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Process
 * @brief     Use these functions to process the Rotary Encoder.
 * @{
 ******************************************************************************/

/** @cond *//* Function Prototypes */
void process_PinAInterrupted(ENC_TypeDef* ENC);
void process_PinBInterrupted(ENC_TypeDef* ENC);
/** @endcond *//* Function Prototypes */

/* Description in .h */
void Encoder_update(ENC_TypeDef* ENC)
{
  ENC_enum_Td InterruptState = ENC->Interrupt;
  Timer_structTd* Timer = &ENC->Timer;

  if(InterruptState == ENCODER_INTERRUPT_A)
  {
    /** @internal     - If Pin A is interrupted:
     *                  - Set Interrupt state to scheduled for next cycle
     *                  - Start debounce timer */
    ENC->Interrupt = ENCODER_INTERRUPT_A_SCHEDULED;
    Timer_restart_Timer(Timer);
  }
  else if(InterruptState == ENCODER_INTERRUPT_A_SCHEDULED)
  {
    /** @internal     - If Pin A is scheduled for processing:
     *                  - If timer is elapsed:
     *                    - Reset Interrupt
     *                    - Process Pins to get tick direction */
    if(Timer_check_TimerElapsed(Timer) == true)
    {
      ENC->Interrupt = ENCODER_NO_INTERRUPT;
      process_PinAInterrupted(ENC);
    }
    else
    {
      ;
    }
  }
  else if(InterruptState == ENCODER_INTERRUPT_B)
  {
    /** @internal     - If Pin B is interrupted:
     *                  - Set Interrupt state to scheduled for next cycle
     *                  - Start debounce timer */
    ENC->Interrupt = ENCODER_INTERRUPT_B_SCHEDULED;
    Timer_restart_Timer(Timer);
  }
  else if(InterruptState == ENCODER_INTERRUPT_B_SCHEDULED)
  {
    /** @internal     - If Pin B is scheduled for processing:
     *                  - If timer is elapsed:
     *                    - Reset Interrupt
     *                    - Process Pins to get tick direction */
    if(Timer_check_TimerElapsed(Timer) == true)
    {
      ENC->Interrupt = ENCODER_NO_INTERRUPT;
      process_PinBInterrupted(ENC);
    }
    else
    {
      ;
    }
  }
  else
  {
    ENC->Tick = ENC_NO_TICK;
  }
}

/** @cond *//* Function Prototypes */
ENC_enum_Td get_TickDirectionInterruptA(GPIO_PinState PinStateA, GPIO_PinState PinStateB);
ENC_enum_Td get_TickDirectionInterruptB(GPIO_PinState PinStateA, GPIO_PinState PinStateB);
/** @endcond *//* Function Prototypes */
/**
 * @brief     This function is used to read the pins and get the rotation
 *            direction if Pin A was interrupted.
 * @param     ENC     pointer to the users rotary encoder data structure
 * @return    none
 */
void process_PinAInterrupted(ENC_TypeDef* ENC)
{
  /** @internal     - Get Pin A and Pin B states */
  GPIO_PinState StatePinA = HAL_GPIO_ReadPin(ENC->PortA, ENC->PinA);
  GPIO_PinState StatePinB = HAL_GPIO_ReadPin(ENC->PortB, ENC->PinB);
  /** @internal     - check and save the valid rotation tick direction */
  ENC->Tick = get_TickDirectionInterruptA(StatePinA, StatePinB);
}

/**
 * @brief     This function is used to read the pins and get the rotation
 *            direction if Pin B was interrupted.
 * @param     ENC     pointer to the users rotary encoder data structure
 * @return    none
 */
void process_PinBInterrupted(ENC_TypeDef* ENC)
{
  /** @internal     - Get Pin A and Pin B states */
  GPIO_PinState StatePinA = HAL_GPIO_ReadPin(ENC->PortA, ENC->PinA);
  GPIO_PinState StatePinB = HAL_GPIO_ReadPin(ENC->PortB, ENC->PinB);
  /** @internal     - check and save the valid rotation tick direction */
  ENC->Tick = get_TickDirectionInterruptB(StatePinA, StatePinB);
}

/**
 * @brief     Get the tick direction of the encoder rotation for Pin A.
 *            This function will be called if Pin A gets interrupted.
 * @param     PinStateA State of Pin A after current interrupt on Pin A
 * @param     PinStateB State of Pin B stored from last interrupt on Pin B
 * @return    none
 */
ENC_enum_Td get_TickDirectionInterruptA(GPIO_PinState PinStateA, GPIO_PinState PinStateB)
{
  ENC_enum_Td ReturnTick = ENC_NO_TICK;

  /** @internal     - If Pin A is High & Pin B is High -> return Tick Left
   *                - If Pin A is High & Pin B is Low -> return Tick Right */
  if (PinStateA == GPIO_PIN_SET)
  {
    if(PinStateB == GPIO_PIN_SET)
    {
      ReturnTick = ENC_TICK_LEFT;
    }
    else if(PinStateB == GPIO_PIN_RESET)
    {
      ReturnTick = ENC_TICK_RIGHT;
    }
  }
  /** @internal     - If Pin A is Low & Pin B is High -> return Tick Right
   *                - If Pin A is Low & Pin B is Low -> return Tick Left */
  else if(PinStateA == GPIO_PIN_RESET)
  {
    if(PinStateB == GPIO_PIN_SET)
    {
      ReturnTick = ENC_TICK_RIGHT;
    }
    else if(PinStateB == GPIO_PIN_RESET)
    {
      ReturnTick = ENC_TICK_LEFT;
    }
  }
  else
  {
    /**@internal      - return ENC_NO_TICK if if none of the previous conditions
     *                  is true */
    ReturnTick = ENC_NO_TICK;
  }

  return ReturnTick;
}

/**
 * @brief     Get the tick direction of the encoder rotation for Pin B.
 *            This function will be called if Pin B gets interrupted.
 * @param     PinStateA State of Pin A stored from last interrupt on Pin A
 * @param     PinStateB State of Pin B after current interrupt on Pin B
 * @return    none
 */
ENC_enum_Td get_TickDirectionInterruptB(GPIO_PinState PinStateA, GPIO_PinState PinStateB)
{
  ENC_enum_Td ReturnTick = ENC_NO_TICK;

  /** @internal     - If Pin B is High & Pin A is High -> return Tick Right
   *                - If Pin B is High & Pin A is Low -> return Tick Left */
  if (PinStateB == GPIO_PIN_SET)
  {
    if(PinStateA == GPIO_PIN_SET)
    {
      ReturnTick = ENC_TICK_RIGHT;
    }
    else if(PinStateA == GPIO_PIN_RESET)
    {
      ReturnTick = ENC_TICK_LEFT;
    }
  }
  /** @internal     - If Pin B is Low & Pin A is High -> return Tick Left
   *                - If Pin B is Low & Pin A is Low -> return Tick Right */
  else if(PinStateB == GPIO_PIN_RESET)
  {
    if(PinStateA == GPIO_PIN_SET)
    {
      ReturnTick = ENC_TICK_LEFT;
    }
    else if(PinStateA == GPIO_PIN_RESET)
    {
      ReturnTick = ENC_TICK_RIGHT;
    }
  }
  else
  {
    /**@internal      - return ENC_NO_TICK if if none of the previous conditions
     *                  is true */
    ReturnTick = ENC_NO_TICK;
  }

  return ReturnTick;
}

/* Description in .h */
void Encoder_manage_Interrupt(ENC_TypeDef* ENC, uint16_t GPIO_Pin)
{
  if(ENC->Interrupt == ENCODER_NO_INTERRUPT)
  {
    if(GPIO_Pin == ENC->PinA)
    {
      ENC->Interrupt = ENCODER_INTERRUPT_A;
    }
    else if(GPIO_Pin == ENC->PinB)
    {
      ENC->Interrupt = ENCODER_INTERRUPT_B;
    }
    else
    {
      ;
    }
  }
  else
  {
    ;
  }
}
/** @} ************************************************************************/
/* end of name "Process"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Interact
 * @brief     Use these functions to interact with the module
 * @{
 ******************************************************************************/

/* Description in .h */
ENC_enum_Td Encoder_get_Tick(ENC_TypeDef* ENC)
{
  return ENC->Tick;
}

/** @} ************************************************************************/
/* end of name "Interact"
 ******************************************************************************/
/**@}*//* end of defgroup "RotaryEncoder_Source" */
/**@}*//* end of defgroup "RotaryEncoder" */
