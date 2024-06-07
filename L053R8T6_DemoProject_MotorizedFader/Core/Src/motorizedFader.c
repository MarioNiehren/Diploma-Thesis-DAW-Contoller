/*
 * motorizedFader.c
 *
 *  Created on: Sep 4, 2023
 *      Author: Mario Niehren
 *
 */

#include <motorizedFader.h>

typedef struct
{
  /** Array to store pointers to all initialized faders */
  MotorizedFader_structTd* InitializedFaders[NUMBER_OF_MOTORIZED_FADERS];
  uint16_t  NumInitializedFaders;   /**<  Number of the actual initialized
                                          faders */
}MotorizedFader_internal_structTd;

MotorizedFader_internal_structTd FadersInternal = {0};

/***************************************************************************//**
 * @name      Initialize Fader
 * @brief     Use these functions to initialize the faders internal structure.
 * @{
 ******************************************************************************/

/* Description in .h */
void MotorizedFader_init_Structure(MotorizedFader_structTd* Fader)
{
  uint16_t Index = FadersInternal.NumInitializedFaders;
  FadersInternal.InitializedFaders[Index] = Fader;
  FadersInternal.NumInitializedFaders++;
}

/* Description in .h */
void MotorizedFader_init_StartForce(MotorizedFader_structTd* Fader, int CCR)
{
  Fader->CCRStartForce = CCR;
}

/* Description in .h */
void MotorizedFader_init_StopRange(MotorizedFader_structTd* Fader, int CCR)
{
  Fader->CCRStopRange = CCR;
}

/** @} ************************************************************************/
/* end of name "Initialize Fader"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize Wiper
 * @brief     Use these functions to initialize the faders wiper.
 * @{
 ******************************************************************************/

/* Description in .h */
void MotorizedFader_init_Whiper(MotorizedFader_structTd* Fader, ADC_HandleTypeDef* Handle)
{
  /** @internal     1.  init ADC. For details look at Wiper_init_ADC() */
  Wiper_init_ADC(&Fader->Wiper, Handle);
  /** @internal     2.  init adc hysteresis. For details look at
   *                    Wiper_init_Hysteresis() */
  Wiper_init_Hysteresis(&Fader->Wiper);
}

/** @} ************************************************************************/
/* end of name "Initialize Wiper"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize Touch Sense Controller
 * @brief     Use these functions to initialize the faders TSC.
 * @{
 ******************************************************************************/

/* Description in .h */
void MotorizedFader_init_TouchTSC(MotorizedFader_structTd* fader, TSC_HandleTypeDef* htsc, uint32_t IOChannel)
{
  /** @internal     1.  init TSC. For details look at TSCButton_init_TSC() */
  TSCButton_init_TSC(&fader->TouchSense, htsc, IOChannel);
}

/* Description in .h */
void MotorizedFader_init_TouchThreshold(MotorizedFader_structTd* fader, uint16_t threshold)
{
  /** @internal     1.  init TSC threshold For details look at
   *                    TSCButton_init_Threshold() */
  TSCButton_init_Threshold(&fader->TouchSense, threshold);
}

/* Description in .h */
void MotorizedFader_init_TouchDischargeTimeMsAll(uint8_t value)
{
  TSCButton_init_DischargeTimeMsAll(value);
}

/** @} ************************************************************************/
/* end of name "Initialize Touch Sense Controller"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize Motor
 * @brief     Use these functions to initialize the faders TSC.
 * @{
 ******************************************************************************/

/* Description in .h */
void MotorizedFader_init_MotorPinIn1(MotorizedFader_structTd* Fader, GPIO_TypeDef* GPIO, uint16_t Pin)
{
  MotorDriver_init_PinIn1(&Fader->Motor, GPIO, Pin);
}

/* Description in .h */
void MotorizedFader_init_MotorPinIn2(MotorizedFader_structTd* Fader, GPIO_TypeDef* GPIO, uint16_t Pin)
{
  MotorDriver_init_PinIn2(&Fader->Motor, GPIO, Pin);
}

/* Description in .h */
void MotorizedFader_init_MotorPinSTBY(MotorizedFader_structTd* Fader, GPIO_TypeDef* GPIO, uint16_t Pin)
{
  MotorDriver_init_PinSTBY(&Fader->Motor, GPIO, Pin);
}

/* Description in .h */
void MotorizedFader_init_PWM(MotorizedFader_structTd* Fader, TIM_HandleTypeDef* htim, uint16_t Channel)
{
  MotorDriver_init_PWM(&Fader->Motor, htim, Channel);
}

/** @} ************************************************************************/
/* end of name "Initialize Motor"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize PID
 * @brief     Use these functions to initialize the PID controller that controls
 *            the motor.
 * @{
 ******************************************************************************/

/* Description in .h */
void MotorizedFader_init_PID(MotorizedFader_structTd* Fader)
{
  PID_init(&Fader->PID);
}

