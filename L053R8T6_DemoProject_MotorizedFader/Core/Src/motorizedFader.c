/***************************************************************************//**
 * @defgroup        MotorFader_Source      Source
 * @brief           Study this part for a details.
 *
 * @addtogroup      MotorFader
 * @{
 *
 * @addtogroup      MotorFader_Source
 * @{
 *
 * @file            motorizedFader.c
 *
 * @date            Sep 4, 2023
 * @author          Mario Niehren
 ******************************************************************************/

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
void MotorizedFader_init_Wiper(MotorizedFader_structTd* Fader, ADC_HandleTypeDef* Handle)
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
void MotorizedFader_init_MotorPWM(MotorizedFader_structTd* Fader, TIM_HandleTypeDef* htim, uint16_t Channel)
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
  /** @internal     1.  Start TSC for all faders */
  TSCButton_start_All();

  /** @internal     2.  Start ADC for all faders */
  Wiper_start_All();

  /** @internal     3.  Start PWM for all faders */
  uint16_t NumFaders = FadersInternal.NumInitializedFaders;
  uint16_t Index = 0;
  for(Index = 0; Index < NumFaders; Index++)
  {
    MotorizedFader_structTd* Fader = FadersInternal.InitializedFaders[Index];
    MotorDriver_start_PWM(&Fader->Motor);
  }
}

/** @cond *//* Function Prototypes */
int get_UpdatedPIDOutput(MotorizedFader_structTd* Fader);
void move_Fader(MotorizedFader_structTd* Fader, int CCR);
/** @endcond *//* Function Prototypes */

/* Description in .h */
void MotorizedFader_update_All()
{
  /** @internal     1.  Update all wipers */
  Wiper_update_All();

  /** @internal     2.  Update all TSCs */
  TSCButton_update_All();

  uint16_t NumFaders = FadersInternal.NumInitializedFaders;
  uint16_t Index = 0;

  /** @internal     3.  Loop through all faders. Inside the Loop: */
  for(Index = 0; Index < NumFaders; Index++)
  {
    MotorizedFader_structTd* Fader = FadersInternal.InitializedFaders[Index];

    /** @internal     4.  Get the current TSC state */
    TSCButton_State_enumTd TSCState;
    TSCState = TSCButton_get_State(&Fader->TouchSense);

    /** @internal     5.  If TSC is not touched, update PID and move fader with
     *                    the new CCR value. If it is touched, reset PID and
     *                    stop the motor.*/
    if(TSCState == TSCBUTTON_TOUCHED)
    {
      PID_reset(&Fader->PID);
      MotorDriver_stop(&Fader->Motor);
    }
    else if(TSCState == TSCBUTTON_RELEASED)
    {
      int CCR = get_UpdatedPIDOutput(Fader);
      move_Fader(Fader, CCR);
    }
  }
}

/**
 * @brief     Update PID with the new value
 * @param     Fader   pointer to the users fader structure
 * @return    int CCR value calculated by PID
 */
int get_UpdatedPIDOutput(MotorizedFader_structTd* Fader)
{
  int ReturnCCR = 0;
  /** @intenral     1.  Get current ADC sample */
  uint16_t ADCSample = Wiper_get_SmoothValue(&Fader->Wiper);
  /** @internal     2.  Update PID with new sample */
  PID_update(&Fader->PID, (double)ADCSample);
  /** @intenral     3.  Get the round PID outout to return */
  ReturnCCR = PID_get_OutputRound(&Fader->PID);

  return ReturnCCR;
}

/**
 * @brief     Move fader with the new CCR value
 * @param     Fader   pointer to the users fader structure
 * @param     int     CCR value for the motors PWM
 * @returen   none
 */
void move_Fader(MotorizedFader_structTd* Fader, int CCR)
{
  int CCRStartForce = Fader->CCRStartForce;
  int CCRStopRange = Fader->CCRStopRange;

  /** @internal     1.  Check if CCR is bigger then the required start
   *                    CCR value. If yes, move the motor according to the CCR
   *                    sign. - is down, + is up */
  if(CCR < -CCRStartForce)
  {
    /* Move down with PID result */
    MotorDriver_move_CounterClockWise(&Fader->Motor, -1*CCR);
  }
  else if(CCR > CCRStartForce)
  {
    /* Move up with PID result */
    MotorDriver_move_ClockWise(&Fader->Motor, CCR);
  }
  /** @internal     2.  If CCR is not in the Stop CCRT range, but lower
   *                    than the required start force, move the fader with
   *                    the start force value in the direction according to the
   *                    CCR sign*/
  else if(CCR < -CCRStopRange && CCR >= -CCRStartForce)
  {
    /* Move down with Start Force (slowest possible) */
    MotorDriver_move_CounterClockWise(&Fader->Motor, CCRStartForce);
  }

  else if(CCR > CCRStopRange && CCR <= CCRStartForce)
  {
    /* Move up with Start Force (slowest possible) */
   MotorDriver_move_ClockWise(&Fader->Motor, CCRStartForce);
  }
  /** @internal     3.  It the motor is in the Stop CCR range, stop the motor */
  else
  {
   MotorDriver_stop(&Fader->Motor);
  }
}

/* Description in .h */
void MotorizedFader_manage_WiperInterrupt(ADC_HandleTypeDef* hadc)
{
  Wiper_manage_Interrupt(hadc);
}

/* Description in .h */
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


/**@}*//* end of defgroup "MotorFader_Source" */
/**@}*//* end of defgroup "MotorFader" */
