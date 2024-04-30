/***************************************************************************//**
 * @defgroup			EPaperSSD1681	E-Paper
 * @brief					This module contains functions to use E-Paper-Displays with
 * 								SSD1681 driver in Black/White-Mode and SPI Interface.
 *
 * @anchor	EpdHowToUse
 * # How to use:
 *
 * 1. Initialize EPD as described here: \ref EPD_Initialize
 * 2.	Set conditions of the Frame that should be displayed with these functions:
 * 		\ref EPD_Set
 * 3. Draw/write to Frame buffer with these functions: \ref EPD_GUI
 * 4.	Refresh process:
 * 		1. 	Set Refresh mode (if not already set)
 * 		2. 	Call Display Frame function (This function is currently blocking and
 * 				requires some redesign to resolve timing issues of other program
 * 				components.)
 *
 * ## Required files
 * - EPD_1-54inch.h/.c
 * - Buffer_Displays.h/.c
 * - GUI.h/.c
 * - SPI_DMA.h/.c
 * - fonts.h
 * 		- SegoeScript31px.c @todo Clear how font can be used in open source!
 *
 * ## Links:
 * @anchor				DataSheet [SSD1681 data sheet](https://cdn-learn.adafruit.com/assets/assets/000/099/573/original/SSD1681.pdf)
 *
 * @defgroup			EPD_Header		Header File
 * @brief					Study this part of the documentation for a quick start
 *
 * @addtogroup		EPaperSSD1681
 * @{
 *
 * @addtogroup		EPD_Header
 * @{
 *
 * @file			EPD_1-54inch.h
 *
 * @date			Oct 13, 2023
 * @author		Mario Niehren
 ******************************************************************************/

/** @cond *//* Preprocessor directives */
#ifndef LIB_MN_PERIPHERALS_EPD_1_54INCH_H_
#define LIB_MN_PERIPHERALS_EPD_1_54INCH_H_

#include "stm32f1xx_hal.h"

#include "GUI.h"
#include "Buffer_Displays.h"
#include "SPI_DMA.h"
#include <stdbool.h>
#include <string.h>

#define EPD_1IN54_WIDTH 200
#define EPD_1IN54_HEIGHT 200

#define LEN_DATA_BUFFER 100
#define LEN_COMMAND_BUFFER 100
#define LEN_BUFFER_DESCRIPTOR 50
#define LEN_FRAMEBW_BUFFER ((EPD_1IN54_WIDTH / 8) * (EPD_1IN54_HEIGHT))
#define LEN_WAVEFORM_LUT 159
/** @endcond *//* Preprocessor directives */

/***************************************************************************//**
 * @name			Enumerations
 * @brief			These enumerations are provided to the user of this module to
 * 						achieve a clean and better readable code.
 * @{
 ******************************************************************************/

/**
 * @brief			Enumerations to describe the active EPD-Refresh mode
 */
typedef enum
{
	EPD_MODE_INVALID = 0x00,	/**<Invalid, try an other mode */
	EPD_MODE_PARTIAL = 0x01,	/**<Enum to set EPD to partial refresh mode */
	EPD_MODE_FULL = 0x02,			/**<Enum to set EPD to full refresh mode */
}EPD_Mode_Td;

/**
 * @brief			Enumerations to describe the pixel color
 */
typedef enum

{
	EPD_COLOR_INVALID = 0x00,	/**<Invalid Color*/
  EPD_COLOR_BLACK = 0x01,		/**<Black*/
  EPD_COLOR_WHITE = 0x02,		/**<White*/
}EPD_Color_Td;

/** @} ************************************************************************/
/* end of name "Enumerations"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Structures
 * @brief			This structure contains arrays to handle data that needs to be
 * 						buffered local.
 * @{
 ******************************************************************************/

/**
 * @brief			This structure stores EPD-Pins that are not part of the SPI
 * 						transmission. CS, DC, MOSI and Clock are stored at the SPI.
 */
typedef struct
{
  GPIO_TypeDef* GPIO_RST;
  uint16_t PIN_RST;
  GPIO_TypeDef* GPIO_BUSY;
  uint16_t PIN_BUSY;
}EPD_GPIOs_StructTd;

/**
 * @brief			This structure is a check-list for several states of the EPD
 */
typedef struct
{
  bool DeviceBusy;
  bool Sleep;
  bool PowerOn;
  bool RefreshInitialized;
  bool RAM0x26EMPTY;
}EPD_Check_StructTd;

/**
 * @brief			This structure contains arrays to buffer data for SPI
 */
typedef struct
{
  uint8_t Frame[LEN_FRAMEBW_BUFFER];
  uint8_t LUT[LEN_WAVEFORM_LUT];
  uint8_t Data[LEN_DATA_BUFFER];
  uint8_t Command[LEN_COMMAND_BUFFER];
  Buffer_Descriptor_TypeDef Descriptor[LEN_BUFFER_DESCRIPTOR];
}EPD_BufferLocal_StructTd;

