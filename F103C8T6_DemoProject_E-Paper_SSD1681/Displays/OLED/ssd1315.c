/*
 * ssd1315.c
 *
 *  Created on: Sep 28, 2023
 *      Author: Mario
 */

#include "ssd1315.h"
#include <string.h>

/*
 * BEGIN: Functions to send SPI data to SSD1315
 */
void spi_write_Data(SSD1315_OLED_TypeDef* ssd1315, uint8_t* data, uint32_t size)
{
  for(uint32_t i = 0; i < size; i++)
  {
    ssd1315->buffer.data[ssd1315->numData] = data[i];
    ssd1315->numData++;
  }
  if(ssd1315->dataTransmissionOnHold == false)
  {
    ssd1315->commandTransmissionOnHold = true;
    ssd1315->activeSendType = ssd1315_DATA;
  }
  else
  {
    ssd1315->dataSendRequest = true;
  }
}

void spi_write_Command(SSD1315_OLED_TypeDef* ssd1315, uint8_t command)
{
  ssd1315->buffer.commands[ssd1315->numCommands] = command;
  ssd1315->numCommands++;
  if(ssd1315->commandTransmissionOnHold == false)
  {
    ssd1315->dataTransmissionOnHold = true;
    ssd1315->activeSendType = ssd1315_COMMAND;
  }
  else
  {
    ssd1315->commandSendRequest = true;
  }


}

/*
 * END: Functions to send SPI data to SSD1315
 */

/*
 * BEGIN: Functions to initialize ssd1315 and its connections.
 */
void ssd1315_init_PinDC(SSD1315_OLED_TypeDef* ssd1315, GPIO_TypeDef* GPIO_DC, uint16_t PIN_DC)
{
  ssd1315->GPIO_DC = GPIO_DC;
  ssd1315->PIN_DC = PIN_DC;
  HAL_GPIO_WritePin(ssd1315->GPIO_DC,ssd1315->PIN_DC, SET);
}

void ssd1315_init_PinCS(SSD1315_OLED_TypeDef* ssd1315, GPIO_TypeDef* GPIO_CS, uint16_t PIN_CS)
{
  ssd1315->GPIO_CS = GPIO_CS;
  ssd1315->PIN_CS = PIN_CS;
  HAL_GPIO_WritePin(ssd1315->GPIO_CS,ssd1315->PIN_CS, SET);
}

void ssd1315_init_PinRST(SSD1315_OLED_TypeDef* ssd1315, GPIO_TypeDef* GPIO_RST, uint16_t PIN_RST)
{
  ssd1315->GPIO_RST = GPIO_RST;
  ssd1315->PIN_RST = PIN_RST;
  HAL_GPIO_WritePin(ssd1315->GPIO_RST,ssd1315->PIN_RST, RESET);
}
void ssd1315_init_SPI(SSD1315_OLED_TypeDef* ssd1315, SPI_HandleTypeDef* hspi)
{
  ssd1315->hspi = hspi;
  ssd1315->initialized = false;
}

void ssd1315_init_Device(SSD1315_OLED_TypeDef* ssd1315)
{
  if(ssd1315->initialized == false)
  {
    ssd1315->initReady = false;
    ssd1315->interrupted = false;
    spi_write_Command(ssd1315, 0x80);
    spi_write_Command(ssd1315, 0x8D);  /* Disable charge pump regulator */
    spi_write_Command(ssd1315, 0x14);
    spi_write_Command(ssd1315, 0x20);  /* Set Memory Addressing Mode */
    spi_write_Command(ssd1315, 0x00);  /* 00b: Horizontal Addressing Mode */
    spi_write_Command(ssd1315, 0x40);  /* 00b: Horizontal Addressing Mode */
    spi_write_Command(ssd1315, 0xC8);  /* c8:flip the 64 rows */
    spi_write_Command(ssd1315, 0xA1);  /* a1:flip the 128 columns */
    spi_write_Command(ssd1315, 0xAF);  /* Display On */

    ssd1315_clear(ssd1315, SSD1315_COLOR_BLACK);

    spi_write_Data(ssd1315, ssd1315->buffer.Frame, SSD1315_LCD_COLUMN_NUMBER*SSD1315_LCD_PAGE_NUMBER);

    /* this command is required to enter the first transmit.*/
    ssd1315->interrupted = true;
    ssd1315->initialized = true;
  }
}
/*
 * END: Functions to initialize ssd1315 and its connections.
 */

