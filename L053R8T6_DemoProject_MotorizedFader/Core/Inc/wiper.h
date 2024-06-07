/***************************************************************************//**
 * @defgroup				WiperMotorFader	Wiper module
 * @brief						This module is responsible to handle the wiper of the
 * 									motorized Fader. It will read the ADC, smooth the values and
 * 									returns the results.
 *
 * # How to use:
 *  1.  Initialize @ref Initialize "ADC and hysteresis"
 *  2.  Setup interrupt function
 *  3.  Start all wipers (this starts the ADC)
 *  4.  Update wipers regularly in While loop.
 *  5.  get values with get functions
 *
 *  # Tip
 *  This module can also be used for regular potentiometers as they work with
 *  the same concept!
 *
 * @defgroup				Wiper_Header			Header
 * @brief						Study this part for a quick overview of wiper functions.
 *
 * @addtogroup			MotorFader
 * @{
 *
 * @addtogroup			WiperMotorFader
 * @{
 *
 * @addtogroup			Wiper_Header
 * @{
 *
 * @file						wiper.h
 *
 * @date						Sep 4, 2023
 * @author					Mario Niehren
 ******************************************************************************/

#ifndef INC_FADER_WIPER_H_MN
#define INC_FADER_WIPER_H_MN

#include "stm32l0xx_hal.h"
#include <stdbool.h>

/**
 * @brief     Number of available ADC channels of the MCU. Change this value
 *            if the used MCU offers more channels.
 */
#define NUM_ALL_ADC_CHANNELS  19

/**
 * @brief     Number of available ADCs (not ADC channels!)
 */
#define NUM_ALL_ADC_ON_MCU 1

/**
 * @brief			Define for the number of sampled values. If the smooth value
 * 						is not clean enough, increase this number. Be careful that the
 * 						ADC does not get to slow by using to many samples.
 */
#define NUMBER_OF_SAMPLES 200

/***************************************************************************//**
 * @name			Structures
 * @brief			This structure is used to store all information about the used
 * 						wiper.
 * @{
 ******************************************************************************/

typedef struct Wiper_structTd
{
  uint16_t  ValueSmooth;								/**<	Current smoothed ADC value */
  uint16_t  ValueRaw;                   /**<  Current raw value of the ADC */
  uint16_t  Samples[NUMBER_OF_SAMPLES];	/**<	Array of measured ADC values.*/
  uint8_t   SamplesIndex;								/**<	Counter for measured ADC values.*/
  uint8_t   Hyst_Threshold;             /**<  Hysteresis Value Range for both
                                              directions. */
  uint16_t  Hyst_NumSmallerValues;      /**<  Counter for values inside the
                                              hysteresis range that are smaller
                                              then the current number */
  uint16_t  Hyst_NumBiggerValues;       /**<  Counter for values inside the
                                              hysteresis range that are bigger
                                              then the current number */
  uint16_t  Hyst_DeviationThreshold;    /**<  This value is the threshold,  when
                                              a value differs to much in one
                                              direction, so it has to be
                                              adjusted */
  uint8_t   IndexBufferDMA;             /**<  Index, where the data for this
                                              Wiper will be buffered by DMA.
                                              The buffer itself is part of the
                                              source code. */
}Wiper_structTd;

/** @} ************************************************************************/
/* end of name "Structures"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Initialize
 * @brief			Use these functions to initialize the wiper of the fader
 * @{
 ******************************************************************************/

/**
 * @brief			Link HAL-ADC-handle used for wiper
 *
 * # How to setup ADC for this module:
 * -	Control the speed of your ADC with the prescaler.
 * -
 *
 *
 * @todo			Add ADC Setup here!!
 *
 * @warning   It is necessary to initialize the ADC in the same order as the
 *            channels of the ADC are set up. So ADC1 Ch1 first and ADCx Chx
 *            last. Otherwise the ADC value will be written to the wrong wiper
 *            structure. It is no problem to skip channels if they are not in
 *            use (The skipped channels must not be set up in Cube MX!).
 *
 * @param			Wiper			  pointer to the users wiper structure
 * @param			hadc				pointer to the HAL generated ADC-handle of the ADC
 * 												that is used for the wiper
 * @return		none
 */
void Wiper_init_ADC(Wiper_structTd* Wiper, ADC_HandleTypeDef* hadc);

