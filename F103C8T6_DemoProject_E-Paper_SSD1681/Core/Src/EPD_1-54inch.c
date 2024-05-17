/***************************************************************************//**
 * @defgroup			EPD_Source		Source Code
 * @brief					Study this part for a better understanding of the drivers
 * 								functionality.
 *
 * @addtogroup		EPaperSSD1681
 * @{
 *
 * @addtogroup		EPD_Source
 * @{
 *
 * @file			EPD_1-54inch.c
 *
 * @date			Oct 13, 2023
 * @author		Mario Niehren
 ******************************************************************************/


#include "EPD_1-54inch.h"

/***************************************************************************//**
 * @name			Waveform Setting
 * @brief			This array contains waveform settings for partial update.
 *
 * @note			This Table is essential for the partial update to work properly.
 * 						Adjustments of this table may be the key for better performance,
 * 						but that requires a very deep dive to the refresh process of EPDs.
 *
 * @warning		Only change values if absolutely necessary and if you know what
 * 						you are doing!
 * 						Look at @ref DataSheet "datasheet chapter 6.6 & 6.7" for details.

 * Default Values:
 *
 * | Voltage Settings | Frame 00 | Frame 01 | Frame 02 | Frame 03 | Frame 04 | Frame 05 | Frame 06 | Frame 07 | Frame 08 | Frame 09 | Frame 10 | Frame 11 |
 * | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
 * | LUT0 | 0x00 | 0x40 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 |
 * | LUT1 | 0x80 | 0x80 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 |
 * | LUT2 | 0x40 | 0x40 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 |
 * | LUT3 | 0x00 | 0x80 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 |
 * | LUT4 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 |
 *
 * TP ... time of phase length <br>
 * SR ... number of repeats (1 - 256) of a Sub Group (A&B or C&D) <br>
 * RP ... number of repeats (1 - 256) of the Group <br>
 *
 * | Frame | TP[A] | TP[B] | SR[AB] | TP[C] | TP[D] | SR[CD] | RP   |
 * | ----- | ----- | ----- | ------ | ----- | ----- | ------ | ---- |
 * | 0     | 0x0A  | 0x00  | 0x00   | 0x00  | 0x00  | 0x00   | 0x00 |
 * | 1     | 0x08  | 0x01  | 0x00   | 0x08  | 0x01  | 0x00   | 0x02 |
 * | 2     | 0x0A  | 0x00  | 0x00   | 0x00  | 0x00  | 0x00   | 0x00 |
 * | 3     | 0x00  | 0x00  | 0x00   | 0x00  | 0x00  | 0x00   | 0x00 |
 * | 4     | 0x00  | 0x00  | 0x00   | 0x00  | 0x00  | 0x00   | 0x00 |
 * | 5     | 0x00  | 0x00  | 0x00   | 0x00  | 0x00  | 0x00   | 0x00 |
 * | 6     | 0x00  | 0x00  | 0x00   | 0x00  | 0x00  | 0x00   | 0x00 |
 * | 7     | 0x00  | 0x00  | 0x00   | 0x00  | 0x00  | 0x00   | 0x00 |
 * | 8     | 0x00  | 0x00  | 0x00   | 0x00  | 0x00  | 0x00   | 0x00 |
 * | 9     | 0x00  | 0x00  | 0x00   | 0x00  | 0x00  | 0x00   | 0x00 |
 * | 10    | 0x00  | 0x00  | 0x00   | 0x00  | 0x00  | 0x00   | 0x00 |
 * | 11    | 0x00  | 0x00  | 0x00   | 0x00  | 0x00  | 0x00   | 0x00 |
 *
 * Fr ... Frame Rate (0 - 7) <br>
 *
 * | FR[0/1] | FR[2/3] | FR[4/5] | FR[6/7] | FR[8/9] | FR[10/11] |
 * | ------- | ------- | ------- | ------- | ------- | --------- |
 * | 0x22    | 0x22    | 0x22    | 0x22    | 0x22    | 0x22      |
 *
 * XON ...	Turn on all gates for a Sub Group (A&B or C&D).
 * 					Here: binary values! <br>
 *
 * | XON[0...3] | XON[4...7] | XON[8...11] |
 * | ---------- | ---------- | ----------- |
 * | 0x00       | 0x00       | 0x00        |
 *
 * EOPT ...	End Option: 0x02 POR, 0x22 Normal or 0x07 Source output level keep
 * 					previous output before power off <br>
 * VGH ...	Gate Signal: 10V to 20V <br>
 * Source / VBD driving output voltage: <br>
 * - VSH1/VSH2 ... 2.4 to 17V <br>
 * - VSL ... -5V to -17V <br>
 *
 * VCOM ... Output Voltage -3V to -0.2V <br>
 *
 * | EOPT | VGH  | VSH1 | VSH2 | VSL  | VCOM |
 * | ---- | ---  | ---- | ---- | ---  | ---- |
 * | 0x02  | 0x17 | 0x41 | 0xB0 | 0x32 | 0x28 |
 *
 * @{
 ******************************************************************************/

