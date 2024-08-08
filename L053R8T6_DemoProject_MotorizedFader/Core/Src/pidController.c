/***************************************************************************//**
 * @defgroup        PID_Source      Source
 * @brief           Study this part for details.
 *
 * @addtogroup      MotorFader
 * @{
 *
 * @addtogroup      PID_Controller
 * @{
 *
 * @addtogroup      PID_Source
 * @{
 *
 * @file            pidController.c
 *
 * @date            Sep 5, 2023
 * @author          Mario Niehren
 ******************************************************************************/

#include <pidController.h>
#include <math.h>

/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize the PID controller
 * @{
 ******************************************************************************/

/* Description in .h */
void PID_init(PID_structTd* pid)
{
  pid->DTerm = 0.0;
  pid->ITerm = 0.0;
  pid->OutputRaw = 0.0;
}

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      (Re-)Set Functions
 * @brief     Use these functions to set or reset parameters
 * @{
 ******************************************************************************/

/* Description in .h */
void PID_set_OutputMinMax(PID_structTd* PID, double Min, double Max)
{
  PID->OutputMin = Min;
  PID->OutputMax = Max;
}

/* Description in .h */
void PID_set_KpKiKd(PID_structTd* PID, double Kp, double Ki, double Kd)
{
  PID->Kp = Kp;
  PID->Ki = Ki;
  PID->Kd = Kd;
}

/* Description in .h */
void PID_set_LowPass(PID_structTd* PID, double Tau)
{
  PID->TauLowPass = Tau;
}

/* Description in .h */
void PID_set_SampleTimeInMs(PID_structTd* PID, uint32_t Threshold)
{
  /** @internal     1.  Set Threshold to the Timer */
  Timer_set_ThresholdInMs(&PID->SampleTimer, Threshold);
  /** @internal     2.  Store threshold value local for PID calculations */
  PID->SampleTime = Threshold;
}

/* Description in .h */
void PID_set_Target(PID_structTd* PID, double setpoint)
{
  PID->Setpoint = setpoint;
}

/* Description in .h */
void PID_reset(PID_structTd* pid)
{
  pid->Sample = 0x00;
  pid->PTerm = 0.00;
  pid->ITerm = 0.00;
  pid->DTerm = 0.00;
  pid->Error = 0.00;
  pid->OutputRaw = 0.00;
}

/** @} ************************************************************************/
/* end of name "(Re-)Set Functions"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Process
 * @brief     Use these functions to calculate the PID
 * @{
 ******************************************************************************/

/** @cond *//* Function Prototypes */
double calculate_Error(PID_structTd* pid, double sample);
double calculate_PTerm(PID_structTd* pid, double Error);
double calculate_ITermWithAntiWindup(PID_structTd* pid, double PTerm, double Error);
double calculate_DTermWithLowPass(PID_structTd* pid, double sample);
double limit_Output(double Output, double Min, double Max);
/** @endcond *//* Function Prototypes */

/* Description in .h */
void PID_update(PID_structTd* pid, double sample)
{
  /** @internal     1. Check it Sample-Time elapsed. Leave function if not!*/
  if(Timer_check_TimerElapsed(&pid->SampleTimer))
  {
    double OutputMin = pid->OutputMin;
    double OutputMax = pid->OutputMax;

    /** @internal     2.  Calculate error between set point and current sample */
    double Error = calculate_Error(pid, sample);

    /** @internal     3.  Calculate proportional Term */
    double PTerm = calculate_PTerm(pid, Error);

    /** @internal     4.  Calculate integral Term with anti wind up */
    double ITerm =  calculate_ITermWithAntiWindup(pid, PTerm, Error);

    /** @internal     5.  Calculate derivative Term with low pass to avoid
     *                    noise */
    double DTerm =  calculate_DTermWithLowPass(pid, sample);

    /** @internal     6.  calculate output value */
    double Output = PTerm + ITerm + DTerm;

    /** @internal     7.  limit output value */
    Output = limit_Output(Output, OutputMin, OutputMax);

    /** @internal     8.  save output to users PID structure (raw and round) */
    pid->OutputRaw = Output;
    pid->OutputRound = round(Output);
  }
}

/**
 * @brief     calculate error of between input value and set point.
 * @param     PID     pointer to the users PID structure
 * @param     Sample  of the current input value
 * @return    Error value
 */
double calculate_Error(PID_structTd* pid, double Sample)
{
  double Error = 0.0;
  double Setpoint = pid->Setpoint;
  /** @internal     1.  Calculate Error */
  Error = Setpoint - Sample;
  /** @internal     2.  Save error for the next calculation cycle. */
  pid->PrevError = pid->Error;
  pid->Error = Error;

  return Error;
}

/**
 * @brief     Calculate the proportional Term of the PID controller.
 * @param     PID     pointer to the users PID structure
 * @param     Error   of the current calculation process
 * @return    P-Term
 */
double calculate_PTerm(PID_structTd* pid, double Error)
{
  double PTerm = 0.0;
  double Kp = pid->Kp;
  /** @internal     1.  Calculate P-Term */
  PTerm = Kp * Error;

  pid->PTerm = PTerm;

  return  PTerm;
}

/**
 * @brief     Calculate the integral Term of the PID controller with anti wind
 *            up.
 * @param     PID     pointer to the users PID structure
 * @param     PTerm   proportional term of the current calculation cycle
 * @param     Error   of the current calculation process
 * @return    I-Term with anti wind-up
 */
double calculate_ITermWithAntiWindup(PID_structTd* pid, double PTerm, double Error)
{
  double ITerm     = 0.0;
  double ITermMin  = 0.0;
  double ITermMax  = 0.0;
  double PrevITerm  = pid->ITerm;
  double SampleTime = pid->SampleTime;
  double Ki         = pid->Ki;
  double OutputMin  = pid->OutputMin;
  double OutputMax  = pid->OutputMax;
  double PrevError  = pid->PrevError;

  /** @internal     1.  Calculate Integral term */
  ITerm = PrevITerm + 0.5 * Ki * SampleTime * (Error + PrevError);

  /** @internal     2.  Calculate I-Term limits for anti wind-up */
  if(OutputMin < PTerm)
  {
    ITermMin = OutputMin - PTerm;
  }
  if(OutputMax > PTerm)
  {
    ITermMax = OutputMax + PTerm;
  }

  /** @internal     3.  Limit I-Term if necessary */
  ITerm = fmax(ITerm, ITermMin);
  ITerm = fmin(ITerm, ITermMax);

  /** @internal     4.  Save I-Term for next calculation cycle */
  pid->ITerm = ITerm;
  
  return ITerm;
}

/**
 * @brief     Calculate derivative term with low pass to avoid noise.
 * @param     PID     pointer to the users PID structure
 * @param     Sample  of the current input value
 * @return    D-Term
 */
double calculate_DTermWithLowPass(PID_structTd* pid, double sample)
{
  double SampleTime = pid->SampleTime;
  double PrevSample = pid->Sample;
  double TauLowPass = pid->TauLowPass;
  double DTerm = 0.0;
  double PrevDTerm = pid->DTerm;
  double Kd = pid->Kd;

  double NumeratorDifferentiatorPortion = -2.0 * Kd * (sample - PrevSample);
  double NumeratorLowPassPortion = (2.0 * TauLowPass - SampleTime) * PrevDTerm;

  DTerm = (NumeratorDifferentiatorPortion + NumeratorLowPassPortion) /
          (2.0 * TauLowPass + SampleTime);

  pid->Sample = sample;
  pid->DTerm = DTerm;

  return DTerm;
}

/**
 * @brief     Limit Output
 * @param     Output  of current calculation cycle without limits.
 * @param     Min     output value
 * @param     Max     output value
 * @return    limited PID output
 */
double limit_Output(double Output, double Min, double Max)
{
  double OutputLimited = Output;

  OutputLimited = fmax(OutputLimited, Min);
  OutputLimited = fmin(OutputLimited, Max);

  return OutputLimited;
}

/** @} ************************************************************************/
/* end of name "Process"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Get Functions
 * @brief     Use these functions get values from the PID
 * @{
 ******************************************************************************/

/* Description in .h */
double PID_get_OutputRaw(PID_structTd* PID)
{
  return  PID->OutputRaw;
}

/* Description in .h */
int PID_get_OutputRound(PID_structTd* PID)
{
  return  PID->OutputRound;
}

/** @} ************************************************************************/
/* end of name "Get Functions"
 ******************************************************************************/

/**@}*//* end of defgroup "PID_Source" */
/**@}*//* end of defgroup "PID_Controller" */
/**@}*//* end of defgroup "MotorFader" */