void ssd1315_start(SSD1315_OLED_TypeDef* ssd1315)
{
  while(ssd1315->initReady == false)
  {
    if(ssd1315->interrupted == true)
     {
       ssd1315->interrupted = false;
      if(ssd1315->activeSendType != ssd1315_NO_SEND)
      {
        if(ssd1315->activeSendType == ssd1315_COMMAND && !ssd1315->commandTransmissionOnHold)
        {
          HAL_GPIO_WritePin(ssd1315->GPIO_CS, ssd1315->PIN_CS, RESET);
          HAL_GPIO_WritePin(ssd1315->GPIO_DC, ssd1315->PIN_DC, RESET);
          HAL_SPI_Transmit_DMA(ssd1315->hspi, ssd1315->buffer.commands, ssd1315->numCommands);
          ssd1315->numCommands = 0x00;
          if(ssd1315->dataSendRequest  == true)
          {
            ssd1315->dataTransmissionOnHold  = false;
            ssd1315->activeSendType = ssd1315_DATA;
            ssd1315->dataSendRequest = false;
          }
          else
          {
            ssd1315->activeSendType = ssd1315_NO_SEND;
            ssd1315->initReady = true;
          }
        }
        else if(ssd1315->activeSendType == ssd1315_DATA)
        {
          HAL_GPIO_WritePin(ssd1315->GPIO_CS, ssd1315->PIN_CS, RESET);
          HAL_SPI_Transmit_DMA(ssd1315->hspi, ssd1315->buffer.data, ssd1315->numData);
          ssd1315->numData = 0x00;
          if(ssd1315->commandSendRequest == true)
          {
            ssd1315->commandTransmissionOnHold = false;
            ssd1315->activeSendType = ssd1315_COMMAND;
            ssd1315->commandSendRequest = false;
          }
          else
          {
            ssd1315->activeSendType = ssd1315_NO_SEND;
            ssd1315->initReady = true;
          }
        }
      }
    }
  }
}
/*
 * BEGIN: Functions to update transmission.
 */
void ssd1315_update_Transmisison(SSD1315_OLED_TypeDef* ssd1315)
{
  if((ssd1315->activeSendType == ssd1315_NO_SEND) && ssd1315->refreshRequired == true)
  {
    ssd1315_refresh(ssd1315);
    ssd1315->refreshRequired = false;
  }
  if(ssd1315->interrupted == true)
  {
    if(ssd1315->activeSendType != ssd1315_NO_SEND)
    {
      if(ssd1315->activeSendType == ssd1315_COMMAND && !ssd1315->commandTransmissionOnHold)
      {
        HAL_GPIO_WritePin(ssd1315->GPIO_CS, ssd1315->PIN_CS, RESET);
        HAL_GPIO_WritePin(ssd1315->GPIO_DC, ssd1315->PIN_DC, RESET);
        HAL_SPI_Transmit_DMA(ssd1315->hspi, ssd1315->buffer.commands, ssd1315->numCommands);
        ssd1315->interrupted = false;
        ssd1315->numCommands = 0x00;
        if(ssd1315->dataSendRequest  == true)
        {
          ssd1315->dataTransmissionOnHold  = false;
          ssd1315->activeSendType = ssd1315_DATA;
          ssd1315->dataSendRequest = false;
        }
        else
        {
          ssd1315->activeSendType = ssd1315_NO_SEND;
        }
      }
      else if(ssd1315->activeSendType == ssd1315_DATA)
      {
        HAL_GPIO_WritePin(ssd1315->GPIO_CS, ssd1315->PIN_CS, RESET);
        HAL_SPI_Transmit_DMA(ssd1315->hspi, ssd1315->buffer.data, ssd1315->numData);
        ssd1315->interrupted = false;
        ssd1315->numData = 0x00;
        if(ssd1315->commandSendRequest == true)
        {
          ssd1315->commandTransmissionOnHold = false;
          ssd1315->activeSendType = ssd1315_COMMAND;
          ssd1315->commandSendRequest = false;
        }
        else
        {
          ssd1315->activeSendType = ssd1315_NO_SEND;
        }
      }
    }
  }
}
/*
 * END: Functions to update transmission.
 */

/*
 * BEGIN: Functions to control the Display.
 */