unsigned char WAVEFORM_PARTIAL[159] =
{
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* VS[LUT0] */
  0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* VS[LUT1] */
  0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* VS[LUT2] */
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* VS[LUT3] */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* VS[LUT4] */

  0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* TP[nA], TP[nB], SR[nAB], TP[nC], TP[nD], SR[nCD], RP[n] | n = 00 */
  0x08, 0x01, 0x00, 0x08, 0x01, 0x00, 0x02, /* TP[nA], TP[nB], SR[nAB], TP[nC], TP[nD], SR[nCD], RP[n] | n = 01 */
  0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* TP[nA], TP[nB], SR[nAB], TP[nC], TP[nD], SR[nCD], RP[n] | n = 02 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* TP[nA], TP[nB], SR[nAB], TP[nC], TP[nD], SR[nCD], RP[n] | n = 03 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* TP[nA], TP[nB], SR[nAB], TP[nC], TP[nD], SR[nCD], RP[n] | n = 04 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* TP[nA], TP[nB], SR[nAB], TP[nC], TP[nD], SR[nCD], RP[n] | n = 05 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* TP[nA], TP[nB], SR[nAB], TP[nC], TP[nD], SR[nCD], RP[n] | n = 06 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* TP[nA], TP[nB], SR[nAB], TP[nC], TP[nD], SR[nCD], RP[n] | n = 07 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* TP[nA], TP[nB], SR[nAB], TP[nC], TP[nD], SR[nCD], RP[n] | n = 08 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* TP[nA], TP[nB], SR[nAB], TP[nC], TP[nD], SR[nCD], RP[n] | n = 09 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* TP[nA], TP[nB], SR[nAB], TP[nC], TP[nD], SR[nCD], RP[n] | n = 10 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* TP[nA], TP[nB], SR[nAB], TP[nC], TP[nD], SR[nCD], RP[n] | n = 11 */

  0x22, 0x22, 0x22, 0x22, 0x22, 0x22,  /* FR[0/1], FR[.../...], FR[10/11] */

  0x00, 0x00, 0x00,  /* XON */

  0x02, 0x17, 0x41, 0xB0, 0x32, 0x28 /* EOPT, VGH, VSH1, VSH2, VSL, VCOM */
};

/** @} ************************************************************************/
/* end of name "Waveform Setting"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Initialize
 * @{
 ******************************************************************************/

void EPD_init_PinDC(EPD_StructTd* EPD, GPIO_TypeDef* GPIO_DC, uint16_t PIN_DC)
{
	/** @internal		1.	Set DC Pin for the SPI communication.
	 * 									\ref SPIDMA_init_PinDC() is used for this. Argument 3 of
	 * 									the function is GPIO_PIN_RESET as this is required by the
	 * 									EPD-driver to be in Command Mode (which is default in our
	 * 									case). */
  SPIDMA_init_PinDC(&EPD->SPI, GPIO_DC, PIN_DC, GPIO_PIN_RESET);
}

void EPD_init_PinCS(EPD_StructTd* EPD, GPIO_TypeDef* GPIO_CS, uint16_t PIN_CS)
{
	/** @internal		1. Set CS Pin for SPI communication. */
  SPIDMA_init_PinCS(&EPD->SPI, GPIO_CS, PIN_CS, GPIO_PIN_RESET);
}

void EPD_init_PinRST(EPD_StructTd* EPD, GPIO_TypeDef* GPIO_RST, uint16_t PIN_RST)
{
	/** @internal		1. Set RST Pin to EPD structure */
  EPD->pins.GPIO_RST = GPIO_RST;
  EPD->pins.PIN_RST = PIN_RST;

  /** @internal		2. Set RST high for normal operation. */
  HAL_GPIO_WritePin(EPD->pins.GPIO_RST, EPD->pins.PIN_RST, SET);
}

void EPD_init_PinBUSY(EPD_StructTd* EPD, GPIO_TypeDef* GPIO_BUSY, uint16_t PIN_BUSY)
{
	/** @internal		1. Set BSY Pin to EPD structure. */
  EPD->pins.GPIO_BUSY = GPIO_BUSY;
  EPD->pins.PIN_BUSY = PIN_BUSY;
}

void EPD_init_SPI(EPD_StructTd* EPD, SPI_HandleTypeDef* hspi)
{
	/** @internal		1. Link hspi to the \ref EPD_SPI_Helper "SPI-helper". */
  SPIDMA_init_SPIHandle(&EPD->SPI, hspi);
}