/**
 * @brief			This is the Main Structure of the Display. The user has to declare
 * 						an object of this type for each used EPD module. All relevant data
 * 						is stored here.
 */
typedef struct
{
  EPD_GPIOs_StructTd pins;
  Buffer_TypeDef Buffer;
  EPD_BufferLocal_StructTd BufferLocal;
  EPD_Color_Td color;
  EPD_Check_StructTd check;
  EPD_Mode_Td Mode;
  GUI_TypeDef GUI;
  SPIDMA_TypeDef SPI;
}EPD_StructTd;

/** @} ************************************************************************/
/* end of name "Structures"
 ******************************************************************************/


/***************************************************************************//**
 * @defgroup			EPD_Initialize	Initialize
 * @brief					These functions are used to initialize the E-Paper Display. <br>
 * 								@ref EpdHowToUse "back to EPD main page"
 *
 * # How to use:
 * 1. Setup an Object of Type @ref EPD_StructTd
 * 2. Initialize Pins for DC, CS, RST, and BUSY
 * 3. Initialize SPI handler
 * 4. Initialize EPD	<br>
 *
 * @addtogroup		EPD_Initialize
 * @{
 ******************************************************************************/

/**
 * @brief			Initialize STM32 Pin that is used for Data/Command signal. It is
 * 						an Output GPIO that will be
 * 						- high to send data
 * 						- low to send commands
 * @param			EPD			pointer to the users EPD structure
 * @param			GPIO_DC	GPIO Port of the DC Pin
 * @param			PIN_DC	Pin of the DC Pin
 * @return		None
 */
void EPD_init_PinDC(EPD_StructTd* EPD, GPIO_TypeDef* GPIO_DC, uint16_t PIN_DC);

/**
 * @brief			Initialize STM32 Pin that is used for Chip Select signal. It is an
 * 						Output GPIO that will be:
 * 						- high if no SPI data should be received
 * 						- low to send SPI data to the Driver
 * @param			EPD			pointer to the users EPD structure
 * @param			GPIO_CS	GPIO Port of the CS Pin
 * @param			PIN_CS	Pin of the CS Pin
 * @return		None
 */
void EPD_init_PinCS(EPD_StructTd* EPD, GPIO_TypeDef* GPIO_CS, uint16_t PIN_CS);

/**
 * @brief			Initialize STM32 Pin that is used to reset EPD. It is an Output
 * 						GPIO that will be:
 * 						- high if EPD should not be reset and run normal
 * 						- low to reset EPD
 * 						- default: high
 * @param			EPD			pointer to the users EPD structure
 * @param			GPIO_RST	GPIO Port of the RST Pin
 * @param			PIN_RST	Pin of the RST Pin
 * @return		None
 */
void EPD_init_PinRST(EPD_StructTd* EPD, GPIO_TypeDef* GPIO_RST, uint16_t PIN_RST);

/**
 * @brief			Initialize STM32 Pin that is used to check if the EPD is busy. It
 * 						is an Input GPIO that will be:
 * 						- high if EPD is busy
 * 						- low if EPD is ready
 * @param			EPD			pointer to the users EPD structure
 * @param			GPIO_BUSY	GPIO Port of the BUSY Pin
 * @param			PIN_BUSY	Pin of the BUSY Pin
 * @return		None
 */
void EPD_init_PinBUSY(EPD_StructTd* EPD, GPIO_TypeDef* GPIO_BUSY, uint16_t PIN_BUSY);

/**
 * @brief			Link STM32 DMA-SPI HAL-handle. The SPI handle is generated by
 * 						Cube MX as a part of the STM32-HAL.
 * @param			EPD			pointer to the users EPD structure
 * @param			hspi		pointer to the HAL-SPI-Struct
 * @return		None
 */
void EPD_init_SPI(EPD_StructTd* EPD, SPI_HandleTypeDef* hspi);

/**
 * @brief			Initialize E-Paper. This will link parts of the EPD structure to
 * 						some helper structures and sets EPD-structure to proper start
 * 						values.
 * @param			EPD			pointer to the users EPD structure
 * @return		None
 */
void EPD_init(EPD_StructTd* EPD);

