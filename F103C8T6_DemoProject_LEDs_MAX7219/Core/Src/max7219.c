/***************************************************************************//**
 * @defgroup        MAX7219_Source  Source
 * @brief           Study this part for a details about this module.
 *
 * @addtogroup      MAX7219
 * @{
 *
 * @addtogroup      MAX7219_Source
 * @{
 *
 * @file            max7219.c
 *
 * @date            Aug 11, 2023
 * @author          Mario Niehren
 ******************************************************************************/

#include <MAX7219.h>

/* Display Test Register [D0] */
typedef enum
{
  MAX7219_DISPLAY_TEST_NO_TEST = 0x00,
  MAX7219_DISPLAY_TEST_TEST_MODE = 0x01
}MAX7219_displayTestRegisterTypeDef;
/* Shutdown Register [D7...D0] */
typedef enum
{
  MAX7219_SHUTDOWN_ON = 0x00,
  MAX7219_SHUTDOWN_OFF = 0x01
}MAX7219_shutdownRegisterTypeDef;

/* Register Address [D11...D8].*/
typedef enum
{
  MAX7219_ADRRESS_NO_OP = 0x00,
  MAX7219_ADDRESS_DIGIT0 = 0x01,
  MAX7219_ADDRESS_DIGIT1 = 0x02,
  MAX7219_ADDRESS_DIGIT2 = 0x03,
  MAX7219_ADDRESS_DIGIT3 = 0x04,
  MAX7219_ADDRESS_DIGIT4 = 0x05,
  MAX7219_ADDRESS_DIGIT5 = 0x06,
  MAX7219_ADDRESS_DIGIT6 = 0x07,
  MAX7219_ADDRESS_DIGIT7 = 0x08,
  MAX7219_ADDRESS_DECODE_MODE = 0x09,
  MAX7219_ADDRESS_INTENSITY = 0x0A,
  MAX7219_ADDRESS_SCAN_LIMIT = 0x0B,
  MAX7219_ADDRESS_SHUTDOWN = 0x0C,
  MAX7219_ADDRESS_DISPLAY_TEST = 0x0F
}MAX7219_registerAddressTypeDef;

/* Decode Mode Register [D7...D0] */
typedef enum
{
  MAX7219_NO_DECODE = 0x00,
  MAX7219_CODE_B_DIGIT0 = (0x01 << 0),
  MAX7219_CODE_B_DIGIT1 = (0x01 << 1),
  MAX7219_CODE_B_DIGIT2 = (0x01 << 2),
  MAX7219_CODE_B_DIGIT3 = (0x01 << 3),
  MAX7219_CODE_B_DIGIT4 = (0x01 << 4),
  MAX7219_CODE_B_DIGIT5 = (0x01 << 5),
  MAX7219_CODE_B_DIGIT6 = (0x01 << 6),
  MAX7219_CODE_B_DIGIT7 = (0x01 << 7),
  MAX7219_CODE_B = 0xFF
}MAX7219_decodeModeRegisterTypeDef;

/* Scan-Limit Register [D7...D0] */
typedef enum
{
  MAX7219_DISPLAY_DIGIT0 = 0x00,
  MAX7219_DISPLAY_DITIG0TO1 = 0x01,
  MAX7219_DISPLAY_DITIG0TO2 = 0x02,
  MAX7219_DISPLAY_DITIG0TO3 = 0x03,
  MAX7219_DISPLAY_DITIG0TO4 = 0x04,
  MAX7219_DISPLAY_DITIG0TO5 = 0x05,
  MAX7219_DISPLAY_DITIG0TO6 = 0x06,
  MAX7219_DISPLAY_DITIG0TO7 = 0x07
}MAX7219_scanLimitRegisterTypeDef;

/***************************************************************************//**
 * @name      Helper Functions
 * @brief     these functions are used for internal module related tasks.
 * @{
 ******************************************************************************/
/**
 * @brief     SPI Send function
 * @note      In many commands, txMSB is the command address and txLSB is the
 *            command value.
 *            Last 16Bits of date are latched on rising edge of load pin. So it
 *            is not possible to send data without toggling load all 16 Bits.
 * @param     max7219   pointer to the users max7219 data structure
 * @param     txMSB     MSB to be send (or command address)
 * @param     txLSB     LSB to be send (or command data)
 * @return    none
 *
 */
void max7219_send16BitDataPoll(max7219_TypeDef* max7219, uint8_t txMSB, uint8_t txLSB)
{
  /** @internal     1. Activate CS-Pin */
  HAL_GPIO_WritePin(max7219->loadGPIO, max7219->loadPin, GPIO_PIN_RESET);
  /** @internal     2.  Transmit Data */
  uint32_t Timeout = 50;
  HAL_SPI_Transmit(max7219->hspi, &txMSB, 1, Timeout);
  HAL_SPI_Transmit(max7219->hspi, &txLSB, 1, Timeout);
  /** @internal     2. Deactivate CS-Pin*/
  HAL_GPIO_WritePin(max7219->loadGPIO, max7219->loadPin, GPIO_PIN_SET);
}

/** @} ************************************************************************/
/* end of name "Helper Functions"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize the LED Driver.
 * @{
 ******************************************************************************/