void EPD_init(EPD_StructTd* EPD)
{
	/** @internal		1. Init width and heigth of EPD for \ref GUI_DrawModule "GUI Module". */
  GUI_init_Size(&EPD->GUI, EPD_1IN54_WIDTH, EPD_1IN54_HEIGHT);
  /** @internal		2. Link Frame Buffer to \ref GUI_DrawModule "GUI Module". */
  GUI_init_Buffer(&EPD->GUI, EPD->BufferLocal.Frame);
	/** @internal		3. link Command Buffer to the \ref EPD_Buffer "Buffer helper". */
  Buffer_init_Command(&EPD->BufferManagement, EPD->BufferLocal.Command);
	/** @internal		4. link Data Buffer to the \ref EPD_Buffer "Buffer helper". */
  Buffer_init_Data(&EPD->BufferManagement, EPD->BufferLocal.Data);
	/** @internal		5. link Frame Buffer to the \ref EPD_Buffer "Buffer helper". */
  Buffer_init_Frame(&EPD->BufferManagement, EPD->BufferLocal.Frame);
	/** @internal		6. link LUT Buffer to the \ref EPD_Buffer "Buffer helper". */
  Buffer_init_LUT(&EPD->BufferManagement, EPD->BufferLocal.LUT);
	/** @internal		7. link Descriptor array to the \ref EPD_Buffer "Buffer helper". */
  Buffer_init_Desctriptor(&EPD->BufferManagement, EPD->BufferLocal.Descriptor, LEN_BUFFER_DESCRIPTOR);
  /** @internal		8. link GUI structure of EPD structure to \ref GUI_DrawModule "GUI Module"*/
  GUI_init(&EPD->GUI);
  /** @internal		9. Set start conditions of EPD Structure */
  EPD->check.Sleep = true;
  EPD->check.RefreshInitialized = false;
  EPD->check.RAM0x26EMPTY = true;
}

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Set Functions
 * @{
 ******************************************************************************/

void EPD_set_DrawColor(EPD_StructTd* EPD, EPD_Color_Td color)
{
  /**
   * @internal		1. Check Color
   * 									- if EPD_WHITE: set GUI_PIXEL_SET to GUI
   * 									- if EPD_BLACK: set GUI_PIXEL_RESET to GUI
   */
  if(color == EPD_COLOR_WHITE)
  {
    GUI_set_PixelStateToDraw(&EPD->GUI, GUI_PIXEL_SET);
  }
  else if(color == EPD_COLOR_BLACK)
  {
    GUI_set_PixelStateToDraw(&EPD->GUI, GUI_PIXEL_RESET);
  }
}

void EPD_set_FullRefreshMode(EPD_StructTd* EPD)
{
	/** @internal		1.	Set EPD Mode to EPD_MODE_FULL */
  EPD->Mode = EPD_MODE_FULL;
}

void EPD_set_PartRefreshMode(EPD_StructTd* EPD)
{
	/** @internal		1.	Set EPD Mode to EPD_MODE_PARTIAL */
  EPD->Mode = EPD_MODE_PARTIAL;
}

void EPD_set_BitmapInvert(EPD_StructTd* EPD)
{
	/** @internal		1.	For details look at GUI_set_Invert() */
  GUI_set_Invert(&EPD->GUI, GUI_INVERT);
}

void EPD_set_BitmapNoInvert(EPD_StructTd* EPD)
{
	/** @internal		1.	For details look at GUI_set_Invert() */
  GUI_set_Invert(&EPD->GUI, GUI_NO_INVERT);
}

void EPD_set_Font(EPD_StructTd* EPD, Fonts_TypeDef Font)
{
	/** @internal		1.	For details look at GUI_set_Font() */
  GUI_set_Font(&EPD->GUI, Font);
}

void EPD_set_PixelStartPosition(EPD_StructTd* EPD, uint16_t X, uint16_t Y)
{
	/** @internal		1.	For details look at GUI_set_StartPoint() */
	GUI_set_StartPoint(&EPD->GUI, X, Y);
}

/** @} ************************************************************************/
/* end of name "Set Functions"
 ******************************************************************************/


/***************************************************************************//**
 * @name			GUI Functions
 * @{
 ******************************************************************************/

void EPD_clear(EPD_StructTd* EPD)
{
	/** @internal		1.	For details look at GUI_clear() */
  GUI_clear(&EPD->GUI);
}

void EPD_draw_Rectangle(EPD_StructTd* EPD, uint8_t Width, uint8_t Height)
{
	/** @internal		1.	For details look at GUI_draw_Rectangle() */
  GUI_draw_Rectangle(&EPD->GUI, Width, Height);
}

void EPD_draw_Bitmap(EPD_StructTd* EPD, uint8_t* Bitmap, uint16_t Width, uint16_t Height)
{
	/** @internal		1.	For details look at GUI_draw_Bitmap() */
  GUI_draw_Bitmap(&EPD->GUI, Bitmap, Width, Height);
}

void EPD_write_String(EPD_StructTd* EPD, char* str, uint32_t length)
{
	/** @internal		1.	For details look at GUI_write_String() */
  GUI_write_String(&EPD->GUI, str, length);
}

void EPD_write_Number(EPD_StructTd* EPD, uint32_t Number)
{
	/** @internal		1.	For details look at GUI_write_Number() */
  GUI_write_Number(&EPD->GUI, Number);
}

/** @} ************************************************************************/
/* end of name "GUI Functions"
 ******************************************************************************/


/***************************************************************************//**
 * @name			EPD Command Helper Functions
 * @brief			these functions are used to send command sets as required by
 * 						@ref DataSheet "datasheet"
 * @note			Please do not add functions here, that do more than just buffering
 * 						command sets as described in @ref DataSheet "datasheet Table 7-1".
 * 						Keep it clean! Keep these functions in the order of the Table 7-1.
 * @{
 ******************************************************************************/

/**
 * @brief			[0x01]	Setup the gate driver operation mode.
 * @param			EPD			pointer to the users EPD structure
 * @param			A0to7		Data bits A0...A7 -> 0x00...0xFF
 * @param			A8			Data bit A8 -> 0x00...0x01
 * @param			B0to2		Data bits B0...B2 -> 0x00...0x07
 * @return		none
 */
void setup_DriverOutputControl(EPD_StructTd* EPD, uint8_t A0to7, uint8_t A8, uint8_t B0to2)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x01);
  Buffer_write_Data(&EPD->BufferManagement, A0to7);  /* POR: 0xC7 */
  Buffer_write_Data(&EPD->BufferManagement, A8);  /* POR: 0x00 */
  Buffer_write_Data(&EPD->BufferManagement, B0to2);    /* POR: 0x00 */
}

/**
 * @brief			[0x03]	Set Gate driving voltage VGH.
 * @param			EPD			pointer to the users EPD structure
 * @param			value		alias number from @ref DataSheet "datasheet Table 7-1" of
 * 						the voltage to be set
 * @return		none
 */
void set_GateDrivingVoltageControl(EPD_StructTd* EPD, uint8_t value)
{
	Buffer_write_Command(&EPD->BufferManagement, 0x03);
	Buffer_write_Data(&EPD->BufferManagement, value);
}

/**
 * @brief			[0x04]	Source driving voltage settings. Values to use for VSH1,
 * 						VSH2, VSL can be found in @ref DataSheet "datasheet Table 7-1".
 * @param			EPD			pointer to the users EPD structure
 * @param			VSH1		High voltage value 1 (POR: +15V)
 * @param			VSH2		High voltage value 2 (POR: +5V)
 * @param			VSL			Low voltage (POR: -5V)
 * @return		none
 */