/** @} ************************************************************************/
/* end of defgroup "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @defgroup			EPD_Set	Set Functions
 * @brief					Functions to set settings that can be changed in running
 * 								program. <br>
 * 								@ref EpdHowToUse "back to EPD main page"
 *
 * # What to set up before displaying the frame:
 * ## Text:
 * 1. Set draw color to background color
 * 2. clear frame
 * 3. Set draw color for text, set font, set start point
 * 4. write text
 * 5. Set refresh mode (only if not set already or if it should be changed)
 * @note			This procedure may vary if the frame already contains image parts
 * 						that should also be used, or if something should be added.
 * ## Image:
 * 1. Invert bitmap if required
 * 2. draw Bitmap
 * 3. 5. Set refresh mode (only if not set already or if it should be changed)
 *
 * @addtogroup		EPD_Set
 * @{
 ******************************************************************************/

/**
 * @brief			Set color to draw. If the color is white, bits in the frame buffer
 * 						will be 1. If the color is black, bits will be 0.
 * @param			EPD			pointer to the users EPD structure
 * @param			color		to draw pixels
 * @return		none
 */
void EPD_set_DrawColor(EPD_StructTd* EPD, EPD_Color_Td color);

/**
 * @brief			Set Full Refresh mode. If this mode is set, all following frames
 * 						will be updated in full refresh mode.
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void EPD_set_FullRefreshMode(EPD_StructTd* EPD);

/**
 * @brief			Set partial refresh mode. If this mode is set, all following
 * 						frames will be updated in partial refresh mode.
 * @warning		If you only use partial refresh, it may lead to burned images and
 * 						gets worse in contrast over time. It is required to use Full
 * 						Refresh from time to time to avoid this problem. At the moment,
 * 						the complete frame will be updated. The Driver supports the update
 * 						of image parts, what should reduce these problems, but this is not
 * 						implemented yet. If you have problems with burned images, try to
 * 						refresh the screen in full mode for a few times with a complete
 * 						black or white frame. Depending on how intense the burned image
 * 						is, this may take a while.
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void EPD_set_PartRefreshMode(EPD_StructTd* EPD);

/**
 * @brief			Invert bitmap
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void EPD_set_BitmapInvert(EPD_StructTd* EPD);

/**
 * @brief			Reset inverted bitmap to normal
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void EPD_set_BitmapNoInvert(EPD_StructTd* EPD);

/**
 * @brief			Set Font that will be used
 * @note			check these requirements for the used font:
 * 						\ref GUI_FontRequirements "Font Requirements"
 * @param			EPD			pointer to the users EPD structure
 * @param			Font		Structure that describes the used font
 * @return		none
 */
void EPD_set_Font(EPD_StructTd* EPD, Fonts_TypeDef Font);

/**
 * @brief			Set start point for GUI functions to draw or write.
 * @param			EPD			pointer to the users EPD structure
 * @param			X				coordinate
 * @param			Y				coordinate
 * @return		none
 */
void EPD_set_PixelStartPosition(EPD_StructTd* EPD, uint16_t X, uint16_t Y);

/** @} ************************************************************************/
/* end of defgroup "Set Functions"
 ******************************************************************************/


/***************************************************************************//**
 * @defgroup			EPD_GUI	GUI specific functions
 * @brief					Use these functions to draw or write on Bitmap.
 * @addtogroup		EPD_GUI
 * @{
 ******************************************************************************/

/**
 * @brief			Clear all display pixels with currently set
 * 						@ref EPD_Color_TypeDef "draw color"
 *
 * @note			The display will be cleared in the currently set draw color.
 *
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void EPD_clear(EPD_StructTd* EPD);

/**
 * @brief			Draw a rectangle with currently set draw color.
 *
 * Use EPD_set_PixelStartPosition() to place the rectangle
 *
 * @param			EPD			pointer to the users EPD structure
 * @param			width		of the rectangle. Be careful to stay in Frame Size
 * @param			height	of the rectangle. Be careful to stay in Frame Size
 * @return		none
 */
void EPD_draw_Rectangle(EPD_StructTd* EPD, uint8_t width, uint8_t height);

/**
 * @brief			Draw a bitmap on the display (max 200x200)
 * @note			Use [LCD-Assistent](https://en.radzio.dxp.pl/bitmap_converter/)
 * 						to generate an usable array of a Binary Bitmap.
 * 						As this EPD module is white if a pixel-byte is 1, it may be
 * 						required to use EPD_set_BitmapInvert() to display the image as
 * 						expected.
 *
 * # How to generate an array for your Bitmap:
 *
 * 1.	use a graphics editor to generate a black and white bitmap (e.g. Photoshop
 * 		 or GIMP).
 * 		- import image
 * 		- adjust contrast / brightness / saturation, etc. to get a black and white
 * 		 	image (no grayscales!)
 * 		- scale to prefered size (max. 200x200)
 * 		- export to .bmp
 * 2.	Open the image in
 * 		[LCD-Assistent](https://en.radzio.dxp.pl/bitmap_converter/)
 * 3. Use these settings:
 * 		- Byte orientation: Horizontal
 * 		- Size: Size of the imported bitmap, max. 200x200 (other sizes not are
 * 			tested!)
 * 		- Other:
 * 			- Include size: no
 * 			- Size endianness: does not matter as we don`t include them
 * 			- Pixels/byte: 8
 * 		- Name: Name your bitmap here
 * 4.	File->Save output (save it as .txt)
 * 5.	Open saved file in any text editor
 * 6. Copy the array to your code and change data type to uint8_t.
 *
 * @param			EPD			pointer to the users EPD structure
 * @param			Bitmap  pointer to the bitmap-array
 * @param			Width		of the bitmap-array
 * @param			Heigth  of the bitmap-array
 * @return		none
 */
