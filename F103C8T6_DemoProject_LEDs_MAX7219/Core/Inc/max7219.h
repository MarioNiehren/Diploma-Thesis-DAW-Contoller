/***************************************************************************//**
 * @defgroup        MAX7219   MAX7219 LED Driver
 * @brief           This module manages the MAX7219 LED driver to manage a big
 *                  amount of LEDs without much effort.
 *
 * # How to use:
 * @todo
 *
 * # Hardware requirements:
 * @todo
 *
 * # Links:
 * @anchor          DataSheetMAX7219 [Data Sheet MAX7219](@todoaddLinkHere)
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


#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "timer.h"

/*
 * BEGIN: Typedefs for Registers.
 */

/* Register Address [D11...D8].*/
typedef enum
{
	MAX7219_REGISTER_ADRRESS_NoOp = 0x00,
	MAX7219_REGISTER_ADDRESS_Digit0 = 0x01,
	MAX7219_REGISTER_ADDRESS_Digit1 = 0x02,
	MAX7219_REGISTER_ADDRESS_Digit2 = 0x03,
	MAX7219_REGISTER_ADDRESS_Digit3 = 0x04,
	MAX7219_REGISTER_ADDRESS_Digit4 = 0x05,
	MAX7219_REGISTER_ADDRESS_Digit5 = 0x06,
	MAX7219_REGISTER_ADDRESS_Digit6 = 0x07,
	MAX7219_REGISTER_ADDRESS_Digit7 = 0x08,
	MAX7219_REGISTER_ADDRESS_DecodeMode = 0x09,
	MAX7219_REGISTER_ADDRESS_Intensity = 0x0A,
	MAX7219_REGISTER_ADDRESS_ScanLimit = 0x0B,
	MAX7219_REGISTER_ADDRESS_Shutdown = 0x0C,
	MAX7219_REGISTER_ADDRESS_DisplayTest = 0x0F
}MAX7219_registerAddressTypeDef;

/* Shutdown Register [D7...D0] */
typedef enum
{
	MAX7219_SHUTDOWN_REGISTER_shutdownMode = 0x00,
	MAX7219_SHUTDOWN_REGISTER_normalOperation = 0x01
}MAX7219_shutdownRegisterTypeDef;

/* Decode Mode Register [D7...D0] */
typedef enum
{
	MAX7219_DECODE_MODE_REGISTER_NoDecode = 0x00,
	MAX7219_DECODE_MODE_REGISTER_CodeBDigit0 = (0x01 << 0),
	MAX7219_DECODE_MODE_REGISTER_CodeBDigit1 = (0x01 << 1),
	MAX7219_DECODE_MODE_REGISTER_CodeBDigit2 = (0x01 << 2),
	MAX7219_DECODE_MODE_REGISTER_CodeBDigit3 = (0x01 << 3),
	MAX7219_DECODE_MODE_REGISTER_CodeBDigit4 = (0x01 << 4),
	MAX7219_DECODE_MODE_REGISTER_CodeBDigit5 = (0x01 << 5),
	MAX7219_DECODE_MODE_REGISTER_CodeBDigit6 = (0x01 << 6),
	MAX7219_DECODE_MODE_REGISTER_CodeBDigit7 = (0x01 << 7),
	MAX7219_DECODE_MODE_REGISTER_CodeB = 0xFF
}MAX7219_decodeModeRegisterTypeDef;

/* Code B Font [D3...D0]. SEG DP [D7] is not affected by code B*/
typedef enum
{
	MAX7219_CODE_B_FONT_REGISTER_0 = 0x00,
	MAX7219_CODE_B_FONT_REGISTER_1 = 0x01,
	MAX7219_CODE_B_FONT_REGISTER_2 = 0x02,
	MAX7219_CODE_B_FONT_REGISTER_3 = 0x03,
	MAX7219_CODE_B_FONT_REGISTER_4 = 0x04,
	MAX7219_CODE_B_FONT_REGISTER_5 = 0x05,
	MAX7219_CODE_B_FONT_REGISTER_6 = 0x06,
	MAX7219_CODE_B_FONT_REGISTER_7 = 0x07,
	MAX7219_CODE_B_FONT_REGISTER_8 = 0x08,
	MAX7219_CODE_B_FONT_REGISTER_9 = 0x09,
	MAX7219_CODE_B_FONT_REGISTER_hyphen = 0x0A,
	MAX7219_CODE_B_FONT_REGISTER_E = 0x0B,
	MAX7219_CODE_B_FONT_REGISTER_H = 0x0C,
	MAX7219_CODE_B_FONT_REGISTER_L = 0x0D,
	MAX7219_CODE_B_FONT_REGISTER_P = 0x0E,
	MAX7219_CODE_B_FONT_REGISTER_blank = 0x0F
}MAX7219_codeBFontRegisterTypeDef;

/* No Decode Data Bits [D7...D0]*/
typedef enum
{
	MAX7219_NO_DECODE_REGISTER_G = (0x01 << 0),
	MAX7219_NO_DECODE_REGISTER_F = (0x01 << 1),
	MAX7219_NO_DECODE_REGISTER_E = (0x01 << 2),
	MAX7219_NO_DECODE_REGISTER_D = (0x01 << 3),
	MAX7219_NO_DECODE_REGISTER_C = (0x01 << 4),
	MAX7219_NO_DECODE_REGISTER_B = (0x01 << 5),
	MAX7219_NO_DECODE_REGISTER_A = (0x01 << 6),
	MAX7219_NO_DECODE_REGISTER_DP = (0x01 << 7)
}MAX7219_noDecodeRegisterTypeDef;