void setup_SourceDrivingVoltageControl(EPD_StructTd* EPD, uint8_t VSH1, uint8_t VSH2, uint8_t VSL)
{
	Buffer_write_Command(&EPD->BufferManagement, 0x04);
	Buffer_write_Data(&EPD->BufferManagement, VSH1);
	Buffer_write_Data(&EPD->BufferManagement, VSH2);
	Buffer_write_Data(&EPD->BufferManagement, VSL);
}

/**
 * @brief			[0x10]	Enter deep sleep mode
 * @param			EPD			pointer to the users EPD structure
 * @param			type		of the deep sleep mode
 * 										(0x00: POR, 0x01: Mode 1, 0x02: Mode 2)
 * @return		none
 */
void activate_DeepSleepMode(EPD_StructTd* EPD, uint8_t type)
{
	Buffer_write_Command(&EPD->BufferManagement, 0x10);
	Buffer_write_Data(&EPD->BufferManagement, type);
}

/**
 * @brief			[0x11]	Setup the direction, how RAM data will be processed
 * @param			EPD			pointer to the users EPD structure
 * @param			Setting	of the data processing sequences
 * @return		none
 */
void setup_DataEntryMode(EPD_StructTd* EPD, uint8_t Setting)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x11);
  Buffer_write_Data(&EPD->BufferManagement, Setting);  /* POR: 0x03 */
}

/**
 * @brief			[0x12]	Perform a software reset -> BUSY will be high during
 * 										reset, RAM is not affected
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void reset_Software(EPD_StructTd* EPD)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x12);
}

/**
 * @brief			[0x18]	Setup if external or internal temperature should be used
 * @param			EPD			pointer to the users EPD structure
 * @param			Sensor	0x48 for external, 0x80 for internal sensor
 * @return		none
 */
void setup_TemperatureSensorControl(EPD_StructTd* EPD, uint8_t Sensor)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x18);
  Buffer_write_Data(&EPD->BufferManagement, Sensor);  /* POR: 0x48 */
}

/**
 * @brief			[0x1A]	Write value to the temperature register
 * @param			MSB	Bits A11...A4
 * @param			LSB	Bits A3...A0
 * @return		none
 */
void write_TemperatureRegister(EPD_StructTd* EPD, uint8_t MSB, uint8_t LSB)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x1A);
  Buffer_write_Data(&EPD->BufferManagement, MSB);  /* POR: 0x7F*/
  Buffer_write_Data(&EPD->BufferManagement, LSB);  /* POR: 0xF0*/
}

/**
 * @brief			[0x20]	Activate Master to start update sequence (BUSY Pin will
 * 										be high during the sequence)
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void activate_Master(EPD_StructTd* EPD)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x20);
}

/**
 * @brief			[0x22]	Setup the Sequence for Update Control 2
 * @note			This function is usually used together with activate_Master()
 * @param			EPD			pointer to the users EPD structure
 * @param			Sequence	number according to the description in the
 * 											@ref DataSheet "datasheet"
 * @return		none
 */
void setup_DisplayUpdateControl2(EPD_StructTd* EPD, uint8_t Sequence)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x22);
  Buffer_write_Data(&EPD->BufferManagement, Sequence);
}

/**
 * @brief			[0x24]	Write frame of EPD to RAM 0x24 Buffer for B/W mono color
											display. Look at
											@ref DataSheet "datasheet chapter 6.5 and Table 7-1" for
											details.
 * @param			EPD			pointer to the users EPD structure
 * @param			Data		pointer to the frame to be displayed
 * @param			Length 	of the Data
 * @return		none
 */
void write_BufferToRAM0x24(EPD_StructTd* EPD, uint8_t* Data, uint16_t Length)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x24);
  Buffer_write_Frame(&EPD->BufferManagement, Data, Length);
}

/**
 * @brief			[0x26]	Write frame of EPD to RAM 0x26 Buffer for red color
 * 										display
 * @param			EPD			pointer to the users EPD structure
 * @param			Data		pointer to the frame to be displayed
 * @param			Length 	of the Data
 * @return		none
 */
void write_BufferToRAM0x26(EPD_StructTd* EPD, uint8_t* Data, uint16_t Length)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x26);
  Buffer_write_Frame(&EPD->BufferManagement, Data, Length);
}

/**
 * @brief			[0x2C]	Write the VCOM value to register from the custom Waveform
 * @param			EPD			pointer to the users EPD structure
 * @param			value		alias number from @ref DataSheet "datasheet Table 7-1" of
 * 										the voltage to be set
 * @return		none
 */
void write_VCOMRegister(EPD_StructTd* EPD, uint8_t value)
{
	Buffer_write_Command(&EPD->BufferManagement, 0x2C);
	Buffer_write_Data(&EPD->BufferManagement, value);
}

/**
 * @brief			[0x32]	Write custom LUT to LUT register
 * @param			EPD			pointer to the users EPD structure
 * @param			lut			pointer to the custom LUT (as a part of the waveform
 * 										array)
 * @return		none
 */
void load_LUT(EPD_StructTd* EPD, uint8_t* lut)
{
  uint8_t tmp_TxLUT[153];
  Buffer_write_Command(&EPD->BufferManagement, 0x32);
  for(uint8_t i = 0; i < 153; i++)
  {
    tmp_TxLUT[i] = lut[i];
  }
  Buffer_write_LUT(&EPD->BufferManagement, tmp_TxLUT, 153);
}

/**
 * @brief			[0x3C]	Setup boarder waveform control. This will control the
 * 										VBD.
 * @todo 							find description of VBD
 * @param			EPD			pointer to the users EPD structure
 * @param			BorderWaveform	Value of the boarder waveform setting (Data)
 * @return		none
 */
void setup_BorderWaveformControl(EPD_StructTd* EPD, uint8_t BorderWaveform)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x3C);
  Buffer_write_Data(&EPD->BufferManagement, BorderWaveform);  /* POR: 0xC0 */
}

