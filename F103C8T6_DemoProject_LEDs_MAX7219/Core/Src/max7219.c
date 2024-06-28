/*
 * max7219.c
 *
 *  Created on: Aug 11, 2023
 *      Author: Mario Niehren
 */

#include <MAX7219.h>

/*
* BEGIN: Function prototypes
*/
void max7219_send16BitDataInterrupt(max7219_TypeDef* max7219, uint8_t* txData);
void max7219_send16BitDataPull(max7219_TypeDef* max7219, uint8_t txMSB, uint8_t txLSB);
/*
* END: Function prototypes
*/

/*
 * BEGIN: Functions to initialize max7219.
 */
void max7219_init_SPI(max7219_TypeDef* max7219, SPI_HandleTypeDef* hspi)
{
	max7219->hspi = hspi;
}

void max7219_init_LoadPin(max7219_TypeDef* max7219, GPIO_TypeDef* loadGPIOx, uint16_t loadPin)
{
	max7219->loadGPIO = loadGPIOx;
	max7219->loadPin = loadPin;
}

void max7219_init_NoDecode(max7219_TypeDef* max7219)
{
	max7219_write_DecodeRegister(max7219, MAX7219_DECODE_MODE_REGISTER_NoDecode);
	max7219_write_ScanLimitRegister(max7219, MAX7219_SCANLIMIT_REGISTER_DisplayDigit0to7);
	max7219_write_ShutdownRegister(max7219, MAX7219_SHUTDOWN_REGISTER_normalOperation);
	max7219_write_DisplayTestRegister(max7219, MAX7219_DISPLAY_TEST_REGISTER_normalOperation);
	max7219_write_IntensityRegister(max7219, MAX7219_INTENSITY_REGISTER_3);	/* MAX7219_INTENSITY_REGISTER from 0 - 15 */
}
/*
 * END: Functions to initialize max7219.
 */

/*
 * BEGIN: Functions to set and reset LEDs on max7219.
 */
void max7219_set_Led(max7219_TypeDef* max7219, max7219led_TypeDef* led)
{
  max7219->segmentRegisterState[(led->digit - 1)] |= led->segment;
  max7219_send16BitDataPull(max7219, led->digit, max7219->segmentRegisterState[(led->digit - 1)]);
}

void max7219_reset_Led(max7219_TypeDef* max7219, max7219led_TypeDef* led)
{
  max7219->segmentRegisterState[(led->digit - 1)] &= ~led->segment;
  max7219_send16BitDataPull(max7219, led->digit, max7219->segmentRegisterState[(led->digit - 1)]);
}

void max7219_write_LED_register(max7219_TypeDef* max7219, uint8_t digit, uint8_t LEDreg)
{
  max7219_send16BitDataPull(max7219, digit, LEDreg);
}

void max7219_set_AllLeds(max7219_TypeDef* max7219)
{
  for(MAX7219_registerAddressTypeDef digit = MAX7219_REGISTER_ADDRESS_Digit0; digit <= MAX7219_REGISTER_ADDRESS_Digit7; digit++)
  {
    for(uint8_t segment = 0; segment < 8; segment++)
    {
      max7219led_TypeDef led = {
          .digit = digit,
          .segment = (0x01 << segment)
      };
      max7219_set_Led(max7219, &led);
    }
  }
}

void max7219_reset_AllLeds(max7219_TypeDef* max7219)
{
  for(MAX7219_registerAddressTypeDef digit = MAX7219_REGISTER_ADDRESS_Digit0; digit <= MAX7219_REGISTER_ADDRESS_Digit7; digit++)
  {
    for(uint8_t segment = 0; segment < 8; segment++)
    {
      max7219led_TypeDef led = {
                .digit = digit,
                .segment = (0x01 << segment)
      };
      max7219_reset_Led(max7219, &led);
    }
  }
}
/*
 * END: Functions to set and reset LEDs on max7219.
 */

/*
 * BEGIN: Functions to write max7219 registers.
 */
void max7219_write_DecodeRegister(max7219_TypeDef* max7219, MAX7219_decodeModeRegisterTypeDef decodeMode)
{
	max7219_send16BitDataPull(max7219, MAX7219_REGISTER_ADDRESS_DecodeMode, decodeMode);
}

void max7219_write_IntensityRegister(max7219_TypeDef* max7219, MAX7219_intensityRegisterTypedef intensity)
{
	max7219_send16BitDataPull(max7219, MAX7219_REGISTER_ADDRESS_Intensity, intensity);
}

void max7219_write_ScanLimitRegister(max7219_TypeDef* max7219, MAX7219_scanLimitRegisterTypeDef scanLimit)
{
	max7219_send16BitDataPull(max7219, MAX7219_REGISTER_ADDRESS_ScanLimit, scanLimit);
}

void max7219_write_ShutdownRegister(max7219_TypeDef* max7219, MAX7219_shutdownRegisterTypeDef shutdownMode)
{
	max7219_send16BitDataPull(max7219, MAX7219_REGISTER_ADDRESS_Shutdown, shutdownMode);
}

void max7219_write_DisplayTestRegister(max7219_TypeDef* max7219, MAX7219_displayTestRegisterTypeDef displayTestMode)
{
	max7219_send16BitDataPull(max7219, MAX7219_REGISTER_ADDRESS_DisplayTest, displayTestMode);
}
/*
 * END: Functions to write max7219 registers.
 */

/*
 * BEGIN: Functions to send data to max7219.
 */
void max7219_send16BitDataPull(max7219_TypeDef* max7219, uint8_t txMSB, uint8_t txLSB)
{
	HAL_GPIO_WritePin(max7219->loadGPIO, max7219->loadPin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(max7219->hspi, &txMSB, 1, 50);
	HAL_SPI_Transmit(max7219->hspi, &txLSB, 1, 50);
	HAL_GPIO_WritePin(max7219->loadGPIO, max7219->loadPin, GPIO_PIN_SET);
}
/*
 * END: Functions to send data to max7219.
 */
