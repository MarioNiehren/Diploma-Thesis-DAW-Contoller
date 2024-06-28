/***************************************************************************//**
 * @defgroup        MotorFader  Motorized Fader
 * @brief           This module can be used to program an Fader with DC-Motor.
 *
 * # How to use:
 * 1. Declare an object of MotorizedFader_structTd type for each used fader
 * 2. Initialize Faders:
 *    - Fader Data:
 *      - MotorizedFader_init_Structure()
 *      - MotorizedFader_init_StartForce()
 *      - MotorizedFader_init_StopRange()
 *    - Wiper:
 *      - MotorizedFader_init_Wiper()
 *    - Touch Sense:
 *      - MotorizedFader_init_TouchTSC()
 *      - MotorizedFader_init_TouchThreshold()
 *      - MotorizedFader_init_TouchDischargeTimeMsAll() (initialize this once
 *        for all faders)
 *    - Motor:
 *      - MotorizedFader_init_MotorPinIn1()
 *      - MotorizedFader_init_MotorPinIn2()
 *      - MotorizedFader_init_MotorPinSTBY()
 *      - MotorizedFader_init_MotorPWM()
 *    - PID:
 *      - MotorizedFader_init_PID();
 *      - MotorizedFader_init_PIDMaxCCR()
 *      - MotorizedFader_init_PIDKpKiKd()
 *      - MotorizedFader_init_PIDLowPass()
 *      - MotorizedFader_init_PIDSampleTimeInMs()
 * 3. Setup Interrupt functions:
 *    - MotorizedFader_manage_WiperInterrupt()
 *    - MotorizedFader_manage_TSCInterrupt()
 * 4. Start all faders:
 *    - MotorizedFader_start_All()
 * 5. In while Loop: update all faders
 *    - MotorizedFader_update_All()
 * 6. Use these functions to access a fader in the running program:
 *    - MotorizedFader_get_WiperValue()
 *    - MotorizedFader_get_TSCState()
 *    - MotorizedFader_set_Target()
 *
 * # Hardware (as used for testing)
 * -  Code was tested with an 100mm linear 10k ALPS motorized fader
 * -  Motor Driver: TB6612FNG
 * -  TSC Sample capacitor: 47nF
 * -  TSC serial resistor on Touch line: 10k
 * -  Motor Supply: 9V
 *
 * # Tips
 * -  If the fader starts to resonate, you have to check Kp, Ki, Kd, low pass,
 *    Start Force and Stop Range. This module was tested with 100mm linear
 *    ALPS faders (RSA0N11M9A07).
 * -  It is recommended to look for other faders than the Alps faders for new
 *    designs, as the discontinued the product.
 * -  Even if only faders of the same type are used, it may be necessary to
 *    setup different PID calibrations. Many faders are very inaccurate and
 *    sluggish according to their specs. If they are a bit worn over time, this
 *    problem gets bigger and the faders need different regulation (if
 *    regulation is still possible. Very old faders have to be replaced)
 * -  If you hear high frequency noise, check your PWM frequency in
 *    Cube MX (PWM-Freuqency = (TIM-CLK / AutoReloadRegister)).
 * -  Take your time to find the right threshold for each Touch line. If the
 *    threshold is not accurate, it my get tricky to get the faders stable
 *    because the motor stops always when the fader is touched.
 *
 * # Links
 * - @ref TB6612FNG_Datasheet "TB6612FNG Motor Driver data sheet"
 * - Helpful PID tutorial (PID calculations are based on this source):
 *    - [PID Controller Implementation in Software - Phil's Lab #6](https://www.youtube.com/watch?v=zOByx3Izf5U)
 *    - [More Phil's Lab videos](https://www.phils-lab.net/videos)
 * - [CONTROLLERSTECH: a helpful blog to learn how to work with STM32, including ADC and PWM](https://controllerstech.com/)
 * - [STM32 TSC Overview](https://wiki.st.com/stm32mcu/wiki/Introduction_to_touch_sensing_with_STM32)
 * - [STM32 ADC Overview](https://wiki.st.com/stm32mcu/wiki/Getting_started_with_ADC)
 * - [Datasheet for ALPS 100mm Fader used for testing](https://cdn-reichelt.de/documents/datenblatt/X200/402685_RSA0N11M9A07.pdf)
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

/**
 * @brief     Link the Fader to the internal structure for internal use.
 * @param     Fader   pointer to the users fader structure
 * @return    none
 */
void MotorizedFader_init_Structure(MotorizedFader_structTd* Fader);