/**
 * @brief			[0x3F]	Set the behavior of the lut end before power off
 * @param			EPD			pointer to the users EPD structure
 * @param			data		End Option [0x02: POR | 0x22: normal | 0x07: keep
 * 										previous source output level before power off]
 * @return		none
 */
void set_EndOption(EPD_StructTd* EPD, uint8_t data)
{
	Buffer_write_Command(&EPD->BufferManagement, 0x3F);
	Buffer_write_Data(&EPD->BufferManagement, data);
}

/**
 * @brief			[0x44]	Set start and end Point of X in RAM
 * @param			EPD			pointer to the users EPD structure
 * @param			Start		point in X direction
 * @param			End			point in X direction
 * @return		none
 */
void setup_RamXAddressStartEnd(EPD_StructTd* EPD, uint8_t Start, uint8_t End)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x44);
  Buffer_write_Data(&EPD->BufferManagement, Start); /* POR: 0x00 */
  Buffer_write_Data(&EPD->BufferManagement, End);   /* POR: 0x15 */
}

/**
 * @brief			[0x45]	Set start and end Point of Y in RAM
 * @param			EPD			pointer to the users EPD structure
 * @param			StartLSB		LSB of start point in Y direction
 * @param			StartMSB		MSB of start point in Y direction
 * @param			EndLSB			LSB of end point in Y direction
 * @param			EndMSB			MSB of end point in Y direction
 * @return		none
 */
void setup_RamYAddressStartEnd(EPD_StructTd* EPD, uint8_t StartLSB, uint8_t StartMSB, uint8_t EndLSB, uint8_t EndMSB)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x45);
  Buffer_write_Data(&EPD->BufferManagement, StartLSB);  /* POR: 0x00 */
  Buffer_write_Data(&EPD->BufferManagement, StartMSB);  /* POR: 0x00 */
  Buffer_write_Data(&EPD->BufferManagement, EndLSB);    /* POR: 0x27 */
  Buffer_write_Data(&EPD->BufferManagement, EndMSB);    /* POR: 0x01 */
}

/**
 * @brief			[0x4E]	set start value for the X-Address counter
 * @param			EPD			pointer to the users EPD structure
 * @param			Value		of the start address
 * @retrun		none
 */
void setup_RamXAddressCounter(EPD_StructTd* EPD, uint8_t Value)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x4E);
  Buffer_write_Data(&EPD->BufferManagement, Value); /* POR: 0x00 */
}

/**
 * @brief			[0x4F]	set start value for the Y-Address counter
 * @param			EPD			pointer to the users EPD structure
 * @param			Value		of the start address
 * @return		none
 */
void setup_RamYAddressCounter(EPD_StructTd* EPD, uint8_t InitialSettingsLSB, uint8_t InitialSettingsMSB)
{
  Buffer_write_Command(&EPD->BufferManagement, 0x4F);
  Buffer_write_Data(&EPD->BufferManagement, InitialSettingsLSB);  /* POR: 0x00 */
  Buffer_write_Data(&EPD->BufferManagement, InitialSettingsMSB);  /* POR: 0x00 */
}

/** @} ************************************************************************/
/* end of name "EPD Command Helper Functions"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Transmission Functions
 * @todo			Redesign these functions to enable non blocking SPI data
 * 						transmission.
 * @{
 ******************************************************************************/

/** @cond *//* Function Prototypes */
void transmit_Command(EPD_StructTd* EPD);
void transmit_Data(EPD_StructTd* EPD);
void transmit_Frame(EPD_StructTd* EPD);
void transmit_LUT(EPD_StructTd* EPD);
void wait_WhileBusy(EPD_StructTd* EPD);
/** @endcond *//* Function Prototypes */

/**
 * @brief			Transmit buffered data with blocking SPI transmission
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void transmit_BlockingSPI(EPD_StructTd* EPD)
{
	/** @internal			1.	Check if while loop is still valid: enter loop if Buffer
	 * 										is not empty, otherwise leave function */
  while(Buffer_get_DescriptorTypeNow(&EPD->BufferManagement) != BUFFER_EMPTY)
  {
  	/** @internal			*Transmission scheme:*
  	 * 								-	wait as long as EPD is busy
  	 * 								- transmit SPI (Command, Data, Frame or LUT)
  	 * 								- wait until SPI transmission is complete	*/
    /** @internal			2.	if scheduled buffer element is a Command: process
     * 										*Transmission scheme* for command */
    if(Buffer_get_DescriptorTypeNow(&EPD->BufferManagement) == BUFFER_COMMAND)
    {
      wait_WhileBusy(EPD);
      transmit_Command(EPD);
      SPIDMA_wait_WhileTransmitting(&EPD->SPI);
    }
    /** @internal			3.	if scheduled buffer element is data: process
     * 										*Transmission scheme* for data */
    if(Buffer_get_DescriptorTypeNow(&EPD->BufferManagement) == BUFFER_DATA)
    {
      wait_WhileBusy(EPD);
      transmit_Data(EPD);
      SPIDMA_wait_WhileTransmitting(&EPD->SPI);
    }
    /** @internal			4.	if scheduled buffer element is a frame: process
     * 										*Transmission scheme* for frame */
    if(Buffer_get_DescriptorTypeNow(&EPD->BufferManagement) == BUFFER_FRAME)
    {
      wait_WhileBusy(EPD);
      transmit_Frame(EPD);
      SPIDMA_wait_WhileTransmitting(&EPD->SPI);
    }
    /** @internal			5.	if scheduled buffer element is a LUT: process
     * 										*Transmission scheme* for LUT */
    if(Buffer_get_DescriptorTypeNow(&EPD->BufferManagement) == BUFFER_LUT)
    {
      wait_WhileBusy(EPD);
      transmit_LUT(EPD);
      SPIDMA_wait_WhileTransmitting(&EPD->SPI);
    }
    /** @internal			2.	if nothing is buffered anymore: reset buffer
     * 										descriptor */
    if(Buffer_get_DescriptorTypeNow(&EPD->BufferManagement) == BUFFER_EMPTY)
    {
      Buffer_flush_Descriptor(&EPD->BufferManagement);
    }
  }
}

