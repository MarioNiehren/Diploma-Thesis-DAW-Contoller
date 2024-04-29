/*
 * ssd1315.h
 *
 *  Created on: Sep 28, 2023
 *      Author: Mario Niehren
 */

/*
 * SSD1315 Library inspired by STM Library for ssd1315
 */

#ifndef PERIPHERALS_SSD1315_H_
#define PERIPHERALS_SSD1315_H_

#include <Fonts/Fonts.h>
#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <string.h>
/*
 * BEGIN: defines for SSC1315 Registers
 */
#define  SSD1315_LCD_PIXEL_WIDTH    ((uint16_t)128)
#define  SSD1315_LCD_PIXEL_HEIGHT   ((uint16_t)64)

#define  SSD1315_LCD_COLUMN_NUMBER  ((uint16_t)128)
#define  SSD1315_LCD_PAGE_NUMBER    ((uint16_t)8)

#define  SSD1315_COLOR_WHITE  0xFF
#define  SSD1315_COLOR_BLACK  0x00

#define  SSD1315_SCROLL_RIGHT 0x26
#define  SSD1315_SCROLL_LEFT  0x27

#define  SSD1315_SCROLL_FREQ_2FRAMES    0x07
#define  SSD1315_SCROLL_FREQ_3FRAMES    0x04
#define  SSD1315_SCROLL_FREQ_4FRAMES    0x05
#define  SSD1315_SCROLL_FREQ_5FRAMES    0x00
#define  SSD1315_SCROLL_FREQ_25FRAMES   0x06
#define  SSD1315_SCROLL_FREQ_64FRAMES   0x01
#define  SSD1315_SCROLL_FREQ_128FRAMES  0x02
#define  SSD1315_SCROLL_FREQ_256FRAMES  0x03
/*
 * END: defines for SSC1315 Registers
 */

typedef enum
{
  ssd1315_NO_SEND = 0x00,
  ssd1315_COMMAND = 0x01,
  ssd1315_DATA = 0x02,
}ssd1315_SendTypes_TypeDef;

typedef struct
{
  uint8_t Frame[SSD1315_LCD_COLUMN_NUMBER * SSD1315_LCD_PAGE_NUMBER];
  uint8_t data[SSD1315_LCD_COLUMN_NUMBER * SSD1315_LCD_PAGE_NUMBER];
  uint8_t commands[20];
}SSD1315_Buffer_TypeDef;

typedef struct
{
  SPI_HandleTypeDef* hspi;
  GPIO_TypeDef* GPIO_DC;
  uint16_t PIN_DC;
  GPIO_TypeDef* GPIO_RST;
  uint16_t PIN_RST;
  GPIO_TypeDef* GPIO_CS;
  uint16_t PIN_CS;
  SSD1315_Buffer_TypeDef buffer;
  bool interrupted;
  ssd1315_SendTypes_TypeDef activeSendType;
  uint32_t numData;
  uint32_t numCommands;
  bool dataTransmissionOnHold;
  bool commandTransmissionOnHold;
  bool dataSendRequest;
  bool commandSendRequest;
  bool initReady;
  bool initialized;
  bool refreshRequired;
  uint16_t currentPosX;
  uint16_t currentPosY;
  uint16_t currentColor;
}SSD1315_OLED_TypeDef;

/*
 * BEGIN: Functions to initialize ssd1315 and its connections.
 */
void ssd1315_init_PinDC(SSD1315_OLED_TypeDef* ssd1315, GPIO_TypeDef* GPIO_DC, uint16_t PIN_DC);
void ssd1315_init_PinCS(SSD1315_OLED_TypeDef* ssd1315, GPIO_TypeDef* GPIO_CS, uint16_t PIN_CS);
void ssd1315_init_PinRST(SSD1315_OLED_TypeDef* ssd1315, GPIO_TypeDef* GPIO_RST, uint16_t PIN_RST);
void ssd1315_init_SPI(SSD1315_OLED_TypeDef* ssd1315, SPI_HandleTypeDef* hspi);
void ssd1315_init_Device(SSD1315_OLED_TypeDef* ssd1315);
void ssd1315_start(SSD1315_OLED_TypeDef* ssd1315);
/*
 * END: Functions to initialize ssd1315 and its connections.
 */

