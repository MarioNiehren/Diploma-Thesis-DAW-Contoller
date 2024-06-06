/***************************************************************************//**
 * @defgroup        PID_Controller PID controller
 * @brief           This module is used to calculate a PID output to reach a
 *                  specific and stable target.
 *
 * # How to use:
 * 1. Declare an object of PID_structTd data type
 * 2. Initialize this object with init funciton
 * 3. Use set functions to tune PID.
 *    - Set Kp, Ki, Kd
 *    - Set Low Pass
 *    - Set Limits for output value
 *    - Set Sample Time for controlled update frequency
 * 4. Set Target
 * 5. In while loop:
 *    - Get the current value of the controlled system
 *    - Update PID with the current value
 *    - Use Get-Functions to get the PID results and use them to correct the
 *      system that is controlled.
 *
 * # A manual approach for calibration:
 * 1. Set Kp until the output gets as close as possible to the target without
 *    overshooting
 * 2. Set Ki to come closer to the target. Try to find a good balance between
 *    speed and overshooting
 * 3. Set Kp if it is required to reach the target faster. Be careful because
 *    the D term tends to bring noise and heavy overshooting to the system.
 *    Many systems do not require a D-Term.
 *
 * # Links
 * The Code is based on this Video: https://www.youtube.com/watch?v=zOByx3Izf5U
 * by Phil's Lab
 *
 * @defgroup        PID_Header      Header
 * @brief           Study this part for a quick overview.
 *
 * @addtogroup      MotorFader
 * @{
 *
 * @addtogroup      PID_Controller
 * @{
 *
 * @addtogroup      PID_Header
 * @{
 *
 * @file            pidController.h
 *
 * @date            Sep 5, 2023
 * @author          Mario Niehren
 ******************************************************************************/

#ifndef INC_PERIPHERALS_FADER_PIDCONTROLLER_H_
#define INC_PERIPHERALS_FADER_PIDCONTROLLER_H_

#include "main.h"
#include "timer.h"


/***************************************************************************//**
 * @name      Structures and Enumerations
 * @{
 ******************************************************************************/

/**
 * @brief     Main structure used to store all data for the PID.
 *            The user has to declare an Object of this type for each used
 *            PID-controller.
 */
typedef struct
{
  double    Kp;         /**< P coefficient to tune the P-Term */
  double    Ki;         /**< I coefficient to tune the I-Term */
  double    Kd;         /**< D coefficient to tune the D-Term */

  double    PTerm;      /**< P-Term from last calculation */
  double    ITerm;      /**< I-Term from last calculation */
  double    DTerm;      /**< D-Term from last calculation */

  double    Setpoint;   /**< Target value of the system */
  double    Sample;     /**< Sample of the current value of the system*/

  double    Error;      /**< Deviation from sample to set point in the current
                             calculation */
  double    PrevError;  /**< Deviation from sample to set point of the previous
                              calculation */

  double    OutputMin;  /**< Minimal limit of the output value */
  double    OutputMax;  /**< Maximum limit of the output value */

  double    TauLowPass; /**< Value to tune the low pass on the D-Term */

  double    OutputRaw;  /**< Result of the PID */
  int       OutputRound;/**< Round value of the result of the PIDS*/

  uint32_t  SampleTime; /**< Timer threshold for the update frequency (ms)*/
  timer_setup_structTd SampleTimer; /**< Timer for internal use */

}PID_structTd;

/** @} ************************************************************************/
/* end of name "Structures and Enumerations"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize the PID controller
 * @{
 ******************************************************************************/

/**
 * @brief     Initialize the users PID-structure to make sure that the start
 *            values are properly set.
 * @param     PID     pointer to the users PID structure
 * @return    none
 */
void PID_init(PID_structTd* PID);

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      (Re-)Set Functions
 * @brief     Use these functions to set or reset parameters
 * @{
 ******************************************************************************/

/**
 * @brief     Set the value range for the PID output.
 * @param     PID     pointer to the users PID structure
 * @param     Min     value
 * @param     Max     value
 * @return    none
 */
void PID_set_OutputMinMax(PID_structTd* PID, double Min, double Max);

/**
 * @brief     Set the coefficients for proportional, integral and derivative
 *            terms.
 * @param     PID     pointer to the users PID structure
 * @param     Kp      coefficient for the proportional term
 * @param     Ki      coefficient for the integral term
 * @param     Kd      coefficient for the derivative term
 * @return    none
 */
void PID_set_KpKiKd(PID_structTd* PID, double Kp, double Ki, double Kd);

/**
 * @brief     Set new set point for the PID Controller as new target of the
 *            controlled system.
 * @param     PID     pointer to the users PID structure
 * @param     setpoint  for the PID controller
 * @return    none
 */
void PID_set_Target(PID_structTd* PID, double setpoint);

/**
 * @brief     Set tau to tune the low pass used on the D-Term to reduce noise.
 *            Increase tau for a lower crossover frequency. Be careful that the
 *            D-Term does not get "killed" by this filter.
 * @param     PID     pointer to the users PID structure
 * @param     Tau   @todo: Look what Tau is exactly!!
 * @return    none
 */
void PID_set_LowPass(PID_structTd* PID, double Tau);

/**
 * @brief     Set the sample time to control the update frequency of the
 *            PID-controller. Make sure that the program is fast enough to call
 *            the update function at least as fast as the sample time. Otherwise
 *            the sample time will be uncontrolled slower.
 * @param     PID     pointer to the users PID structure
 * @param     Threshold of the sample time in milliseconds
 * @return    none
 */
void PID_set_SampleTimeInMs(PID_structTd* PID, uint32_t Threshold);

/**
 * @brief     Reset all values from previous calculations in the structure to
 *            make the PID-controller ready for a clean start.
 * @param     PID     pointer to the users PID structure
 * @return   none
 */
void PID_reset(PID_structTd* PID);

/** @} ************************************************************************/
/* end of name "(Re-)Set Functions"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Process
 * @brief     Use these functions to calculate the PID
 * @{
 ******************************************************************************/

/**
 * @brief     Call this function periodically calculate the newest PID output.
 *            Be careful to call this function faster as the set sample time,
 *            otherwise it might get difficult to calibrate the PID-terms.
 * @param     PID     pointer to the users PID structure
 * @param     Sample  is the current process variable
 * @return    result of the PID calculation with the new sample.
 */
void PID_update(PID_structTd* PID, double sample);

/** @} ************************************************************************/
/* end of name "Process"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Get Functions
 * @brief     Use these functions get values from the PID
 * @{
 ******************************************************************************/

/**
 * @brief     Get raw output value of the last PID calculation.
 * @param     PID     pointer to the users PID structure
 * @return    Raw PID output value
 */
double PID_get_OutputRaw(PID_structTd* PID);

/**
 * @brief     Get round output value of the last PID calculation.
 * @param     PID     pointer to the users PID structure
 * @return    Round PID output value
 */
int PID_get_OutputRound(PID_structTd* PID);

/** @} ************************************************************************/
/* end of name "Get Functions"
 ******************************************************************************/

/**@}*//* end of defgroup "PID_Header" */
/**@}*//* end of defgroup "PID_Controller" */
/**@}*//* end of defgroup "MotorFader" */

#endif /* INC_PERIPHERALS_FADER_PIDCONTROLLER_H_ */