/**
 * @brief			Transmission scheme for commands
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void transmit_Command(EPD_StructTd* EPD)
{
  /** @internal			1.	Get start pointer of the buffered command */
  uint8_t* tmp_StartPointer = Buffer_get_StartPointer(&EPD->BufferManagement, EPD->BufferLocal.Command);
  /** @internal			2.	Get length of the buffered commmand */
  uint16_t tmp_Length = Buffer_get_DescriptorNumBytesNow(&EPD->BufferManagement);
  /** @internal			3.	Count up buffer descriptor to the next item */
  Buffer_countUp_SendDescriptorIndex(&EPD->BufferManagement);
  /** @internal			4.	Transmit Command. Look at SPIDMA_transmit_Command() for
   * 										details. */
  SPIDMA_transmit_Command(&EPD->SPI, tmp_StartPointer, tmp_Length);
}

/**
 * @brief			Transmission scheme for Data
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void transmit_Data(EPD_StructTd* EPD)
{
	/** @internal			1.	Get start pointer of the buffered data */
  uint8_t* tmp_StartPointer = Buffer_get_StartPointer(&EPD->BufferManagement, EPD->BufferLocal.Data);
  /** @internal			2.	Get length of the buffered data */
  uint16_t tmp_Length = Buffer_get_DescriptorNumBytesNow(&EPD->BufferManagement);
  /** @internal			3.	Count up buffer descriptor to the next item */
  Buffer_countUp_SendDescriptorIndex(&EPD->BufferManagement);
  /** @internal			4.	Transmit Data. Look at SPIDMA_transmit_Data() for
   * 										details. */
  SPIDMA_transmit_Data(&EPD->SPI, tmp_StartPointer, tmp_Length);

}

/**
 * @brief			Transmission scheme for Frame
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void transmit_Frame(EPD_StructTd* EPD)
{
	/** @internal			1.	Get start pointer of the buffered frame */
  uint8_t* tmp_StartPointer = Buffer_get_StartPointer(&EPD->BufferManagement, EPD->BufferLocal.Frame);
  /** @internal			2.	Get length of the buffered frame */
  uint16_t tmp_Length = Buffer_get_DescriptorNumBytesNow(&EPD->BufferManagement);
  /** @internal			3.	Count up buffer descriptor to the next item */
  Buffer_countUp_SendDescriptorIndex(&EPD->BufferManagement);
  /** @internal			4.	Transmit frame. Look at SPIDMA_transmit_Data() for
   * 										details. */
  SPIDMA_transmit_Data(&EPD->SPI, tmp_StartPointer, tmp_Length);
  /** @internal			5.	Wait 10ms to avoid corrupted pixels at the boarder.
   * 										@todo: find out why and how to avoid without delay */
  HAL_Delay(10);
}

/**
 * @brief			Transmission scheme for LUT
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void transmit_LUT(EPD_StructTd* EPD)
{
	/** @internal			1.	Get start pointer of the buffered LUT */
  uint8_t* tmp_StartPointer = Buffer_get_StartPointer(&EPD->BufferManagement, EPD->BufferLocal.LUT);
  /** @internal			2.	Get length of the buffered LUT */
  uint16_t tmp_Length = Buffer_get_DescriptorNumBytesNow(&EPD->BufferManagement);
  /** @internal			3.	Count up buffer descriptor to the next item */
  Buffer_countUp_SendDescriptorIndex(&EPD->BufferManagement);
  /** @internal			4.	Transmit LUT. Look at SPIDMA_transmit_Data() for
   * 										details. */
  SPIDMA_transmit_Data(&EPD->SPI, tmp_StartPointer, tmp_Length);
}

/** @cond *//* Function Prototypes */
bool check_Busy(EPD_StructTd* EPD);
/** @endcond *//* Function Prototypes */

/**
 * @brief			wait while BUSY Pin of EPD is high
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void wait_WhileBusy(EPD_StructTd* EPD)
{
	/** @internal			1.	stay in while loop as long as BUSY is high*/
  while(check_Busy(EPD))
  {
   ;
  }
}

/**
 * @brief			check if BUSY Pin of EPD is high
 * @param			EPD			pointer to the users EPD structure
 * @return		true		if EPD is busy, fals if not
 */
bool check_Busy(EPD_StructTd* EPD)
{
  GPIO_PinState tmp_PinState;
  /** @internal			1.	Read BUSY Pin*/
  tmp_PinState = HAL_GPIO_ReadPin(EPD->pins.GPIO_BUSY, EPD->pins.PIN_BUSY);
  /** @internal			2. 	If BUSY Pin is Set, return "true", else "false" */
  if(tmp_PinState == GPIO_PIN_SET)
  {
    EPD->check.DeviceBusy = true;
  }
  else if(tmp_PinState == GPIO_PIN_RESET)
  {
    EPD->check.DeviceBusy = false;
  }
  return EPD->check.DeviceBusy;
}

/** @} ************************************************************************/
/* end of name "Transmission Functions"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Display Frame
 * @{
 ******************************************************************************/

/** @cond *//* Function Prototypes */
void display_Full(EPD_StructTd* EPD);
void display_Part(EPD_StructTd* EPD);
void enter_SleepMode(EPD_StructTd* EPD);
/**@endcond *//* Function Prototypes */

