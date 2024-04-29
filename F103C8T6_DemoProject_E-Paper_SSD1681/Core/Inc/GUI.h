/**
 * @defgroup		GUI_DrawModule	GUI module
 * @brief				Use this module to draw and write to a Frame Buffer to develop a GUI
 *
 * @addtogroup		EPD_Source
 * @{
 *
 * @addtogroup		GUI_DrawModule
 * @{
 *
 * @file		GUI.h
 *
 * @data		Oct 18, 2023
 * @author	Mario Niehren
 */

#ifndef MN_PERIPHERALS_DISPLAYS_GUI_H_
#define MN_PERIPHERALS__DISPLAYS_GUI_H_

#include <fonts.h>
#include <stdint.h>
#include <stdbool.h>

/**@cond*/
typedef enum
{
  GUI_PIXEL_SET,
  GUI_PIXEL_RESET
}GUI_PixelState_TypeDef;

typedef enum
{
  GUI_HORIZONTAL,
  GUI_VERTICAL
}GUI_Direction_TypeDef;

typedef enum
{
  GUI_INVERT,
  GUI_NO_INVERT,
}GUI_Invert_TypeDef;

typedef struct
{
  uint16_t X;
  uint16_t Y;
}GUI_Coordinate_TypeDef;

typedef struct
{
  uint8_t Width;
  uint8_t Length;
  GUI_Direction_TypeDef Direction;
}GUI_Line_TypeDef;
typedef struct
{
  uint16_t Width;
  uint16_t Height;
  uint16_t WidthBytes;
  uint8_t* Buffer;
  GUI_Invert_TypeDef Invert;
  GUI_PixelState_TypeDef PixelState;
  GUI_Line_TypeDef Line;
  GUI_Coordinate_TypeDef Start;
  GUI_Coordinate_TypeDef PositionNow;
  Fonts_TypeDef Font;
  bool PositionResetRequest;
}GUI_TypeDef;
/**@endcond*/
/***************************************************************************************************
 * BEGIN: Functions to initialize GUI.
 ***************************************************************************************************/
void GUI_init_Size(GUI_TypeDef* GUI, uint16_t Width, uint16_t Height);
void GUI_init_Buffer(GUI_TypeDef* GUI, uint8_t* Buffer);
void GUI_init(GUI_TypeDef* GUI);

/***************************************************************************************************
 * BEGIN: Functions to set conditions.
 ***************************************************************************************************/
void GUI_set_PixelStateToDraw(GUI_TypeDef* GUI, GUI_PixelState_TypeDef PixelState);
void GUI_set_LineWidth(GUI_TypeDef* GUI, uint8_t Width);
void GUI_set_LineDirection(GUI_TypeDef* GUI, GUI_Direction_TypeDef Direction);
void GUI_set_StartPoint(GUI_TypeDef* GUI, uint16_t X, uint16_t Y);
void GUI_set_Invert(GUI_TypeDef* GUI, GUI_Invert_TypeDef Invert);

/**
 * @name		Set Font
 * @anchor  GUI_FontRequirements
 * Requirements for fonts used in this function:
 *
 *    Size support:
 *    - Width: up to 24px
 *
 *    Padding Removal:
 *    - Height: fixed
 *    - Width: tightest (can be fixed, but descriptor is still required)
 *
 *    Byte Layout:
 *    - RowMajor
 *    - MsbFirst
 *
 *    Descriptor:
 *    - Char width in bits
 *
 *    Space char generation:
 *    - check the box to generate s space bitmap at the beginning of the complete bitmap.
 *    - space width is free
 *
 * Tool for bitmap generation: [The Dot Factory] http://www.eran.io/the-dot-factory-an-lcd-font-and-image-generator/
 *
 * Sign Set ASCII 32 - 126:
 *
 *  !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
 *
 */
void GUI_set_Font(GUI_TypeDef* GUI, Fonts_TypeDef Font);

/***************************************************************************************************
 * BEGIN: Functions write.
 ***************************************************************************************************/
void GUI_write_Character(GUI_TypeDef* GUI, char chr);
void GUI_write_String(GUI_TypeDef* GUI, char* str, uint32_t Length);
void GUI_write_Number(GUI_TypeDef* GUI, uint32_t Number);

/***************************************************************************************************
 * BEGIN: Functions to show image or clean screen.
 ***************************************************************************************************/
void GUI_draw_Bitmap(GUI_TypeDef* GUI, uint8_t* Bitmap, uint16_t Width, uint16_t Height);
/**
 * @brief			clear GUI with active pixel state.
 *
 * @param			GUI		pointer to the users GUI.
 * @return		none
 */
void GUI_clear(GUI_TypeDef* GUI);

/***************************************************************************************************
 * BEGIN: Functions to design GUI.
 ***************************************************************************************************/
void GUI_draw_Pixel(GUI_TypeDef* GUI, uint16_t X, uint16_t Y);
void GUI_draw_Line(GUI_TypeDef* GUI,  uint16_t X, uint16_t Y, uint16_t Length);
void GUI_draw_Rectangle(GUI_TypeDef* GUI, uint8_t Width, uint8_t Height);

/***************************************************************************************************
 * BEGIN: Functions get information about GUI.
 ***************************************************************************************************/
uint16_t GUI_get_StartPositionX(GUI_TypeDef* GUI);
uint16_t GUI_get_StartPositionY(GUI_TypeDef* GUI);

/***************************************************************************************************
 * BEGIN: Functions Limit Coordinates and Sizes to Frame Size.
 ***************************************************************************************************/
uint16_t GUI_limit_XOnFrameBorders(GUI_TypeDef* GUI, uint16_t X);
uint16_t GUI_limit_YOnFrameBorders(GUI_TypeDef* GUI, uint16_t Y);
uint16_t GUI_limit_WidthOnFrameBorders(GUI_TypeDef* GUI, uint16_t X, uint16_t Width);
uint16_t GUI_limit_HeightOnFrameBorders(GUI_TypeDef* GUI, uint16_t Y, uint16_t Height);
uint16_t GUI_get_WidthAsMultipleOf8(uint16_t X, uint16_t Width);
uint16_t GUI_get_XAsMultipleOf8(uint16_t X);

/**@}*//* end of defgroup "GUI_DrawModule" */
/**@}*//* end of defgroup "EPaperSSD1681" */
#endif /* PERIPHERALS_GUI_H_ */