/* Scan-Limit Register [D7...D0] */
typedef enum
{
	MAX7219_SCANLIMIT_REGISTER_DisplayDigit0 = 0x00,
	MAX7219_SCANLIMIT_REGISTER_DisplayDigit0to1 = 0x01,
	MAX7219_SCANLIMIT_REGISTER_DisplayDigit0to2 = 0x02,
	MAX7219_SCANLIMIT_REGISTER_DisplayDigit0to3 = 0x03,
	MAX7219_SCANLIMIT_REGISTER_DisplayDigit0to4 = 0x04,
	MAX7219_SCANLIMIT_REGISTER_DisplayDigit0to5 = 0x05,
	MAX7219_SCANLIMIT_REGISTER_DisplayDigit0to6 = 0x06,
	MAX7219_SCANLIMIT_REGISTER_DisplayDigit0to7 = 0x07
}MAX7219_scanLimitRegisterTypeDef;

/* Display Test Register [D0] */
typedef enum
{
	MAX7219_DISPLAY_TEST_REGISTER_normalOperation = 0x00,
	MAX7219_DISPLAY_TEST_REGISTER_DisplayTestMode = 0x01
}MAX7219_displayTestRegisterTypeDef;

/* Intensity Register [D3...D0]*/
typedef enum
{
	MAX7219_INTENSITY_REGISTER_0 = 0x00,
	MAX7219_INTENSITY_REGISTER_1 = 0x01,
	MAX7219_INTENSITY_REGISTER_2 = 0x02,
	MAX7219_INTENSITY_REGISTER_3 = 0x03,
	MAX7219_INTENSITY_REGISTER_4 = 0x04,
	MAX7219_INTENSITY_REGISTER_5 = 0x05,
	MAX7219_INTENSITY_REGISTER_6 = 0x06,
	MAX7219_INTENSITY_REGISTER_7 = 0x07,
	MAX7219_INTENSITY_REGISTER_8 = 0x08,
	MAX7219_INTENSITY_REGISTER_9 = 0x09,
	MAX7219_INTENSITY_REGISTER_10 = 0x0A,
	MAX7219_INTENSITY_REGISTER_11 = 0x0B,
	MAX7219_INTENSITY_REGISTER_12 = 0x0C,
	MAX7219_INTENSITY_REGISTER_13 = 0x0D,
	MAX7219_INTENSITY_REGISTER_14 = 0x0E,
	MAX7219_INTENSITY_REGISTER_15 = 0x0F

}MAX7219_intensityRegisterTypedef;
/*
 * END: Typedefs for Registers.
 */


typedef struct {
  MAX7219_registerAddressTypeDef digit;
  MAX7219_noDecodeRegisterTypeDef segment;
}max7219led_TypeDef;

typedef struct{
  max7219led_TypeDef led;
  SPI_HandleTypeDef* hspi;
  uint16_t loadPin;
  GPIO_TypeDef* loadGPIO;
  bool forceOn;
  bool forceOff;
  uint8_t segmentRegisterState[8];
}max7219_TypeDef;

/*
 * BEGIN: Functions to initialize max7219.
 */
void max7219_init_SPI(max7219_TypeDef* max7219, SPI_HandleTypeDef* hspi);
void max7219_init_LoadPin(max7219_TypeDef* max7219, GPIO_TypeDef* loadGPIOx, uint16_t loadPin);
void max7219_init_NoDecode(max7219_TypeDef* max7219);
/*
 * END: Functions to initialize max7219.
 */

/*
 * BEGIN: Functions to set and reset LEDs on max7219.
 */
void max7219_set_Led(max7219_TypeDef* max7219, max7219led_TypeDef* led);
void max7219_reset_Led(max7219_TypeDef* max7219, max7219led_TypeDef* led);

void max7219_write_LED_register(max7219_TypeDef* max7219, uint8_t digit, uint8_t LEDreg);

void max7219_set_AllLeds(max7219_TypeDef* max7219);
void max7219_reset_AllLeds(max7219_TypeDef* max7219);
/*
 * END: Functions to set and reset LEDs on max7219.
 */

/*
 * BEGIN: Functions to write max7219 registers.
 */
void max7219_write_DecodeRegister(max7219_TypeDef* max7219, MAX7219_decodeModeRegisterTypeDef decodeMode);
void max7219_write_IntensityRegister(max7219_TypeDef* max7219, MAX7219_intensityRegisterTypedef intensity);
void max7219_write_ScanLimitRegister(max7219_TypeDef* max7219, MAX7219_scanLimitRegisterTypeDef scanLimit);
void max7219_write_ShutdownRegister(max7219_TypeDef* max7219, MAX7219_shutdownRegisterTypeDef shutdownMode);
void max7219_write_DisplayTestRegister(max7219_TypeDef* max7219, MAX7219_displayTestRegisterTypeDef displayTestMode);
/*
 * END: Functions to write max7219 registers.
 */

/**@}*//* end of defgroup "MAX7219_Header" */
/**@}*//* end of defgroup "MAX7219" */

#endif /* INC_MAX7219_H_ */
