/***************************************************************************//**
 * @defgroup				WiperMotorFader	Wiper module
 * @brief						This module is responsible to handle the wiper of the
 * 									motorized Fader. It will read the ADC, smooth the values and
 * 									returns the results.
 *
 * # How to use:
 *  1.  Initialize @ref Initialize "ADC and hysteresis"
 *  2.  Setup interrupt function
 *  3.  Start wiper
 *  4.  Update wiper regularly in While loop.
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

#include "adc.h"
#include <stdbool.h>

/**
 * @brief			Define for the number of sampled values. If the smooth value
 * 						is not clean enough, increase this number. Be careful that the
 * 						ADC does not get to slow by using to many samples.
 */
#define NUMBER_OF_SAMPLES 100

/***************************************************************************//**
 * @name			Structures
 * @brief			This structure is used to store all information about the used
 * 						wiper.
 * @{
 ******************************************************************************/

typedef struct Wiper_structTd
{
  ADC_HandleTypeDef* handle;						/**<	Handle to the ADC Setup of HAL */
  uint16_t  ValueSmooth;											/**<	Current smoothed ADC value */
  uint32_t	BufferDMA;									/**<	Buffer for the DMA to store ADC
																							data */
  uint16_t  Samples[NUMBER_OF_SAMPLES];	/**<	Array of measured ADC values.*/
  uint8_t   SamplesIndex;								/**<	Counter for measured ADC values.*/
  bool			Interrupted;								/**<	Check if ADC Callback was called,
																							so the module knows that a new
																							value is available.*/
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
 *
 *
 * @param			Wiper			pointer to the users wiper structure
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
void Wiper_start(Wiper_structTd* Wiper);

/**
 * @brief			Call this function periodically in while function to check the
 *						wiper if new ADC values are available and to smooth them.
 * @param			Wiper		pointer to the users wiper structure
 * @return		none
 */
void Wiper_update(Wiper_structTd* Wiper);

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
 * @param			Wiper		pointer to the users wiper structure
 * @param		  hadc			pointer to the HAL generated ADC-handle of the ADC
 * 											that is used for the wiper
 * @return		none
 */
void Wiper_manage_Interrupt(Wiper_structTd* Wiper, ADC_HandleTypeDef* hadc);

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