void ssd1315_DisplayOn(SSD1315_OLED_TypeDef* ssd1315)
{
  /* Power On sequence */
  spi_write_Command(ssd1315, 0x8D);
  spi_write_Command(ssd1315, 0x14);
  spi_write_Command(ssd1315, 0xAF);
}

void ssd1315_DisplayOff(SSD1315_OLED_TypeDef* ssd1315)
{
  /* Power Off sequence */
  spi_write_Command(ssd1315, 0x8D);
  spi_write_Command(ssd1315, 0x10);
  spi_write_Command(ssd1315, 0xAE);
}

void ssd1315_clear(SSD1315_OLED_TypeDef* ssd1315, uint16_t RGBCode)
{
  /* Check color */
  if (RGBCode == SSD1315_COLOR_WHITE)
  {
    memset(ssd1315->buffer.Frame, 0xFF, SSD1315_LCD_COLUMN_NUMBER * SSD1315_LCD_PAGE_NUMBER);
  }
  else
  {
    memset(ssd1315->buffer.Frame, 0x00, SSD1315_LCD_COLUMN_NUMBER * SSD1315_LCD_PAGE_NUMBER);
  }
  ssd1315->refreshRequired = true;
}

void ssd1315_refresh(SSD1315_OLED_TypeDef* ssd1315)
{
  /* Set Display Start Line to 0*/
  spi_write_Command(ssd1315, 0x40);
  /* Set Column Address Setup column start(0) and end address (127)*/
  spi_write_Command(ssd1315, 0x21);
  spi_write_Command(ssd1315, 0x00);
  spi_write_Command(ssd1315, 0x7F);
  /* Set Page Address Setup page start (0)  and end address (7)*/
  spi_write_Command(ssd1315, 0x22);
  spi_write_Command(ssd1315, 0x00);
  spi_write_Command(ssd1315, 0x07);
  /* Fill Buffer in GDDRAM of LCD */
  spi_write_Data(ssd1315, ssd1315->buffer.Frame, SSD1315_LCD_COLUMN_NUMBER*SSD1315_LCD_PAGE_NUMBER);

}

void ssd1315_write_Pixel(SSD1315_OLED_TypeDef* ssd1315, uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode)
{
  /* Set color */
  if (RGBCode == SSD1315_COLOR_WHITE) {
    ssd1315->buffer.Frame[Xpos + (Ypos / 8) * SSD1315_LCD_PIXEL_WIDTH] |= 1 << (Ypos % 8);
  } else {
    ssd1315->buffer.Frame[Xpos + (Ypos / 8) * SSD1315_LCD_PIXEL_WIDTH] &= ~(1 << (Ypos % 8));
  }
  ssd1315->refreshRequired = true;
}

void ssd1315_set_StartPositionXY(SSD1315_OLED_TypeDef* ssd1315, uint16_t Xpos, uint16_t Ypos)
{
  ssd1315->currentPosX = Xpos;
  ssd1315->currentPosY = Ypos;
}

void ssd1315_set_CurrentColor(SSD1315_OLED_TypeDef* ssd1315, uint16_t RGBCode)
{
  ssd1315->currentColor = RGBCode;
}

void ssd1315_write_Character(SSD1315_OLED_TypeDef* ssd1315, Fonts_TypeDef Font, char chr)
{
#if 0
  uint32_t Yoffset, currentByte, Xoffset, MSBcompare;
  /* count pixel position from bottom to the top. */
  for(Yoffset = 0; Yoffset < Font.Height; Yoffset++)
  {
    if(Font.Width < 8)
    {
      currentByte = Font.table[(chr - 32) * Font.Height + Yoffset];
      MSBcompare = 0x80;
    }
    else if(Font.Width >= 8 && Font.Width < 16)
    {
      uint8_t Byte1 = Font.table[(chr - 32) * Font.Height * 2 + Yoffset * 2];
      uint8_t Byte2 = Font.table[(chr - 32) * Font.Height * 2 + (Yoffset * 2 + 1)];
      currentByte = (Byte1 << 8) + Byte2;
      MSBcompare = 0x8000;
    }
    else if(Font.Width > 16)
    {
      uint8_t Byte1 = Font.table[(chr - 32) * Font.Height * 3 + Yoffset * 3];
      uint8_t Byte2 = Font.table[(chr - 32) * Font.Height * 3 + (Yoffset * 3 + 1)];
      uint8_t Byte3 = Font.table[(chr - 32) * Font.Height * 3 + (Yoffset * 3 + 2)];
      currentByte = (Byte1 << 16) + (Byte2 << 8) + Byte3;
      MSBcompare = 0x800000;
    }
    /* count pixels from left to right. */
    for(Xoffset = 0; Xoffset < Font.Width; Xoffset++)
    {
      if((currentByte << Xoffset) & MSBcompare)
      {
        ssd1315_write_Pixel(ssd1315, ssd1315->currentPosX + Xoffset, ssd1315->currentPosY + Yoffset, ssd1315->currentColor);
      }
      else
        ssd1315_write_Pixel(ssd1315, ssd1315->currentPosX + Xoffset, ssd1315->currentPosY + Yoffset, !ssd1315->currentColor);
    }
  }
  ssd1315->currentPosX += Font.Height;
#endif
}