/**
 * @brief     Initialize the start force the fader needs to start moving.
 *            To find the CCR value needed to move the motor, you can use
 *            MotorDriver_move_ClockWise() with the &Fader->Motor and increase
 *            the CCR until the fader starts moving up. Sometimes trial and
 *            error will also lead to a working value.
 * @param     Fader   pointer to the users fader structure
 * @param     CCR     value where the fader starts to move
 * @return    none
 */
void MotorizedFader_init_StartForce(MotorizedFader_structTd* Fader, int CCR);

/**
 * @brief     Initialize a CCR Range, where the motor will not move. This is
 *            required to avoid resonating around the target value. Resonating
 *            is caused because the fader will always move at least with the
 *            start force. As the ADC value will never be completely stable and
 *            reached on point, the fader would always try to correct. This
 *            stop range is a simple approach to resolve this problem.
 * @param     Fader   pointer to the users fader structure
 * @param     CCR     value where the fader does not move. The range will be
 *                    from -CCR to +CCr.
 * @return    none
 */
void MotorizedFader_init_StopRange(MotorizedFader_structTd* Fader, int CCR);

/** @} ************************************************************************/
/* end of name "Initialize Structure"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize Wiper
 * @brief     Use these functions to initialize the faders wiper.
 * @{
 ******************************************************************************/

/**
 * @brief     Link the ADC for the wiper. For details and how to set up ADC
 *            please look at the documentation of Wiper_init_ADC().
 *
 * @note      The wiper sample size is set to 100 on default. Change
 *            NUMBER_OF_SAMPLES in wiper.h if the wiper value has to be
 *            smoother. Do not increase the value too much, because that would
 *            cause, that the ADC value will be updated too slow to be
 *            used by PID. The motor would always overshoot.
 *
 * @param     Fader     pointer to the users fader structure
 * @param     hadc      pointer to the HAL generated ADC-handle of the ADC
 *                      that is used for the wiper
 * @return    none
 */
void MotorizedFader_init_Wiper(MotorizedFader_structTd* Fader, ADC_HandleTypeDef* Handle);

/** @} ************************************************************************/
/* end of name "Initialize Wiper"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize Touch Sense Controller
 * @brief     Use these functions to initialize the faders TSC.
 * @{
 ******************************************************************************/

/**
 * @brief     Link the touch sense controller for the touch line.
 *            For details and how to setup TSC please look at the documentation
 *            of TSCButton_init_TSC().
 * @param     Fader   pointer to the users fader structure
 * @param     htsc      pointer to the HAL generated TSC-handle of the TSC
 *                      that is used for the button
 * @param     IOChannel channel of the TSC-Group used for this button. Please
 *                      use the HAL defines that contain the proper register
 *                      values. This is the naming convention: TSC_GROUPx_IOx.
 *                      For nerds: it is also possible to use the CMSIS register
 *                      define TSC_IOCCR_Gx_IOx.
 *                      You can find the active channels in the htsc
 *                      initialization in MX_TSC_init().
 * @return    none
 */
void MotorizedFader_init_TouchTSC(MotorizedFader_structTd* Fader, TSC_HandleTypeDef* htsc, uint32_t IOChannel);

/**
 * @brief     Initialize the threshold value where the touch line will change
 *            between touched and released.
 *            For details please look at the documentation of
 *            TSCButton_init_Threshold().
 * @param     Fader     pointer to the users fader structure
 * @param     Threshold value when the touch state toggles.
 * @return    none
 */
void MotorizedFader_init_TouchThreshold(MotorizedFader_structTd* Fader, uint16_t Threshold);

/**
 * @brief     Initialize the discharge time for the sample capacitor of the
 *            TSC group. This value is used fop all TSC sample capacitors and
 *            all faders. Only initialize this once for all.
 *            For details please look at the documentation of
 *            TSCButton_init_DischargeTimeMsAll().
 * @param     Fader     pointer to the users fader structure
 * @param     TimeInMs  discharge time
 * @return    none
 */
void MotorizedFader_init_TouchDischargeTimeMsAll(uint8_t TimeInMs);

/** @} ************************************************************************/
/* end of name "Initialize Touch Sense Controller"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize Motor
 * @brief     Use these functions to initialize the faders TSC.
 * @{
 ******************************************************************************/

/**
 * @brief     Link Pin In1 for the Motor Driver.
 *            For details please look at the documentation of
 *            MotorDriver_init_PinIn1().
 * @param     Fader     pointer to the users fader structure
 * @param     GPIO      port of the In 1 Pin
 * @param     Pin       number of In 1
 * @retrun    none
 */