/* Description in .h */
void EPD_display_Frame(EPD_StructTd* EPD)
{
	/** @internal			1.	Check Refresh mode.
	 * 										EDP_MODE_FULL ->	Display image with the full refresh
	 * 																			mode
	 * 										EPD_MODE_PART ->	Display image with the partial refresh
	 * 																			mode */
  if(EPD->Mode == EPD_MODE_FULL)
  {
    display_Full(EPD);
  }
  else if(EPD->Mode == EPD_MODE_PARTIAL)
  {
    display_Part(EPD);
  }

  /** @internal			2.	Set EPD to sleep mode */
  enter_SleepMode(EPD);

  /** @internal			3.	Transmit buffered SPI data */
  transmit_BlockingSPI(EPD);
}

/** @cond *//* Function Prototypes */
void setup_FullRefreshMode(EPD_StructTd* EPD);
void setup_PartialRefreshMode(EPD_StructTd* EPD);
/**@endcond *//* Function Prototypes */

/**
 * @brief			Sequence to display a frame in full refresh mode
 * @param			EPD			pointer to the users EPD structure
 * @return 		none
 */
void display_Full(EPD_StructTd* EPD)
{
	/** @internal			1.	Configure settings for full refresh */
	setup_FullRefreshMode(EPD);
	/** @internal			2.	Write buffered Frame to RAM 0x24 because we are in B/W
	 * 										mode. */
	write_BufferToRAM0x24(EPD, EPD->BufferManagement.Frame, (EPD->GUI.WidthBytes * EPD->GUI.Height));
	/** @internal			3.	Setup Display Update Control 2 with 0xF7 */
  setup_DisplayUpdateControl2(EPD, 0xF7);
  /** @internal			4.	Activate EPD-Master [Command 0x20] to start the
   * 										sequence */
  activate_Master(EPD);
}

/**
 * @brief			Sequence to display a frame in partial refresh mode
 * @param			EPD			pointer to the users EPD structure
 * @return 		none
 */
void display_Part(EPD_StructTd* EPD)
{
	/** @internal			1.	Configure settings for partial refresh */
	setup_PartialRefreshMode(EPD);
	/** @internal			2.	Write buffered Frame to RAM 0x24 because we are in B/W
	 * 										mode. */
	write_BufferToRAM0x24(EPD, EPD->BufferManagement.Frame, (EPD->GUI.WidthBytes * EPD->GUI.Height));
	/** @internal			3.	Setup Display Update Control 2 with 0xC7 */
  setup_DisplayUpdateControl2(EPD, 0xCF);
  /** @internal			4.	Activate EPD-Master [Command 0x20] to start the
   * 										sequence */
  activate_Master(EPD);
}

/** @cond *//* Function Prototypes */
void init_Display(EPD_StructTd* EPD);
void turnOn_Display(EPD_StructTd* EPD);
void init_Waveform(EPD_StructTd* EPD, uint8_t* lut);
/**@endcond *//* Function Prototypes */

/**
 * @brief			Initialize and setup EPD for full refresh
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void setup_FullRefreshMode(EPD_StructTd* EPD)
{
	/** @internal			1.	Initialize display */
  init_Display(EPD);
  /** @internal			2.	Turn on display */
  turnOn_Display(EPD);
}

/**
 * @brief			Initialize and setup EPD for partial refresh
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void setup_PartialRefreshMode(EPD_StructTd* EPD)
{
	/** @internal			1.	Initialize display */
  init_Display(EPD);
  /** @internal			2.	load the custom waveform for partial refresh */
  init_Waveform(EPD, WAVEFORM_PARTIAL);
  /** @internal			3.	Turn on display */
  turnOn_Display(EPD);
}

/** @cond *//* Function Prototypes */
void reset_Hardware(EPD_StructTd* EPD);
void setup_SpecificRamArea(EPD_StructTd* EPD, uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height);
/**@endcond *//* Function Prototypes */

/**
 * @brief			helper function to initialize the display at every refresh. This
 * 						is required because E-Paper Displays go to sleep and power off
 * 						when the frame is refreshed.
 * @todo			redesign this function to avoid blocking. Possible approach: use
 * 						a schedule concept and let an update function work through this
 * 						schedule.
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void init_Display(EPD_StructTd* EPD)
{
	/** @internal			1.	check if EPD is in sleep mode and wake-up with a
	 * 										hardware reset if true.*/
  if(EPD->check.Sleep)
  {
    reset_Hardware(EPD);
  }
  /** @internal			2.	reset software and wait 10 ms as required by
   * 										@ref DataSheet "datasheet chapter 8".*/
  reset_Software(EPD);
  HAL_Delay(10);
  /**@internal			3.	Set Driver Output Control with MUX Gate lines setting
   * 										0xC7, 0x00 (POR) and Gate scanning sequence and
   * 										direction 0x00 (POR)*/
  setup_DriverOutputControl(EPD, 0xC7, 0x00, 0x00);
  /** @internal			4.	Set Border Waveform Control to 0x05 */
  setup_BorderWaveformControl(EPD, 0x05);
  /** @internal			5.	Set Temperature Sensor Control 0x80 */
  setup_TemperatureSensorControl(EPD, 0x80);
  /** @internal			6.	Setup Ram Area for the complete frame (Start point 0/0,
   * 										size: 200x200)*/
  setup_SpecificRamArea(EPD, 0, 0, EPD->GUI.Width, EPD->GUI.Height);
  /** @internal			7.	Transmit buffered Commands and Data. */
  transmit_BlockingSPI(EPD);
}