void EPD_draw_Bitmap(EPD_StructTd* EPD, uint8_t* Bitmap, uint16_t Width, uint16_t Height);

/**
 * @brief			Write text on EPD
 * @note			Make sure to set a font first and don't use the same color as the
 * 						background.
 *
 * @warning		This function supports a very basic and unfinished text flow
 * 						control. Try not to overflow the pixel width of the display, a it
 * 						may produce corrupted text. Use EPD_set_PixelStartPosition() to
 * 						place text manually.
 * 						# What should work at the moment:
 * 						- Fonts with variable width support auto formatting. If the page
 * 							is full, the rest of the text will be truncated
 * 						# What does not work:
 * 						- Fonts with fixed width
 * 						- Scrolling
 * 						- automatic frame around the text (the text will write till the
 * 							last pixel if necessary).
 *
 * @param			EPD			pointer to the users EPD structure
 * @param			width		of the rectangle. Be careful to stay in Frame Size
 * @param			height	of the rectangle. Be careful to stay in Frame Size
 * @return		none
 */
void EPD_write_String(EPD_StructTd* ssd1315, char* str, uint32_t length);

/**
 * @brief			Write a number on EPD
 * @note			Make sure to set a font first and don't use the same color as the
 * 						background.
 *
 * @param			EPD			pointer to the users EPD structure
 * @param			Number	to be printed.
 * @return		none
 */
void EPD_write_Number(EPD_StructTd* EPD, uint32_t Number);

/** @} ************************************************************************/
/* end of name "EPD_GUI"
 ******************************************************************************/


/***************************************************************************//**
 * @defgroup			EPD_Process	EPD processing functions
 * @brief					These functions are used to process the refresh of the EPD
 *
 * # How to use:
 * 1. call EPD_manage_Interrupt() in the SPI interrupt handler
 * 2. Set everything \ref EPD_Set	"Set Functions"
 * 3. prepare Frame to be displayed with \ref EPD_GUI	"GUI specific functions"
 * 4. call EPD_update_Transmission() to actually show the current frame.
 *
 * @warning				This part of the module is a snapshot of a work in progress
 * 								project. The SPI management takes interrupt handling already
 * 								in account, but the functions itself are still blocking! You
 * 								should not combine this code with any time critical processes.
 *
 * @addtogroup		EPD_Process
 * @{
 ******************************************************************************/

/**
 * @brief			Display the Frame with current settings. This function will go
 * 						through the complete update procedure to display the current
 * 						frame buffer. According to current refresh setting, the display
 * 						will perform a full or a partial refresh.
 *
 * @note			This is the part of code that has to be redesigned for time
 * 						critical use cases. The attempt should be to write a function
 * 						that is polled in while loop and skips if the EPD is still busy
 * 						or SPI transmission is not done yet.
 *
 * @warning		This function is blocking, as it waits for every SPI transmission
 * 						to be completed and calls HAL_Delay() in the reset routine.
 *
 * @param			EPD			pointer to the users EPD structure
 * @return		none
 */
void EPD_display_Frame(EPD_StructTd* EPD);

/**
 * @brief			SPI interrupt management
 *
 * This function has to be called in the interrupt handler. <br>
 * Here is an example:
 * @code
 * void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi)
 * {
 *   EPD_manage_Interrupt(&Display, hspi);
 * }
 * @endcode
 *
 * This function handles the CS and DC Pins when the data transfer is finished.
 *
 * @note			This function is part of the concept for future use, when all SPI
 * 						will be non blocking anymore. For now it is required for
 * 						EPD_display_Frame() otherwise it will not work.
 *
 * @param			EPD			pointer to the users EPD structure
 * @param			hspi		pointer to the handler of the interrupted SPI
 * @return		none
 */
void EPD_manage_Interrupt(EPD_StructTd* EPD, SPI_HandleTypeDef* hspi);

/** @} ************************************************************************/
/* end of name "EPD_Process"
 ******************************************************************************/

/**@}*//* end of defgroup "EPD_Header" */
/**@}*//* end of defgroup "EPaperSSD1681" */

#endif /* LIB_MN_PERIPHERALS_EPD_1_54INCH_H_ */