void MotorizedFader_init_MotorPinIn1(MotorizedFader_structTd* Fader, GPIO_TypeDef* GPIO, uint16_t Pin);

/**
 * @brief     Link Pin In2 for the Motor Driver.
 *            For details please look at the documentation of
 *            MotorDriver_init_PinIn2().
 * @param     Fader     pointer to the users fader structure
 * @param     GPIO      port of the In 2 Pin
 * @param     Pin       number of In 2
 * @retrun    none
 */
void MotorizedFader_init_MotorPinIn2(MotorizedFader_structTd* Fader, GPIO_TypeDef* GPIO, uint16_t Pin);

/**
 * @brief     Link Pin STBY for the Motor Driver.
 *            For details please look at the documentation of
 *            MotorDriver_init_PinInSTBY().
 * @param     Fader     pointer to the users fader structure
 * @param     GPIO      port of the In STBY Pin
 * @param     Pin       number of In STBY
 * @retrun    none
 */
void MotorizedFader_init_MotorPinSTBY(MotorizedFader_structTd* Fader, GPIO_TypeDef* GPIO, uint16_t Pin);

/**
 * @brief     Link PWM used for the Motor Driver.
 *            For details and how to setup PWM please look at the
 *            documentation of
 *            MotorDriver_init_PWM().
 * @note      For a very quick PWM explanation, please look at
 *            @ref PWM_Quick_Explanation "this explanation"
 * @param     Fader     pointer to the users fader structure
 * @param     htim      pointer to the HAL-handle for the timer used for PWM
 * @param     Channel   of the timer used for PWM. Use the HAL-defines.
 *                      They have the following format: TIM_CHANNEL_x, where
 *                      x is the channel number.
 * @return    none
 */
void MotorizedFader_init_MotorPWM(MotorizedFader_structTd* Fader, TIM_HandleTypeDef* htim, uint16_t Channel);

/** @} ************************************************************************/
/* end of name "Initialize Motor"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize PID
 * @brief     Use these functions to initialize the PID controller that controls
 *            the motor.
 *            # A "hands on" explanation of the PID controller
 *            - The PID controller is used to calculate the CCR value for the
 *              motor. As motors are sluggish systems, they need to change the
 *              speed all the time while moving to get accurate to the target.
 *              The controller is fed with the current wiper value and will
 *              control the CCR of the motors PWM until the wiper value matches
 *              the target value (in theory, as there will always be noise and
 *              tolerance).
 *            - P = Propotional, I = Integral, D = Derivative
 *            - Here: "error" means the difference between the setpoint (target
 *              value) and the input value.
 *            - Kp is the coefficient to tune the P-Term of the controller.
 *              This term will directly move the fader according to the
 *              error.
 *            - Ki is the coefficient to tune the I-Term of the controller. It
 *              will add the error again and again over time, so it will move
 *              the fader slowly to the target if P is not strong enough
 *              anymore. This is helpful if you are close to the target but not
 *              there yet.
 *            - Kd is the coefficient to tune the D-Term of the controller.
 *              It is used to counteract with P and I to avoid rapid changes.
 *              As it tends to oszillate, it is paired with a low pass here to
 *              reduce this effect. Be careful with the D-Term, as it may cause
 *              instability in the system if it is used too much. But don't be
 *              afraid to use it!
 *            - Experiences from testing: Often the coefficients are in relation
 *              of Kp >> Kd >> Ki. With Alps Faders, the values are often
 *              anywhere in the dimension of Kp = 0.x | Ki = 0.000x and
 *              Kd = 0.0x.
 *            - More calibration tips in the @ref PID_Controller "PID module".
 * @{
 ******************************************************************************/

/**
 * @brief     Initialize the PID structure used by the fader.
 *            For details please look at the documentation of
 *            PID_init().
 * @param     Fader     pointer to the users fader structure
 * @return    none
 */
void MotorizedFader_init_PID(MotorizedFader_structTd* Fader);

/**
 * @brief     Initialize PID Output range
 *            For details please look at the documentation of
 *            PID_set_OutputMinMax().
 * @param     Fader     pointer to the users fader structure
 * @paramm    MaxCCR    largest possible CCR value for PWM
 * @return    none
 */
void MotorizedFader_init_PIDMaxCCR(MotorizedFader_structTd* Fader, uint16_t MaxCCR);