void ssd1315_write_String(SSD1315_OLED_TypeDef* ssd1315, Fonts_TypeDef Font, char* str, uint32_t length)
{
  for(uint32_t i = 0; i < length; i++)
  {
    ssd1315_write_Character(ssd1315, Font, str[i]);
  }
}

uint16_t ssd1315_read_Pixel(SSD1315_OLED_TypeDef* ssd1315, uint16_t Xpos, uint16_t Ypos)
{
  if ((Xpos >= SSD1315_LCD_PIXEL_WIDTH) || (Ypos >= SSD1315_LCD_PIXEL_HEIGHT)) return 0;
  return ssd1315->buffer.Frame[Xpos+ (Ypos/8)*SSD1315_LCD_PIXEL_WIDTH] & (1 << Ypos%8) ? 1 : 0;
}

void ssd1315_set_Page(SSD1315_OLED_TypeDef* ssd1315, uint16_t Page)
{
  /* Set Page position  */
  spi_write_Command(ssd1315, 0xB0 | Page);
}

void ssd1315_set_Column(SSD1315_OLED_TypeDef* ssd1315, uint16_t Column)
{
  /* Set Column position */
  spi_write_Command(ssd1315, 0x00);
  spi_write_Command(ssd1315, 0x00 | Column);
  spi_write_Command(ssd1315, 0x1F);
}

void ssd1315_draw_HLine(SSD1315_OLED_TypeDef* ssd1315, uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t i = 0;

  /* Send a complete horizontal line */
  for (i = Xpos; i < (Xpos+Length); i++)
  {
    ssd1315_write_Pixel(ssd1315, i, Ypos, RGBCode);
  }
}

void ssd1315_draw_VLine(SSD1315_OLED_TypeDef* ssd1315, uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t i = 0;

  /* Send a complete vertical line */
  for (i = Ypos; i < (Ypos+Length); i++)
  {
    ssd1315_write_Pixel(ssd1315, Xpos, i, RGBCode);
  }
}

void ssd1315_draw_Rectangle(SSD1315_OLED_TypeDef* ssd1315, uint8_t width, uint8_t height)
{
  /* Top line */
  ssd1315_draw_HLine(ssd1315, SSD1315_COLOR_WHITE, ssd1315->currentPosX, ssd1315->currentPosY, width); /* Blue area begins at YPos 16. */
  /* Bottom line */
  ssd1315_draw_HLine(ssd1315, SSD1315_COLOR_WHITE, ssd1315->currentPosX, (ssd1315->currentPosY + height - 1), width);
  /* Left line */
  ssd1315_draw_VLine(ssd1315, SSD1315_COLOR_WHITE, ssd1315->currentPosX, (ssd1315->currentPosY + 1), (height - 2));
  /* Right line */
  ssd1315_draw_VLine(ssd1315, SSD1315_COLOR_WHITE, (ssd1315->currentPosX + width - 1), (ssd1315->currentPosY + 1), (height - 2));

}

