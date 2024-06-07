/***************************************************************************//**
 * @defgroup        MotorFader  Motorized Fader
 * @brief           This module can be used to program an Fader with DC-Motor.
 *
 * # How to use:
 * @todo!!!
 *
 * # Links
 * @todo!!!
 *
 * @defgroup        MotorFader_Header      Header
 * @brief           Study this part for a quick overview.
 *
 * @addtogroup      MotorFader
 * @{
 *
 * @addtogroup      MotorFader_Header
 * @{
 *
 * @file            motorizedFader.h
 *
 * @date            Sep 4, 2023
 * @author          Mario Niehren
 ******************************************************************************/

#ifndef INC_FADER_MOTORIZEDFADER_H_
#define INC_FADER_MOTORIZEDFADER_H_

#include <math.h>

#include "tscButton.h"
#include "TB6612FNG_MotorDriver.h"
#include "wiper.h"
#include "pidController.h"

/**
 * @brief     This number can be changed according to the users requirements.
 *            Make sure it is equal or higher than the actual used faders.
 */
#define NUMBER_OF_MOTORIZED_FADERS 8

/***************************************************************************//**
 * @name      Structures and Enumerations
 * @{
 ******************************************************************************/

/**
 * @brief     Main structure to store all data for the fader. The user has to
 *            declare one object of this data type for each fader.
 */
typedef struct
{
  Wiper_structTd  Wiper;
  TB6612FNGMotorDriver_structTd Motor;
  PID_structTd PID;
  TSCButton_structTd TouchSense;
  int CCRStartForce;
  int CCRStopRange;
}MotorizedFader_structTd;

/** @} ************************************************************************/
/* end of name "Structures and Enumerations"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize Structure
 * @brief     Use these functions to initialize the faders data structure.
 * @{
 ******************************************************************************/

void MotorizedFader_init_Structure(MotorizedFader_structTd* Fader);
void MotorizedFader_init_StartForce(MotorizedFader_structTd* Fader, int CCR);
void MotorizedFader_init_StopRange(MotorizedFader_structTd* Fader, int CCR);

/** @} ************************************************************************/
/* end of name "Initialize Structure"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize Wiper
 * @brief     Use these functions to initialize the faders wiper.
 * @{
 ******************************************************************************/

void MotorizedFader_init_Whiper(MotorizedFader_structTd* Fader, ADC_HandleTypeDef* Handle);

/** @} ************************************************************************/
/* end of name "Initialize Wiper"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize Touch Sense Controller
 * @brief     Use these functions to initialize the faders TSC.
 * @{
 ******************************************************************************/

void MotorizedFader_init_TouchTSC(MotorizedFader_structTd* fader, TSC_HandleTypeDef* htsc, uint32_t IOChannel);
void MotorizedFader_init_TouchThreshold(MotorizedFader_structTd* fader, uint16_t threshold);
void MotorizedFader_init_TouchDischargeTimeMsAll(uint8_t value);

/** @} ************************************************************************/
/* end of name "Initialize Touch Sense Controller"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize Motor
 * @brief     Use these functions to initialize the faders TSC.
 * @{
 ******************************************************************************/

void MotorizedFader_init_MotorPinIn1(MotorizedFader_structTd* Fader, GPIO_TypeDef* GPIO, uint16_t Pin);
void MotorizedFader_init_MotorPinIn2(MotorizedFader_structTd* Fader, GPIO_TypeDef* GPIO, uint16_t Pin);
void MotorizedFader_init_MotorPinSTBY(MotorizedFader_structTd* Fader, GPIO_TypeDef* GPIO, uint16_t Pin);
void MotorizedFader_init_PWM(MotorizedFader_structTd* Fader, TIM_HandleTypeDef* htim, uint16_t Channel);
/** @} ************************************************************************/
/* end of name "Initialize Motor"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize PID
 * @brief     Use these functions to initialize the PID controller that controls
 *            the motor.
 * @{
 ******************************************************************************/

void MotorizedFader_init_PID(MotorizedFader_structTd* Fader);
void MotorizedFader_init_PIDMaxCCR(MotorizedFader_structTd* Fader, uint16_t MaxCCR);
void MotorizedFader_init_PIDKpKiKd(MotorizedFader_structTd* Fader, double Kp, double Ki, double Kd);
void MotorizedFader_init_PIDLowPass(MotorizedFader_structTd* Fader, double Tau);
void MotorizedFader_init_PIDSampleTimeInMs(MotorizedFader_structTd* Fader, uint32_t SampleTime);

/** @} ************************************************************************/
/* end of name "Initialize PID"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Process
 * @brief     Use these functions to process all faders
 * @{
 ******************************************************************************/

void MotorizedFader_start_All(void);
void MotorizedFader_update_All(void);
void MotorizedFader_manage_WiperInterrupt(ADC_HandleTypeDef* hadc);
void MotorizedFader_manage_TSCInterrupt(void);

/** @} ************************************************************************/
/* end of name "Process"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Set Functions
 * @brief     Use these functions to set values to the fader
 * @{
 ******************************************************************************/

void MotorizedFader_set_Target(MotorizedFader_structTd* fader, uint16_t Target);

/** @} ************************************************************************/
/* end of name "Set Functions"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Get Functions
 * @brief     Use these functions to get values from the fader
 * @{
 ******************************************************************************/

uint16_t MotorizedFader_get_WiperValue(MotorizedFader_structTd* Fader);
TSCButton_State_enumTd MotorizedFader_get_TSCState(MotorizedFader_structTd* Fader);

/** @} ************************************************************************/
/* end of name "Get Functions"
 ******************************************************************************/


/**@}*//* end of defgroup "MotorFader_Header" */
/**@}*//* end of defgroup "MotorFader" */

#endif /* INC_FADER_MOTORIZEDFADER_H_ */