/**
 * @brief     Initialize PID coefficients for tuning the PIT-Terms.
 *            For details please look at the documentation of
 *            PID_set_KpKiKd().
 * @param     Fader     pointer to the users fader structure
 * @param     Kp      coefficient for the proportional term
 * @param     Ki      coefficient for the integral term
 * @param     Kd      coefficient for the derivative term
 * @return    none
 */
void MotorizedFader_init_PIDKpKiKd(MotorizedFader_structTd* Fader, double Kp, double Ki, double Kd);

/**
 * @brief     Initialize the low pass for the PID D-Term.
 *            For details please look at the documentation of
 *            PID_set_LowPass().
 * @param     Fader     pointer to the users fader structure
 * @param     Tau       time constant of the low pass filter
 * @return    none
 */
void MotorizedFader_init_PIDLowPass(MotorizedFader_structTd* Fader, double Tau);

/**
 * @brief     Initialize the sample to control the PID update frequency.
 *            For details please look at the documentation of
 *            PID_set_SampleTimeInMs().
 * @param     Fader     pointer to the users fader structure
 * @param     Threshold of the sample time in milliseconds
 * @return    none
 */
void MotorizedFader_init_PIDSampleTimeInMs(MotorizedFader_structTd* Fader, uint32_t SampleTime);

/** @} ************************************************************************/
/* end of name "Initialize PID"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Process
 * @brief     Use these functions to process all faders
 * @{
 ******************************************************************************/

/**
 * @brief     Call this function once to start all faders
 * @param     none
 * @return    none
 */
void MotorizedFader_start_All(void);

/**
 * @brief     Call this function periodically in the main while loop to update
 *            all faders
 * @param     none
 * @return    none
 */
void MotorizedFader_update_All(void);

/**
 * @brief     Call this function in the HAL ADC interrupt.
 *            For details please look at the documentation of
 *            Wiper_manage_Interrupt().
 *
 * Here is an example:
 * @code
 * void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
 * {
 *   MotorizedFader_manage_WiperInterrupt(&UsersFader, hadc);
 * }
 * @endcode
 *
 * @param     Fader     pointer to the users fader structure
 * @param     hadc      pointer to the HAL generated ADC-handle of the ADC
 *                      that is used for the wiper
 * @return    none
 */
void MotorizedFader_manage_WiperInterrupt(ADC_HandleTypeDef* hadc);

/**
 * @brief     Call this funciton in the HAL TSC Interrupt
 *            For details please look at the documentation of
 *            TSCButton_manage_Interrupt().
 *
 * Here is an example:
 * @code
 * void HAL_TSC_ConvCpltCallback(TSC_HandleTypeDef* htsc)
 * {
 *   MotorizedFader_manage_TSCInterrupt();
 * }
 * @endcode
 *
 * @param     none
 * @return    none
 */
void MotorizedFader_manage_TSCInterrupt(void);

/** @} ************************************************************************/
/* end of name "Process"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Set Functions
 * @brief     Use these functions to set values to the fader
 * @{
 ******************************************************************************/

/**
 * @brief     Use this function to set a Target value for the fader. It will
 *            move to this value automatically.
 * @param     Fader     pointer to the users fader structure
 * @param     Target    value where the fader should move to.
 * @return    none
 */
void MotorizedFader_set_Target(MotorizedFader_structTd* fader, uint16_t Target);

/** @} ************************************************************************/
/* end of name "Set Functions"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Get Functions
 * @brief     Use these functions to get values from the fader
 * @{
 ******************************************************************************/

/**
 * @brief     Use this function to get the wiper value. This value can be used
 *            to control something else, e.g. a protocol value to mode a
 *            DAW fader.
 * @param     Fader     pointer to the users fader structure
 * @return    current ADC value of the wiper
 */
uint16_t MotorizedFader_get_WiperValue(MotorizedFader_structTd* Fader);

/**
 * @brief     Use this function to check if a fader is touched or not.
 * @param     Fader     pointer to the users fader structure
 * @return    Touch State: TSCBUTTON_TOUCHED or TSCBUTTON_RELEASED
 */
TSCButton_State_enumTd MotorizedFader_get_TSCState(MotorizedFader_structTd* Fader);

/** @} ************************************************************************/
/* end of name "Get Functions"
 ******************************************************************************/


/**@}*//* end of defgroup "MotorFader_Header" */
/**@}*//* end of defgroup "MotorFader" */

#endif /* INC_FADER_MOTORIZEDFADER_H_ */