void ssd1315_draw_BitmapBeta(SSD1315_OLED_TypeDef* ssd1315, uint8_t width, uint8_t height, uint8_t* bitmap)
{
  uint8_t byteWidth = (width + 7) / 8;
  uint8_t currentByte;
  uint8_t compareMSB = 0x80;
  uint8_t heightCnt, bytWdthCnt, bitCnt;
  for(heightCnt = 0; heightCnt < height; heightCnt++)
  {
    for(bytWdthCnt = 0; bytWdthCnt < byteWidth; bytWdthCnt++)
    {
      currentByte = bitmap[byteWidth * heightCnt + bytWdthCnt];
      for(bitCnt = 0; bitCnt < 8; bitCnt++)
      {
        if((currentByte << bitCnt) & compareMSB)
        {
          ssd1315_write_Pixel(ssd1315, ssd1315->currentPosX + bytWdthCnt * 8 + bitCnt, ssd1315->currentPosY + heightCnt, ssd1315->currentColor);
        }
        else
          ssd1315_write_Pixel(ssd1315, ssd1315->currentPosX + bytWdthCnt * 8 + bitCnt, ssd1315->currentPosY + heightCnt, !ssd1315->currentColor);
      }
    }
  }
#if 0
  uint32_t index = 0, size = 0;
  uint32_t height = 0, width  = 0;
  uint32_t x = 0, y  = 0, y0 = 0;
  uint32_t XposBMP = 0, YposBMP  = 0;

  /* Read bitmap size */
  size = pbmp[2] + (pbmp[3] << 8) + (pbmp[4] << 16)  + (pbmp[5] << 24);

  /* Get bitmap data address offset */
  index = pbmp[10] + (pbmp[11] << 8) + (pbmp[12] << 16)  + (pbmp[13] << 24);

  /* Read bitmap width */
  width = pbmp[18] + (pbmp[19] << 8) + (pbmp[20] << 16)  + (pbmp[21] << 24);

  /* Read bitmap height */
  height = pbmp[22] + (pbmp[23] << 8) + (pbmp[24] << 16)  + (pbmp[25] << 24);

  /* Size converion */
  size = (size - index)/2;

  /* Apply offset to bypass header */
  pbmp += index;

  /* if bitmap cover whole screen */
  if((Xpos == 0) && (Xpos == 0) & (size == (SSD1315_LCD_PIXEL_WIDTH * SSD1315_LCD_PIXEL_HEIGHT/8)))
  {
    memcpy(ssd1315->buffer.Frame, pbmp, size);
  }
  else
  {
    x=Xpos+width;
    y=Ypos+height;
    y0 = Ypos;

    for(; Xpos < x; Xpos++, XposBMP++)
    {
      for(Ypos = y0, YposBMP = 0; Ypos < y; Ypos++, YposBMP++)
      {
        /* if bitmap and screen are aligned on a Page */
        if(((Ypos%8) == 0) && (y-Ypos >= 8) && ((YposBMP%8) == 0))
        {
          ssd1315->buffer.Frame[Xpos+ (Ypos/8)*SSD1315_LCD_PIXEL_WIDTH] = pbmp[XposBMP+((YposBMP/8)*width)];
          Ypos+=7;
          YposBMP+=7;
        }
        else
        {
          /* Draw bitmap pixel per pixel */
          if( (pbmp[XposBMP+((YposBMP/8)*width)]&(1<<(YposBMP%8))) != 0)
             ssd1315_write_Pixel(ssd1315, Xpos, Ypos, 0xFF );
          else
            ssd1315_write_Pixel(ssd1315, Xpos, Ypos, 0x00 );
        }
      }
    }
  }
#endif
}

