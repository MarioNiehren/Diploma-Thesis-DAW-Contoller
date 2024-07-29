/***************************************************************************//**
 * @defgroup        MAX7219   MAX7219 LED Driver
 * @brief           This module manages the MAX7219 LED driver to manage a big
 *                  amount of LEDs without much effort.
 *
 * # How to use:
 * 1. Initialize SPI, Load Pin and IC
 * 2. Initialize each LED used on IC
 * 3. Use Control functions to turn LEDs on an off or adjust settings of the IC
 *
 * @note            Multiple IC use is not supported by software yet! Max7219
 *                  supports cascading multiple ICs on one SPI Line.
 *
 * # Links:
 * @anchor          DataSheetMAX7219 [Data Sheet MAX7219](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX7219-MAX7221.pdf)
 *
 * @defgroup        MAX7219_Header  Header
 * @brief           Study this part for a quick overview of this module.
 *
 * @addtogroup      MAX7219
 * @{
 *
 * @addtogroup      MAX7219_Header
 * @{
 *
 * @file            max7219.h
 *
 * @date            Aug 11, 2023
 * @author          Mario Niehren
 ******************************************************************************/

#ifndef INC_MAX7219_H_
#define INC_MAX7219_H_

#define MAX7219_NUM_DIGITS  8

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "timer.h"

/***************************************************************************//**
 * @name      Structures and Enumerations
 * @brief     use these structure to declare on object for each used IC and
 *            for each LED.
 * @{
 ******************************************************************************/

/**
 * @brief     LED structure
 */
typedef struct {
  uint8_t Digit;
  uint8_t Segment;
}max7219_LED_TypeDef;

/**
 * @brief     max7219 data structure
 */
typedef struct{
  SPI_HandleTypeDef* hspi;    /**< pointer to HAL-SPI handle */
  uint16_t loadPin;           /**< load Pin (CS) */
  GPIO_TypeDef* loadGPIO;     /**< load port (CS)*/
  uint8_t SegmentRegisterState[MAX7219_NUM_DIGITS]; /**< Array of LED Register
                                   states for each digit */
}max7219_TypeDef;

/** @} ************************************************************************/
/* end of name "Structures and Enumerations"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize the LED Driver.
 * @{
 ******************************************************************************/
/**
 * @brief     Link HAL-SPI handle to the max7219 data structure
 * @param     max7219   pointer to the users max7219 data structure
 * @param     hspi      pointer to the HAL-SPI handle
 * @return    none
 */
void max7219_init_SPI(max7219_TypeDef* max7219, SPI_HandleTypeDef* hspi);

/**
 * @brief     Store Load Pin to the max7219 data structure
 * @param     max7219   pointer to the users max7219 data structure
 * @param     GPIO      Port used for the load pin
 * @param     Pin       number of the load pin
 * @return    none
 */
void max7219_init_LoadPin(max7219_TypeDef* max7219, GPIO_TypeDef* GPIO, uint16_t Pin);

/**
 * @brief     Initialize max7219 in no decode mode.
 * @note      The no decode mode is preferred to access each LED individually.
 *            Max7219 supports the use of "Code B" for the use with 8 segment
 *            digit displays but it will not be used here.
 * @param     max7219   pointer to the users max7219 data structure
 * @return    none
 */
void max7219_init_NoDecode(max7219_TypeDef* max7219);

/**
 * @brief     Initialize the coordinate of an LED. Use this function to setup
 *            each LED.
 * @param     LED     pointer to the LED to be initialized
 * @param     Digit   values from 0 to 7 are possible
 * @param     Segment values from 0 to 7 are possible. G = 0, F = 1, E = 2,
 *                    D = 3, C = 4, B = 5, A = 6, DP = 7
 * @return    none
 */
void max7219_init_LED(max7219_LED_TypeDef* LED, uint8_t Digit, uint8_t Segment);
/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Control
 * @brief     Use these functions to control LEDs.
 * @{
 ******************************************************************************/

/**
 * @brief     Turn on an LED on the IC
 * @param     max7219   pointer to the users max7219 data structure
 * @param     LED       pointer to the LED to turn on
 * @return    none
 */
void max7219_turnOn_LED(max7219_TypeDef* max7219, max7219_LED_TypeDef* led);

/**
 * @brief     Turn off an LED on the IC
 * @param     max7219   pointer to the users max7219 data structure
 * @param     LED       pointer to the LED to turn off
 * @return    none
 */
void max7219_turnOff_LED(max7219_TypeDef* max7219, max7219_LED_TypeDef* led);

/**
 * @brief     Write the register state of all segments on one digit.
 *            This can be helpful for quick and efficient LED updates.
 * @param     max7219   pointer to the users max7219 data structure
 * @param     Digit     number [0 - 7]
 * @param     Register  states of the LEDs of the digit.
 *                      | D7  | D6  | D6  | D4  | D3  | D2  | D1  | D0  |
 *                      | --- | --- | --- | --- | --- | --- | --- | --- |
 *                      | DP  | A   | B   | C   | D   | E   | F   | G   |
 * @return    none
 */
void max7219_write_LEDStatesOnDigit(max7219_TypeDef* max7219, uint8_t Digit, uint8_t Register);

/**
 * @brief     Turn all LEDs on
 * @param     max7219   pointer to the users max7219 data structure
 * return     none
 */
void max7219_turnOn_AllLEDs(max7219_TypeDef* max7219);

/**
 * @brief     Turn all LEDs off
 * @param     max7219   pointer to the users max7219 data structure
 * return     none
 */
void max7219_turnOff_AllLEDs(max7219_TypeDef* max7219);

/**
 * @brief     Control the brightness of the LEDs on the IC. The LEDs can only
 *            be dimmed all at once, not separate.
 * @param     max7219     pointer to the users max7219 data structure
 * @param     Brightness  of the IC. Possible values are from 0 - 15.
 * @return    none
 */
void max7219_dim_BrightnessAllLEDs(max7219_TypeDef* max7219, uint8_t Brightness);

/** @} ************************************************************************/
/* end of name "Control"
 ******************************************************************************/

/**@}*//* end of defgroup "MAX7219_Header" */
/**@}*//* end of defgroup "MAX7219" */

#endif /* INC_MAX7219_H_ */