/**
 * @brief     Set hysteresis settings to default.
 * @param     Wiper      pointer to the users wiper structure
 * @return    none
 */
void Wiper_init_Hysteresis(Wiper_structTd* Wiper);

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Calibrate
 * @brief     Use this function to calibrate the wiper
 * @note      These functions are not essential, but may be helpful to improve
 *            the performance.
 * # How to calibrate:
 * ## Hardware (if possible):
 * -  make sure that the ADC is working with a clean Vref
 * ## Software:
 * 1. Change HAL-Settings to get as close as possible to the preferred result.
 *    Where to adapt:
 *    - HCLK (if it can be changed without affecting other components) and / or
 *      Prescaler to reduce clock of ADC
 *    - Sampling Time
 * 2. Change define of @ref NUMBER_OF_SAMPLES until value is closer to the
 *    preferred result
 * 3. Change hysteresis threshold to reduce noise
 * 4. Change deviation threshold to get closer to the exact value inside the
 *    hysteresis range.
 *
 * @{
 ******************************************************************************/

/**
 * @brief     Calibrate the hysteresis threshold.
 *
 * Default: 4
 *
 * @param     Wiper     pointer to the users wiper structure
 * @param     Threshold of the hysteresis. The hysteresis range will be from
 *                      the current value - Threshold until the current value +
 *                      Threshold.
 * @return    none
 */
void Wiper_calibrate_HysteresisThreshold(Wiper_structTd* Wiper, uint8_t Threshold);

/**
 * @brief     Calibrate the Deviation Threshold. This will define, when
 *            the value will be corrected, if the value thrifts inside the
 *            hysteresis threshold.
 *
 * Default: 200
 *
 * @param     Wiper     pointer to the users wiper structure
 * @param     Threshold of the deviation. All values that are not equal to the
 *                      current value are counted in separate counters,
 *                      depending if they are lower or higher. If the counter
 *                      of one direction + this threshold gets bigger then the
 *                      counter of the other direction, the current value will
 *                      be adjusted (+1 or -1).
 * @return    none
 */
void Wiper_calibrate_HysteresisDeviationThreshold(Wiper_structTd* Wiper, uint8_t Threshold);

/** @} ************************************************************************/
/* end of name "Calibrate"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Process ADC
 * @brief			Use these functions to control the ADC
 * @{
 ******************************************************************************/

/**
 * @brief			Function to start the ADC over DMA conversion. Call this function
 * 						once when the ADC should start.
 * @param			Wiper		pointer to the users wiper structure
 * @return		none
 */
void Wiper_start_All(void);

/**
 * @brief			Call this function periodically in while function to fill the
 *            wipers with samples from the DMA buffer.
 * @param			none
 * @return		none
 */
void Wiper_update_All(void);

/**
 * @brief			Call this function in the HAL-Interrupt handler to set an internal
 * 						interrupt flag for the wiper module.
 *
 * Here is an example:
 * @code
 * void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
 * {
 *   Wiper_manage_Interrupt(&UsersWhiper, hadc);
 * }
 * @endcode
 *
 * @param		  hadc			pointer to the HAL generated ADC-handle of the ADC
 * 											that is used for the wiper
 * @return		none
 */
void Wiper_manage_Interrupt(ADC_HandleTypeDef* hadc);

/** @} ************************************************************************/
/* end of name "Process ADC"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Get Functions
 * @brief			Use these functions to get values from the Wiper
 * @{
 ******************************************************************************/

/**
 * @brief			Call this function to get the current raw ADC value from the DMA
 * 						buffer.
 * @param			Wiper		pointer to the users wiper structure
 * @return		value of the last ADC measurement
 */
uint16_t Wiper_get_RawValue(Wiper_structTd* Wiper);

/**
 * @brief			Call this function to get the smoothed value of the ADC.
 * @param			Wiper		pointer to the users wiper structure
 * @return		value of the smoothed ADC measurement
 */
uint16_t Wiper_get_SmoothValue(Wiper_structTd* Wiper);

/** @} ************************************************************************/
/* end of name "Get Functions"
 ******************************************************************************/

/**@}*//* end of defgroup "Wiper_Header" */
/**@}*//* end of defgroup "WiperMotorFader" */
/**@}*//* end of defgroup "MotorFader" */

#endif /* INC_FADER_WHIPER_H_ */