/*
 * BEGIN: Functions to control the Display.
 */
void ssd1315_DisplayOn(SSD1315_OLED_TypeDef* ssd1315);
void ssd1315_DisplayOff(SSD1315_OLED_TypeDef* ssd1315);
void ssd1315_clear(SSD1315_OLED_TypeDef* ssd1315, uint16_t RGBCode);
void ssd1315_refresh(SSD1315_OLED_TypeDef* ssd1315);

/*
 * BEGIN: Functions to update transmission.
 */
void ssd1315_update_Transmisison(SSD1315_OLED_TypeDef* ssd1315);
/*
 * END: Functions to update transmission.
 */

void ssd1315_write_Pixel(SSD1315_OLED_TypeDef* ssd1315, uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode);
uint16_t ssd1315_read_Pixel(SSD1315_OLED_TypeDef* ssd1315, uint16_t Xpos, uint16_t Ypos);
void ssd1315_set_Page(SSD1315_OLED_TypeDef* ssd1315, uint16_t Page);
void ssd1315_set_Column(SSD1315_OLED_TypeDef* ssd1315, uint16_t Column);

void ssd1315_draw_HLine(SSD1315_OLED_TypeDef* ssd1315, uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void ssd1315_draw_VLine(SSD1315_OLED_TypeDef* ssd1315, uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void ssd1315_draw_Rectangle(SSD1315_OLED_TypeDef* ssd1315, uint8_t width, uint8_t height);

void ssd1315_draw_Bitmap(SSD1315_OLED_TypeDef* ssd1315, uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp);
void ssd1315_draw_BitmapBeta(SSD1315_OLED_TypeDef* ssd1315, uint8_t width, uint8_t height, uint8_t* bitmap);
void ssd1315_shift_Bitmap(SSD1315_OLED_TypeDef* ssd1315, uint16_t Xpos, uint16_t Ypos, int16_t Xshift, int16_t Yshift,uint8_t *pbmp);

void ssd1315_set_StartPositionXY(SSD1315_OLED_TypeDef* ssd1315, uint16_t Xpos, uint16_t Ypos);
void ssd1315_set_CurrentColor(SSD1315_OLED_TypeDef* ssd1315, uint16_t RGBCode);
void ssd1315_write_Character(SSD1315_OLED_TypeDef* ssd1315, Fonts_TypeDef Font, char chr);
void ssd1315_write_String(SSD1315_OLED_TypeDef* ssd1315, Fonts_TypeDef Font, char* str, uint32_t length);

uint16_t ssd1315_get_LcdPixelWidth(SSD1315_OLED_TypeDef* ssd1315);
uint16_t ssd1315_get_LcdPixelHeight(SSD1315_OLED_TypeDef* ssd1315);

void ssd1315_setup_Scrolling(SSD1315_OLED_TypeDef* ssd1315, uint16_t ScrollMode, uint16_t StartPage, uint16_t EndPage, uint16_t Frequency);
void ssd1315_start_Scrolling(SSD1315_OLED_TypeDef* ssd1315);
void ssd1315_stop_Scrolling(SSD1315_OLED_TypeDef* ssd1315);
/*
 * END: Functions to control the Display.
 */

/*
 * BEGIN: Functions to be called in SPI Interrupt handler.
 */
void ssd1315_interrupt(SSD1315_OLED_TypeDef* ssd1315, SPI_HandleTypeDef* hspi);
/*
 * END: Functions to be called in SPI Interrupt handler.
 */
#endif /* PERIPHERALS_SSD1315_H_ */
