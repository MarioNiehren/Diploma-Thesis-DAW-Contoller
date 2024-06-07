/***************************************************************************//**
 * @defgroup        TSCButton Buttons with STM32 Touch Sense Control (TSC)
 * @brief           This module manages the STM32 touch sense controller to use
 *                  is as capacitive buttons. It also provides the raw
 *                  TSC-values for each button. This may be useful for non
 *                  binary use.
 *
 * # How to use:
 * 1. Initialize the TSC for each button
 * 2. Initialize the sample capacitor discharge time used for all buttons
 * 3. Initialize threshold for each button (read TSCButton_init_DischargeTime()
 *    documentation for a more detailed explanation)
 * 5. Initialize start conditions with TSCButton_init_GeneralStartConditions()
 * 4. Setup TSCButton_manage_Interrupt()
 * 5. call TSCButton_updateAll() periodical in the main while loop
 * 6. use get functions to read the states.
 *
 * @note  TSCButton_init_GeneralStartConditions() must be the last thing
 *        to initialize before starting to update!
 *
 * # Hardware requirements:
 * - use a serial 10k resistor for each touch line
 * - use a Sampling Capacitor in the range of 22-200nF
 *
 * # Links:
 * @anchor          TSCIntroduction [STM32 TSC introduction](https://wiki.st.com/stm32mcu/wiki/Introduction_to_touch_sensing_with_STM32)
 *
 * @defgroup        TSCButton_Header  Header
 * @brief           Study this part for a quick overview of this module.
 *
 * @addtogroup      MotorFader
 * @{
 *
 * @addtogroup      TSCButton
 * @{
 *
 * @addtogroup      TSCButton_Header
 * @{
 *
 * @file            tscFader.h
 *
 * @date            Sep 6, 2023
 * @author          Mario Niehren
 ******************************************************************************/

#ifndef INC_TSC_BUTTON_H_MN
#define INC_TSC_BUTTON_H_MN

#include "stm32l0xx_hal.h"
#include "timer.h"
#include <stdbool.h>

/**
 * @brief     This define represents the max number of TSC-Groups. Change, if
 *            the used MCU offers a different number of TSC Groups.
 */
#define NUM_TSC_GROUPS  8   /**< Number of available TSC groups */
#define NUM_FREE_IOS    3   /**< 3 because 1 is used by sampling capacitor. */
#define MAX_TSC_BUTTONS (NUM_TSC_GROUPS * NUM_FREE_IOS)
#define TSC_NUM_SAMPLES 50  /**< Number of values stored to calculate a
                                 smooth value */

/***************************************************************************//**
 * @name      Structures and Enumerations
 * @{
 ******************************************************************************/
typedef enum
{
  TSCBUTTON_TOUCHED = 1,
  TSCBUTTON_RELEASED = 0
}TSCButton_State_enumTd;

typedef struct
{
  TSC_HandleTypeDef*      htsc;
  TSC_IOConfigTypeDef     ioConfigTsc;
  TSCButton_State_enumTd  state;
  uint32_t  Samples[TSC_NUM_SAMPLES];
  uint8_t   SamplesIndex;
  uint32_t  SmoothValue;
  uint32_t  RawValue;
  uint16_t  threshold;
  uint32_t  TSCGroup;
}TSCButton_structTd;

/** @} ************************************************************************/
/* end of name "Structures and Enumerations"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize the touch sense controller
 *            based button.
 * @{
 ******************************************************************************/

/**
 * @brief     Link the htsc handler to this module and specify the IO Channel.
 *            All Buttons have to be initialized before starting TSC.
 *            If you add buttons while TSC is running, internal counters might
 *            get confused and crash the program.
 * @param     tsc       pointer to the users tsc structure
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
void TSCButton_init_TSC(TSCButton_structTd* tsc, TSC_HandleTypeDef* htsc, uint32_t IOChannel);

/**
 * @brief     Setup the threshold where the TSC button will toggle between
 *            touched and released
 * @note      How to evaluate a proper value:
 *            1. Let the TSC button run without a threshold
 *            2. read the raw value (e.g. with live expression) and
 *               look what the values are when touched and when released.
 *            3. Choose a value in between and set it up as threshold.
 * @param     tsc       pointer to the users tsc structure
 * @prarm     value     of the threshold
 * @return    none
 */
void TSCButton_init_Threshold(TSCButton_structTd* tsc, uint16_t value);

/**
 * @brief     Setup the discharge time in ms, that is required to discharge the
 *            sensing capacitor. Depending
 *            on the used capacitor, 1ms may be enough.
   @note      Only setup this value once for all TSC Keys! Adapt the value to
              the slowest sampling capacitor.
 * @param     value     of the discharge time
 * @return    none
 */
void TSCButton_init_DischargeTimeMsAll(uint8_t value);

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Process TSC
 * @brief     Use these functions to control the TSC for all buttons
 * @{
 ******************************************************************************/

/**
 * @brief     This function sets the module to start conditions.
 * @note      This function must be called once before updating and after
 *            initialization.
 * @param     none
 * @return    none
 */
void TSCButton_start_All(void);

/**
 * @brief     Update the TSC for all buttons. Calöl this funciton periodically
 *            in the main while loop
 * @param     none
 * @return    none
 */
void TSCButton_update_All(void);

/**
 * @brief     Call this function in the HAL-Interrupt handler to set an internal
 *            interrupt flag for the TSC module.
 *
 * Here is an example:
 * @code
 * void HAL_TSC_ConvCpltCallback(TSC_HandleTypeDef* htsc)
 * {
 *   TSCButton_manage_Interrupt();
 * }
 * @endcode
 *
 * @param     none
 * @return    none
 */
void TSCButton_manage_Interrupt(void);

/** @} ************************************************************************/
/* end of name "Process TSC"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Get Functions
 * @brief     Use these functions to get values for the TSC-Buttons
 * @{
 ******************************************************************************/

/**
 * @brief     Call this function to get the current state of a TSC-Button.
 * @param     tsc       pointer to the users tsc structure
 * @return    state of the tsc structure.
 */
TSCButton_State_enumTd TSCButton_get_State(TSCButton_structTd* tsc);

/**
 * @brief     Call this function to get the current raw TSC value of a Button.
 * @param     tsc       pointer to the users tsc structure
 * @return    value of the last TSC measurement for this tsc structure
 */
uint32_t TSCButton_get_RawValue(TSCButton_structTd* tsc);

/**
 * @brief     Call this function to get the current smooth TSC value of a Button.
 * @param     tsc       pointer to the users tsc structure
 * @return    value of the last TSC measurement for this tsc structure
 */
uint32_t TSCButton_get_SmoothValue(TSCButton_structTd* tsc);

/** @} ************************************************************************/
/* end of name "Get Functions"
 ******************************************************************************/

/**@}*//* end of defgroup "TSCButton_Header" */
/**@}*//* end of defgroup "TSCButton" */
/**@}*//* end of defgroup "MotorFader" */

#endif /* INC_TSC_BUTTON_H_MN */