/* Description in .h */
void MotorizedFader_init_PIDMaxCCR(MotorizedFader_structTd* Fader, uint16_t MaxCCR)
{
  /** @internal     1.  Setup PID Output limits with -CCR to CCR. "-"
   *                    indicates the down direction. */
  PID_set_OutputMinMax(&Fader->PID, (double)-MaxCCR, (double)MaxCCR);
}

/* Description in .h */
void MotorizedFader_init_PIDKpKiKd(MotorizedFader_structTd* Fader, double Kp, double Ki, double Kd)
{
  PID_set_KpKiKd(&Fader->PID, Kp, Ki, Kd);
}

/* Description in .h */
void MotorizedFader_init_PIDLowPass(MotorizedFader_structTd* Fader, double Tau)
{
  PID_set_LowPass(&Fader->PID, Tau);
}

/* Description in .h */
void MotorizedFader_init_PIDSampleTimeInMs(MotorizedFader_structTd* Fader, uint32_t SampleTime)
{
  PID_set_SampleTimeInMs(&Fader->PID, SampleTime);
}

/** @} ************************************************************************/
/* end of name "Initialize PID"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Process
 * @brief     Use these functions to process all faders
 * @{
 ******************************************************************************/

/* Description in .h */
void MotorizedFader_start_All()
{
  TSCButton_start_All();
  Wiper_start_All();

  uint16_t NumFaders = FadersInternal.NumInitializedFaders;
  uint16_t Index = 0;
  for(Index = 0; Index < NumFaders; Index++)
  {
    MotorizedFader_structTd* Fader = FadersInternal.InitializedFaders[Index];
    MotorDriver_start_PWM(&Fader->Motor);
  }
}

/* Description in .h */
void MotorizedFader_update_All()
{
  Wiper_update_All();
  TSCButton_update_All();

  uint16_t NumFaders = FadersInternal.NumInitializedFaders;
  uint16_t Index = 0;

  for(Index = 0; Index < NumFaders; Index++)
  {
    MotorizedFader_structTd* Fader = FadersInternal.InitializedFaders[Index];

    TSCButton_State_enumTd TSCState;
    TSCState = TSCButton_get_State(&Fader->TouchSense);

    if(TSCState == TSCBUTTON_TOUCHED)
    {
      PID_reset(&Fader->PID);
      MotorDriver_stop(&Fader->Motor);
    }
    else if(TSCState == TSCBUTTON_RELEASED)
    {
      uint16_t ADCSample = Wiper_get_SmoothValue(&Fader->Wiper);
      PID_update(&Fader->PID, (double)ADCSample);
      int CCR = PID_get_OutputRound(&Fader->PID);

      int CCRStartForce = Fader->CCRStartForce;
      int CCRStopRange = Fader->CCRStopRange;

      if(CCR < -CCRStartForce)
      {
        /* Move down with PID result */
        MotorDriver_move_CounterClockWise(&Fader->Motor, -1*CCR);
      }
      else if(CCR < -CCRStopRange && CCR >= -CCRStartForce)
      {
        /* Move down with Start Force (slowest possible) */
        MotorDriver_move_CounterClockWise(&Fader->Motor, CCRStartForce);
      }
      else if(CCR > CCRStartForce)
      {
        /* Move up with PID result */
        MotorDriver_move_ClockWise(&Fader->Motor, CCR);
      }
      else if(CCR > CCRStopRange && CCR <= CCRStartForce)
      {
        /* Move up with Start Force (slowest possible) */
       MotorDriver_move_ClockWise(&Fader->Motor, CCRStartForce);
      }
      else
      {
       MotorDriver_stop(&Fader->Motor);
      }
    }
  }
}

void MotorizedFader_manage_WiperInterrupt(ADC_HandleTypeDef* hadc)
{
  Wiper_manage_Interrupt(hadc);
}

void MotorizedFader_manage_TSCInterrupt()
{
  TSCButton_manage_Interrupt();
}

/** @} ************************************************************************/
/* end of name "Process"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Set Functions
 * @brief     Use these functions to set values to the fader
 * @{
 ******************************************************************************/

/* Description in .h */
void MotorizedFader_set_Target(MotorizedFader_structTd* Fader, uint16_t Target)
{
  PID_set_Target(&Fader->PID, (double)Target);
}
/** @} ************************************************************************/
/* end of name "Set Functions"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Get Functions
 * @brief     Use these functions to get values from the fader
 * @{
 ******************************************************************************/

uint16_t MotorizedFader_get_WiperValue(MotorizedFader_structTd* Fader)
{
  uint16_t Value;
  Value = Wiper_get_SmoothValue(&Fader->Wiper);
  return Value;
}

TSCButton_State_enumTd MotorizedFader_get_TSCState(MotorizedFader_structTd* Fader)
{
  TSCButton_State_enumTd State;
  State = TSCButton_get_State(&Fader->TouchSense);
  return State;
}
/** @} ************************************************************************/
/* end of name "Get Functions"
 ******************************************************************************/