/**
 * @brief			Reset Hardware with Reset-Pin
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void reset_Hardware(EPD_StructTd* EPD)
{
	/** @internal			1.	Set Reset Pin low and wait 10 ms*/
  HAL_GPIO_WritePin(EPD->pins.GPIO_RST, EPD->pins.PIN_RST, GPIO_PIN_RESET);
  HAL_Delay(10);
  /** @internal			2.	Set Reset Pin high and wait 10 ms*/
  HAL_GPIO_WritePin(EPD->pins.GPIO_RST, EPD->pins.PIN_RST, GPIO_PIN_SET);
  HAL_Delay(10);
  /** @internal			3.	Set EPD sleep flag to false */
  EPD->check.Sleep = false;
}

/**
 * @brief			Setup a specific RAM Area to store the frame data
 * @param			EPD			pointer to the users EPD structure
 * @param			X				coordinate for the start point
 * @param			Y				coordinate for the start point
 * @param			Width   of the frame area to be stored
 * @param			Height	of the frame area to be stored
 * @retrun 		none
 * @todo			add Error Code return to tell if the screen limit is exceeded
 */
void setup_SpecificRamArea(EPD_StructTd* EPD, uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height)
{
  /** @internal			1.	Setup sequence how data will be written to RAM (mode
   * 										0x03) */
  setup_DataEntryMode(EPD, 0x03);

  /** @internal			2.	Calculate Hex-Numbers for RAM register settings
   *  @todo					Move these calculations to the RAM Command functions */
  uint16_t tmp_XStart = X / 8;
  uint16_t tmp_XEnd = (X + Width - 1) / 8;
  uint16_t tmp_YStartLSB = Y % 256;
  uint16_t tmp_YStartMSB = Y / 256;
  uint16_t tmp_YEndLSB = (Y + Height - 1) % 256;
  uint16_t tmp_YEndMSB = (Y + Height - 1) / 256;

  /** @internal			3.	setup RAM */
  setup_RamXAddressStartEnd(EPD, tmp_XStart, tmp_XEnd);
  setup_RamYAddressStartEnd(EPD, tmp_YStartLSB, tmp_YStartMSB, tmp_YEndLSB, tmp_YEndMSB);
  setup_RamXAddressCounter(EPD, tmp_XStart);
  setup_RamYAddressCounter(EPD, tmp_YStartLSB, tmp_YStartMSB);
}

/**
 * @brief			Load the custom waveform to the SSD1681
 * @param			EPD			pointer to the users EPD structure
 * @param			lut			pointer to the custom waveform array
 * @return		none
 */
void init_Waveform(EPD_StructTd* EPD, uint8_t* lut)
{
	/** @internal			1.	Load LUT from the Wavefrom*/
  load_LUT(EPD, lut);
  transmit_BlockingSPI(EPD);
  /** @internal			2.	Set End Option from the Waveform*/
  set_EndOption(EPD, lut[153]);
  /** @internal			3.	Set gate driving voltage from the Waveform (VGH)*/
  set_GateDrivingVoltageControl(EPD, lut[154]);
  /** @internal			4.	Setup Source driving voltage settings from the Waveform
   * 										(VSH1, VSH2, VSL)*/
  setup_SourceDrivingVoltageControl(EPD, lut[155], lut[156], lut[157]);
  /** @internal			5.	Write the VCOM value to register from the Waveform*/
  write_VCOMRegister(EPD, lut[158]);

  transmit_BlockingSPI(EPD);
}

/**
 * @brief			Turn on display
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void turnOn_Display(EPD_StructTd* EPD)
{
	/** @internal			1.	Check if display is powered on
	 * 										-> if yes: leave function */
  if(EPD->check.PowerOn == false)
  {
  	/** @internal		2.	Setup Display Update Control 2 with 0xC7 */
    setup_DisplayUpdateControl2(EPD, 0xC0);
    /** @internal		3.	Activate EPD-Master [0x20] to start the sequence */
    activate_Master(EPD);
    /** @internal		4.  Save power and sleep state to EPD-structure */
    EPD->check.PowerOn = true;
    EPD->check.Sleep = false;
  }
}

/** @cond *//* Function Prototypes */
void turnOff_Display(EPD_StructTd* EPD);
/** @endcond *//* Function Prototypes */

/**
 * @brief			Set Display to Sleep Mode
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void enter_SleepMode(EPD_StructTd* EPD)
{
	/** @internal		1.	Process turn off sequence */
  turnOff_Display(EPD);
  /** @internal		2.	Set EPD to deep sleep mode */
  activate_DeepSleepMode(EPD, 0x01);

  EPD->check.Sleep = true;
}

/**
 * @brief			Turn off display
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void turnOff_Display(EPD_StructTd* EPD)
{
	/** @internal			1.	Check if display is powered on
	 * 										-> if not: leave function */
  if(EPD->check.PowerOn == true)
  {
    /** @internal		2.	Setup Display Update Control 2 with 0x83
     *  @note						0x83 is not mentioned in data sheet, but with other
     *  								settings, the program will get stuck. Not sure why,
     *  								it seems to be a problem with the BUSY Pin. */
    setup_DisplayUpdateControl2(EPD, 0x83);
    /** @internal		3.	Activate EPD-Master [0x20] to start the sequence */
    activate_Master(EPD);
  }
  /** @internal		4.  Set Power On flag to false */
  EPD->check.PowerOn = false;
}

/** @} ************************************************************************/
/* end of name "Display Frame"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Interrupt management
 * @{
 ******************************************************************************/

void EPD_manage_Interrupt(EPD_StructTd* EPD, SPI_HandleTypeDef* hspi)
{
	/** @internal		1.	For details look at SPIDMA_manage_Interrupt() */
  SPIDMA_manage_Interrupt(&EPD->SPI, hspi);
}

/** @} ************************************************************************/
/* end of name "Interrupt management"
 ******************************************************************************/

/**@}*//* end of defgroup "EPD_Source" */
/**@}*//* end of defgroup "EPaperSSD1681" */