void ssd1315_shift_Bitmap(SSD1315_OLED_TypeDef* ssd1315, uint16_t Xpos, uint16_t Ypos, int16_t Xshift, int16_t Yshift,uint8_t *pbmp)
{
  uint32_t index = 0, size = 0;
  uint32_t height = 0, width  = 0, original_width  = 0;
  uint32_t x = 0, y  = 0, y0 = 0;
  uint32_t XposBMP = 0, YposBMP  = 0, original_YposBMP = 0;

  /* Read bitmap size */
  size = *(volatile uint16_t *) (pbmp + 2);
  size |= (*(volatile uint16_t *) (pbmp + 4)) << 16;

  /* Get bitmap data address offset */
  index = *(volatile uint16_t *) (pbmp + 10);
  index |= (*(volatile uint16_t *) (pbmp + 12)) << 16;

  /* Read bitmap width */
  width = *(uint16_t *) (pbmp + 18);
  width |= (*(uint16_t *) (pbmp + 20)) << 16;
  original_width = width;
  if( Xshift>=0)
  {
    Xpos = Xpos + Xshift;
    width = width - Xshift;
  }
  else
  {
    width = width + Xshift;
    XposBMP = -Xshift;
  }

  /* Read bitmap height */
  height = *(uint16_t *) (pbmp + 22);
  height |= (*(uint16_t *) (pbmp + 24)) << 16;
  if( Yshift>=0)
  {
    height = height - Yshift;
    Ypos = Ypos + Yshift;
  }
  else
  {
    height = height + Yshift;
    YposBMP = -Yshift;
  }
  original_YposBMP = YposBMP;

  /* Size converion */
  size = (size - index)/2;
  size = size - ((Xshift*height/8)+(Yshift*width/8 ));

  /* Apply offset to bypass header */
  pbmp += index;

  /* if bitmap cover whole screen */
  if((Xpos == 0) && (Xpos == 0) & (size == (SSD1315_LCD_PIXEL_WIDTH * SSD1315_LCD_PIXEL_HEIGHT/8)))
  {
    memcpy(ssd1315->buffer.Frame, pbmp, size);
  }
  else
  {
    x=Xpos+width;
    y=Ypos+height;
    y0 = Ypos;

    for(; Xpos < x; Xpos++, XposBMP++)
    {
      for(Ypos = y0, YposBMP = original_YposBMP; Ypos < y; Ypos++, YposBMP++)
      {
        /* if bitmap and screen are aligned on a Page */
        if(((Ypos%8) == 0) && (y-Ypos >= 8) && ((YposBMP%8) == 0))
        {
          ssd1315->buffer.Frame[Xpos+ (Ypos/8)*SSD1315_LCD_PIXEL_WIDTH] = pbmp[XposBMP+((YposBMP/8)*original_width)];
          Ypos+=7;
          YposBMP+=7;
        }
        else
        {
          /* Draw bitmap pixel per pixel */
          if( (pbmp[XposBMP+((YposBMP/8)*original_width)]&(1<<(YposBMP%8))) != 0)
            ssd1315_write_Pixel(ssd1315, Xpos, Ypos, 0xFF );
          else
            ssd1315_write_Pixel(ssd1315, Xpos, Ypos, 0x00 );
        }
      }
    }
  }
}

uint16_t ssd1315_get_LcdPixelWidth(SSD1315_OLED_TypeDef* ssd1315)
{
  return (uint16_t)SSD1315_LCD_PIXEL_WIDTH;
}

uint16_t ssd1315_get_LcdPixelHeight(SSD1315_OLED_TypeDef* ssd1315)
{
  return (uint16_t)SSD1315_LCD_PIXEL_HEIGHT;
}

void ssd1315_setup_Scrolling(SSD1315_OLED_TypeDef* ssd1315, uint16_t ScrollMode, uint16_t StartPage, uint16_t EndPage, uint16_t Frequency)
{
  /* Scrolling setup sequence */
  spi_write_Command(ssd1315, ScrollMode);  /* Right/Left Horizontal Scroll */
  spi_write_Command(ssd1315, 0x00);        /* Dummy byte (Set as 00h) */
  spi_write_Command(ssd1315, StartPage);   /* start page address*/
  spi_write_Command(ssd1315, Frequency);   /* start page address*/
  spi_write_Command(ssd1315, EndPage);     /* End page address*/
  spi_write_Command(ssd1315, 0x00);        /* Dummy byte (Set as 00h) */
  spi_write_Command(ssd1315, 0xFF);        /* Dummy byte (Set as ffh) */
}

void ssd1315_start_Scrolling(SSD1315_OLED_TypeDef* ssd1315)
{
  /* Start scrolling sequence */
  spi_write_Command(ssd1315, 0x2F);
}

void ssd1315_stop_Scrolling(SSD1315_OLED_TypeDef* ssd1315)
{
  /* Stop scrolling sequence */
  spi_write_Command(ssd1315, 0x2E);
}
/*
 * END: Functions to control the Display.
 */

/*
 * BEGIN: Functions to be called in SPI Interrupt handler.
 */

void ssd1315_interrupt(SSD1315_OLED_TypeDef* ssd1315, SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance == ssd1315->hspi->Instance)
  {
    ssd1315->interrupted = true;
    HAL_GPIO_WritePin(ssd1315->GPIO_CS, ssd1315->PIN_CS, SET);
    HAL_GPIO_WritePin(ssd1315->GPIO_DC, ssd1315->PIN_DC, SET);
  }
}

/*
 * END: Functions to be called in SPI Interrupt handler.
 */