/* Description in .h */
void max7219_init_SPI(max7219_TypeDef* max7219, SPI_HandleTypeDef* hspi)
{
	max7219->hspi = hspi;
}

/* Description in .h */
void max7219_init_LoadPin(max7219_TypeDef* max7219, GPIO_TypeDef* GPIO, uint16_t Pin)
{
	max7219->loadGPIO = GPIO;
	max7219->loadPin = Pin;
}

/* Description in .h */
void max7219_init_NoDecode(max7219_TypeDef* max7219)
{
  max7219_send16BitDataPoll(max7219, MAX7219_ADDRESS_DECODE_MODE, MAX7219_NO_DECODE);
  max7219_send16BitDataPoll(max7219, MAX7219_ADDRESS_SCAN_LIMIT, MAX7219_DISPLAY_DITIG0TO7);
  max7219_send16BitDataPoll(max7219, MAX7219_ADDRESS_SHUTDOWN, MAX7219_SHUTDOWN_OFF);
  max7219_send16BitDataPoll(max7219, MAX7219_ADDRESS_DISPLAY_TEST, MAX7219_DISPLAY_TEST_NO_TEST);
  uint8_t Brightness = 3;
	max7219_dim_BrightnessAllLEDs(max7219, Brightness);
}

/* Description in .h */
void max7219_init_LED(max7219_LED_TypeDef* LED, uint8_t digit, uint8_t segment)
{
  /** @internal     1.  Link digit value to LED-structure. */
  uint8_t RegisterOffset = 1;
  LED->Digit = digit + RegisterOffset;
  /** @internal     2.  shift 1 left by the number of segment value to get the
   *                    register value for the LEDs segment. */
  LED->Segment = 0x01 << segment;
}
/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Control
 * @brief     Use these functions to control LEDs.
 * @{
 ******************************************************************************/

/* Description in .h */
void max7219_turnOn_LED(max7219_TypeDef* max7219, max7219_LED_TypeDef* led)
{
  /** @internal     1.  Set Segment Bit of LED to the Segment Register Array */
  uint8_t RegisterOffset = 1;
  uint8_t Index = led->Digit - RegisterOffset;
  max7219->SegmentRegisterState[(Index)] |= led->Segment;

  /** @internal     2.  Send updated Segment Register to the IC */
  uint8_t TxSegmentRegister = max7219->SegmentRegisterState[(Index)];
  uint8_t Digit = led->Digit;
  max7219_send16BitDataPoll(max7219, Digit, TxSegmentRegister);
}

/* Description in .h */
void max7219_turnOff_LED(max7219_TypeDef* max7219, max7219_LED_TypeDef* led)
{
  /** @internal     1.  Reset Segment Bit of LED to the Segment Register Array */
  uint8_t RegisterOffset = 1;
  uint8_t Index = led->Digit - RegisterOffset;
  max7219->SegmentRegisterState[Index] &= ~led->Segment;

  /** @internal     2.  Send updated Segment Register to the IC */
  uint8_t TxSegmentRegister = max7219->SegmentRegisterState[(Index)];
  uint8_t Digit = led->Digit;
  max7219_send16BitDataPoll(max7219, Digit, TxSegmentRegister);
}

/* Description in .h */
void max7219_write_LEDStatesOnDigit(max7219_TypeDef* max7219, uint8_t Digit, uint8_t Register)
{
  uint8_t RegisterOffset = 1;
  uint8_t DigitAddress = Digit + RegisterOffset;
  max7219_send16BitDataPoll(max7219, DigitAddress, Register);
}

/* Description in .h */
void max7219_turnOn_AllLEDs(max7219_TypeDef* max7219)
{
  /** @internal     Loop through all Digits and turn all segments on */
  for(MAX7219_registerAddressTypeDef digit = MAX7219_ADDRESS_DIGIT0; digit <= MAX7219_ADDRESS_DIGIT7; digit++)
  {
    for(uint8_t segment = 0; segment < 8; segment++)
    {
      max7219_LED_TypeDef led = {
          .Digit = digit,
          .Segment = (0x01 << segment)
      };

      max7219_turnOn_LED(max7219, &led);
    }
  }
}

/* Description in .h */
void max7219_turnOff_AllLEDs(max7219_TypeDef* max7219)
{
  /** @internal     Loop through all Digits and turn all segments off */
  for(MAX7219_registerAddressTypeDef digit = MAX7219_ADDRESS_DIGIT0; digit <= MAX7219_ADDRESS_DIGIT7; digit++)
  {
    for(uint8_t segment = 0; segment < 8; segment++)
    {
      max7219_LED_TypeDef led = {
                .Digit = digit,
                .Segment = (0x01 << segment)
      };

      max7219_turnOff_LED(max7219, &led);
    }
  }
}

/* Description in .h */
void max7219_dim_BrightnessAllLEDs(max7219_TypeDef* max7219, uint8_t Brightness)
{
  max7219_send16BitDataPoll(max7219, MAX7219_ADDRESS_INTENSITY, Brightness);
}
/** @} ************************************************************************/
/* end of name "Control"
 ******************************************************************************/


/**@}*//* end of defgroup "MAX7219_Source" */
/**@}*//* end of defgroup "MAX7219" */
