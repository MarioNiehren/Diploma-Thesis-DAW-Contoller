/*
 * GUI.c
 *
 *  Created on: Oct 18, 2023
 *      Author: Mario Niehren
 */

#include "GUI.h"

/***************************************************************************************************
 * BEGIN: Functions to initialize GUI.
 ***************************************************************************************************/
void GUI_init_Size(GUI_TypeDef* GUI, uint16_t Width, uint16_t Height)
{
  GUI->Width = Width;
  GUI->Height = Height;
  GUI->WidthBytes = (Width % 8 == 0) ? (Width / 8) : (Width / 8 + 1); /* check division error.
                                                                     * Divide by 8 to get
                                                                     * the width in bytes.*/
  GUI->Line.Width = 0x01;
}

void GUI_init_Buffer(GUI_TypeDef* GUI, uint8_t* Buffer)
{
  GUI->Buffer = Buffer;
}

void GUI_init(GUI_TypeDef* GUI)
{
  GUI->Invert = GUI_NO_INVERT;
}

/***************************************************************************************************
 * END: Functions to initialize GUI.
 ***************************************************************************************************/




/***************************************************************************************************
 * BEGIN: Functions to set struct characteristics.
 ***************************************************************************************************/
/* Functions to set characteristics for individual pixels */
void GUI_set_PixelStateToDraw(GUI_TypeDef* GUI, GUI_PixelState_TypeDef PixelState)
{
  GUI->PixelState = PixelState;
}

/* Functions to set characteristics for drawing lines */
void GUI_set_LineWidth(GUI_TypeDef* GUI, uint8_t Width)
{
  GUI->Line.Width = Width;
}

void GUI_set_LineDirection(GUI_TypeDef* GUI, GUI_Direction_TypeDef Direction)
{
  GUI->Line.Direction = Direction;
}

/* Functions to navigate in Buffer */
void GUI_set_StartPoint(GUI_TypeDef* GUI, uint16_t X, uint16_t Y)
{
  GUI->Start.X = X;
  GUI->Start.Y = Y;
  GUI->PositionResetRequest = true;
}

void GUI_set_Invert(GUI_TypeDef* GUI, GUI_Invert_TypeDef Invert)
{
  GUI->Invert = Invert;
}

void GUI_set_Font(GUI_TypeDef* GUI, Fonts_TypeDef Font)
{
  GUI->Font = Font;
}
/***************************************************************************************************
 * END: Functions to set struct characteristics.
 ***************************************************************************************************/




/***************************************************************************************************
 * BEGIN: Functions to design GUI.
 ***************************************************************************************************/
void GUI_draw_Pixel(GUI_TypeDef* GUI, uint16_t X, uint16_t Y)
{
  if(GUI->PixelState == GUI_PIXEL_SET)
  {
    GUI->Buffer[X/8 + Y * GUI->WidthBytes] |= 0x80 >> (X % 8);
  }
  else if(GUI->PixelState == GUI_PIXEL_RESET)
  {
    GUI->Buffer[X/8 + Y * GUI->WidthBytes] &= ~(0x80 >> (X % 8));
  }
}

void GUI_draw_Line(GUI_TypeDef* GUI,  uint16_t X, uint16_t Y, uint16_t Length)
{
  if(GUI->Line.Direction == GUI_HORIZONTAL)
  {
    for(uint16_t Xnow = X; Xnow < (X + Length); Xnow++)
    {
      /* Increase thickness of Line in Y direction. */
      for(uint8_t i = 0; i < GUI->Line.Width; i++)
      {
        GUI_draw_Pixel(GUI, Xnow, Y + i);
      }
    }
  }
  else if(GUI->Line.Direction == GUI_VERTICAL)
  {
    for(uint16_t Ynow = Y; Ynow < (Y + Length); Ynow++)
    {
      /* Increase thickness of Line in X direction. */
      for(uint8_t i = 0; i < GUI->Line.Width; i++)
      {
        GUI_draw_Pixel(GUI, X + i, Ynow);
      }
    }
  }
}


void GUI_draw_Rectangle(GUI_TypeDef* GUI, uint8_t Width, uint8_t Height)
{
  /* Top line */
  GUI_set_LineDirection(GUI, GUI_HORIZONTAL);
  GUI_draw_Line(GUI, GUI->Start.X, GUI->Start.Y, Width);

  /* Bottom line */
  GUI_draw_Line(GUI, GUI->Start.X, (GUI->Start.Y + Height - 1), Width);

  /* Left line */
  GUI_set_LineDirection(GUI, GUI_VERTICAL);
  GUI_draw_Line(GUI, GUI->Start.X, (GUI->Start.Y + 1), (Height - 2));

  /* Right line */
  GUI_draw_Line(GUI, (GUI->Start.X + Width - 1), (GUI->Start.Y + 1), (Height - 2));
}


/***************************************************************************************************
 * BEGIN: Private Function: void write_ASCII(GUI_TypeDef* GUI, uint8_t Sign, int8_t OffsetASCII)
 ***************************************************************************************************/

/*
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
 * Tool for bitmap generation: [The Dot Factory](http://www.eran.io/the-dot-factory-an-lcd-font-and-image-generator/)
 *
 * Sign Set ASCII 32 - 126:
 *
 *  !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
 *
 */

/* Prototypes */
GUI_PixelState_TypeDef get_PixelStateDraw(GUI_TypeDef* GUI);
GUI_PixelState_TypeDef get_PixelStateErase(GUI_TypeDef* GUI);
uint8_t get_CurrentWidth(GUI_TypeDef* GUI, uint32_t IndexNow);
uint32_t get_CurrentBitmapIndex(GUI_TypeDef* GUI, uint8_t WidthNow, uint32_t IndexNow);
uint32_t get_CurrentDoubleWord(GUI_TypeDef* GUI, uint8_t WidthNow, uint32_t IndexNow, uint32_t IndexOffset);
uint32_t get_CurrentMSBCompare(GUI_TypeDef* GUI, uint8_t WidthNow);

void write_ASCII(GUI_TypeDef* GUI, uint8_t Sign, int8_t OffsetASCII)
{
	if(GUI->PositionResetRequest == true)
	{
		GUI->PositionNow.X = GUI->Start.X;
		GUI->PositionNow.Y = GUI->Start.Y;
		GUI->PositionResetRequest = false;
	}

  uint32_t Yoffset, currentBytes, Xoffset;
  GUI_PixelState_TypeDef tmp_StateDrawPx = get_PixelStateDraw(GUI);
  GUI_PixelState_TypeDef tmp_StateErasePx = get_PixelStateErase(GUI);
  uint32_t tmp_SignIndexNow = Sign + OffsetASCII;
  uint8_t tmp_WidthNow = get_CurrentWidth(GUI, tmp_SignIndexNow);
  uint32_t tmp_BitmapIndexNow = get_CurrentBitmapIndex(GUI, tmp_WidthNow, tmp_SignIndexNow);
  uint32_t MSBcompare =  get_CurrentMSBCompare(GUI, tmp_WidthNow);

  /* count pixel position from bottom to the top. */
  for(Yoffset = 0; Yoffset < GUI->Font.Height; Yoffset++)
  {
    currentBytes = get_CurrentDoubleWord(GUI, tmp_WidthNow, tmp_BitmapIndexNow, Yoffset);

    /* count pixels from left to right. */
    for(Xoffset = 0; Xoffset < tmp_WidthNow; Xoffset++)
    {
      if((currentBytes << Xoffset) & MSBcompare)
      {
        GUI_set_PixelStateToDraw(GUI, tmp_StateDrawPx);
        GUI_draw_Pixel(GUI, GUI->PositionNow.X + Xoffset, GUI->PositionNow.Y + Yoffset);
      }
      else
      {
        GUI_set_PixelStateToDraw(GUI, tmp_StateErasePx);
        GUI_draw_Pixel(GUI, GUI->PositionNow.X + Xoffset, GUI->PositionNow.Y + Yoffset); /* replace with clear pixel */
      }
    }
  }
  GUI->PixelState = tmp_StateDrawPx;
  GUI->PositionNow.X += tmp_WidthNow - 1;
}

GUI_PixelState_TypeDef get_PixelStateDraw(GUI_TypeDef* GUI)
{
  GUI_PixelState_TypeDef tmp_PixelStateDraw;
  if(GUI->PixelState == GUI_PIXEL_SET)
  {
    tmp_PixelStateDraw = GUI_PIXEL_SET;
  }
  else if(GUI->PixelState == GUI_PIXEL_RESET)
  {
    tmp_PixelStateDraw = GUI_PIXEL_RESET;
  }
  return tmp_PixelStateDraw;
}

GUI_PixelState_TypeDef get_PixelStateErase(GUI_TypeDef* GUI)
{
  GUI_PixelState_TypeDef tmp_PixelStateErase;
  if(GUI->PixelState == GUI_PIXEL_SET)
  {
    tmp_PixelStateErase = GUI_PIXEL_RESET;
  }
  else if(GUI->PixelState == GUI_PIXEL_RESET)
  {
    tmp_PixelStateErase = GUI_PIXEL_SET;
  }
  return tmp_PixelStateErase;
}


uint8_t get_CurrentWidth(GUI_TypeDef* GUI, uint32_t IndexNow)
{
  uint8_t tmp_CurrentWidth = 0x00;
  if(GUI->Font.Widthtype == FONTS_VARIABLE_WIDTH)
  {
    tmp_CurrentWidth = GUI->Font.Descriptor[IndexNow].Width;
  }
  else if(GUI->Font.Widthtype == FONTS_CONSTANT_WIDTH)
  {
    tmp_CurrentWidth = GUI->Font.Descriptor[0].Width;
  }
  return tmp_CurrentWidth;
}

uint32_t get_CurrentBitmapIndex(GUI_TypeDef* GUI, uint8_t WidthNow, uint32_t IndexNow)
{
  uint32_t tmp_BitmapIndex = 0x00;
  if(GUI->Font.Widthtype == FONTS_VARIABLE_WIDTH)
  {
    tmp_BitmapIndex = GUI->Font.Descriptor[IndexNow].BitmapIndex;
  }
  else if(GUI->Font.Widthtype == FONTS_CONSTANT_WIDTH)
  {
    if(WidthNow < 8)
    {
      tmp_BitmapIndex = IndexNow * GUI->Font.Height;
    }
    else if(WidthNow >= 8 && WidthNow <= 16)
    {
      tmp_BitmapIndex = IndexNow * GUI->Font.Height * 2;
    }
    else if(WidthNow > 16)
    {
      tmp_BitmapIndex = IndexNow * GUI->Font.Height * 3;
    }
  }
  return tmp_BitmapIndex;
}

uint32_t get_CurrentDoubleWord(GUI_TypeDef* GUI, uint8_t WidthNow, uint32_t IndexNow, uint32_t IndexOffset)
{
  uint32_t tmp_CurrentDoubleWord = 0x0000;
  if(WidthNow < 8)
  {
    tmp_CurrentDoubleWord = GUI->Font.Bitmap[IndexNow + IndexOffset];
  }
  else if(WidthNow >= 8 && WidthNow <= 16)
  {
   uint8_t Byte1 = GUI->Font.Bitmap[IndexNow + IndexOffset * 2];
   uint8_t Byte2 = GUI->Font.Bitmap[IndexNow + (IndexOffset * 2 + 1)];
   tmp_CurrentDoubleWord = (Byte1 << 8) + Byte2;
  }
  else if(WidthNow > 16)
  {
   uint8_t Byte1 = GUI->Font.Bitmap[IndexNow + IndexOffset * 3];
   uint8_t Byte2 = GUI->Font.Bitmap[IndexNow + (IndexOffset * 3 + 1)];
   uint8_t Byte3 = GUI->Font.Bitmap[IndexNow + (IndexOffset * 3 + 2)];
   tmp_CurrentDoubleWord = (Byte1 << 16) + (Byte2 << 8) + Byte3;
  }
  return tmp_CurrentDoubleWord;
}

uint32_t get_CurrentMSBCompare(GUI_TypeDef* GUI, uint8_t WidthNow)
{
  uint32_t tmp_CurrentMSBCompare = 0x0000;
  if(WidthNow < 8)
  {
    tmp_CurrentMSBCompare = 0x80;
  }
  else if(WidthNow >= 8 && WidthNow <= 16)
  {
    tmp_CurrentMSBCompare = 0x8000;
  }
  else if(WidthNow > 16)
  {
    tmp_CurrentMSBCompare = 0x800000;
  }
  return tmp_CurrentMSBCompare;
}
/***************************************************************************************************
 * END: Private Function: void write_ASCII(GUI_TypeDef* GUI, uint8_t Sign, int8_t OffsetASCII)
 ***************************************************************************************************/





void GUI_write_Character(GUI_TypeDef* GUI, char chr)
{
  int8_t tmp_OffsetASCII = -32;
  write_ASCII(GUI, chr, tmp_OffsetASCII);
}

void GUI_write_Digit(GUI_TypeDef* GUI, Fonts_TypeDef Font, uint8_t Digit)
{
  int8_t tmp_OffsetASCII = 16;
  write_ASCII(GUI, Digit, tmp_OffsetASCII);
}

/* Struct for this function */
typedef struct
{
  uint32_t PxWidth;
  uint16_t NumElement;
  uint8_t NumSigns;
  uint32_t TotalPxLength;
  bool Space;
}WriteStringDescriptor_TypeDef;

/* Prototypes */
void analyze_String(GUI_TypeDef* GUI, char* str, uint32_t Length, WriteStringDescriptor_TypeDef* Descriptor);
bool check_XPxLimitReached(GUI_TypeDef* GUI);
bool check_YPxLimitReached(GUI_TypeDef* GUI);
void autoset_CoordinatesForStartPoint(GUI_TypeDef* GUI);
void autoset_CoordinatesForNewLine(GUI_TypeDef* GUI);
void write_CharacterToTheRightPosition(GUI_TypeDef* GUI, char* str,  uint32_t Length, WriteStringDescriptor_TypeDef* Descriptor);

void GUI_write_String(GUI_TypeDef* GUI, char* str, uint32_t Length)
{
  WriteStringDescriptor_TypeDef StringDescriptor[Length];
  analyze_String(GUI, str, Length, StringDescriptor);
  autoset_CoordinatesForStartPoint(GUI);

  write_CharacterToTheRightPosition(GUI, str,  Length, StringDescriptor);
}

void analyze_String(GUI_TypeDef* GUI, char* str, uint32_t Length, WriteStringDescriptor_TypeDef* Descriptor)
{
  uint32_t LengthCount = 0x00;
  uint16_t ElementCnt = 0x00;
  uint8_t ASCIIOffset = 32;
  uint8_t NumSigns = 0x00;
  while(LengthCount < Length)
  {
    uint16_t tmp_PxCount = 0x00;

    /* count pixels of word */
    while(str[LengthCount] != 0x20 && LengthCount < Length) /* 0x20: ASCII number for space */
    {
      tmp_PxCount = tmp_PxCount + GUI->Font.Descriptor[str[LengthCount] - ASCIIOffset].Width;
      LengthCount++;
      NumSigns++;
    }

    /* update descriptor */
    if(ElementCnt == 0x00)
    {
      Descriptor[ElementCnt].TotalPxLength = tmp_PxCount;
    }
    else
    {
      Descriptor[ElementCnt].TotalPxLength = Descriptor[ElementCnt - 1].TotalPxLength + tmp_PxCount;
    }
    Descriptor[ElementCnt].Space = false;
    Descriptor[ElementCnt].PxWidth = tmp_PxCount;
    tmp_PxCount = 0x00;
    Descriptor[ElementCnt].NumSigns = NumSigns;
    NumSigns = 0x00;
    Descriptor[ElementCnt].NumElement = ElementCnt;
    ElementCnt++;


    /* count pixels of space */
    while(str[LengthCount] == 0x20 && LengthCount < Length) /* 0x20: ASCII number for space */
    {
      tmp_PxCount = tmp_PxCount + GUI->Font.Descriptor[str[LengthCount] - ASCIIOffset].Width;
      LengthCount++;
      NumSigns++;
    }

    /* update descriptor */
    if(ElementCnt == 0x00)
    {
      Descriptor[ElementCnt].TotalPxLength = tmp_PxCount;
    }
    else
    {
      Descriptor[ElementCnt].TotalPxLength = Descriptor[ElementCnt - 1].TotalPxLength + tmp_PxCount;
    }
    Descriptor[ElementCnt].Space = true;
    Descriptor[ElementCnt].PxWidth = tmp_PxCount;
    tmp_PxCount = 0x00;
    Descriptor[ElementCnt].NumSigns = NumSigns;
    NumSigns = 0x00;
    Descriptor[ElementCnt].NumElement = ElementCnt;
    ElementCnt++;

  }
}

void write_CharacterToTheRightPosition(GUI_TypeDef* GUI, char* str,  uint32_t Length, WriteStringDescriptor_TypeDef* Descriptor)
{
  uint16_t CharCount = 0x00;
  uint16_t ElementCount = 0x00;
  uint32_t PxOffset = 0x00;
  uint32_t PxInLineNow = 0x00;

  PxInLineNow = Descriptor[ElementCount].TotalPxLength; /* get pixel length of 1st element */

  while(CharCount < Length)
  {
    while(PxInLineNow <= (GUI->Width) && CharCount < Length)
    {
      for(uint16_t i = 0; i < Descriptor[ElementCount].NumSigns; i++)
      {
        GUI_write_Character(GUI, str[CharCount]);
        CharCount++;
      }
      if(CharCount < Length)
      {
        ElementCount++;
        PxInLineNow = Descriptor[ElementCount].TotalPxLength - PxOffset;
      }
    }
    autoset_CoordinatesForNewLine(GUI);
    if(CharCount < Length)
    {
      if(Descriptor[ElementCount].Space == true)
      {
        PxOffset = Descriptor[ElementCount].TotalPxLength;
        PxInLineNow = Descriptor[ElementCount].TotalPxLength - PxOffset;
        CharCount++;
        ElementCount++;
      }
      else
      {
        PxOffset = Descriptor[ElementCount - 1].TotalPxLength;
        PxInLineNow = Descriptor[ElementCount - 1].TotalPxLength - PxOffset;
      }
    }


    if(check_YPxLimitReached(GUI))
    {
      CharCount = Length; /* set variable to Length to leave while loop. */
    }
  }
#if 0
  uint8_t tmp_ChrNow = 0x00;
  bool tmp_YPxLimitReached = false;
  while(tmp_ChrNow < Length)
  {
    while(!tmp_YPxLimitReached)
    {
      if(check_XPxLimitReached(GUI))
      {
        tmp_YPxLimitReached = check_YPxLimitReached(GUI);
        if(!tmp_YPxLimitReached)
        {
          autoset_CoordinatesForNewLine(GUI);
        }
      }
      if(tmp_YPxLimitReached)
      {
        tmp_ChrNow = Length;  /* set variable to Length to leave while loop. */
      }
      while(!check_XPxLimitReached(GUI) &&  !tmp_YPxLimitReached)
      {
        GUI_write_Character(GUI, str[tmp_ChrNow]);
        tmp_ChrNow++;
      }
    }
  }
#endif
}
bool check_XPxLimitReached(GUI_TypeDef* GUI)
{
  bool limitReached = false;
  if(GUI->PositionNow.X >= GUI->Width - GUI->Start.X + 1 - 8)
  {
    limitReached = true;
  }
  return limitReached;
}
bool check_YPxLimitReached(GUI_TypeDef* GUI)
{
  bool limitReached = false;
  if((GUI->PositionNow.Y + GUI->Font.Height) > (GUI->Height - GUI->Start.Y))
  {
    limitReached = true;
  }
  return limitReached;
}

void autoset_CoordinatesForStartPoint(GUI_TypeDef* GUI)
{
  GUI->PositionNow.X = GUI->Start.X;
  GUI->PositionNow.Y = GUI->Start.Y;
}

void autoset_CoordinatesForNewLine(GUI_TypeDef* GUI)
{
  GUI->PositionNow.X = GUI->Start.X;
  GUI->PositionNow.Y = GUI->PositionNow.Y + GUI->Font.Height;
}




void GUI_write_Number(GUI_TypeDef* GUI, uint32_t Number)
{
  uint8_t tmp_Digit[10];
  uint8_t tmp_DigitCounter = 0x00;

  while(Number != 0x00)
  {
    tmp_Digit[tmp_DigitCounter] = Number % 10;
    Number = Number / 10;
    tmp_DigitCounter++;
  }

  for(uint8_t i = 0x00; i < tmp_DigitCounter; i++)
  {
    GUI_write_Digit(GUI, GUI->Font, tmp_Digit[tmp_DigitCounter - 1 - i]);
  }
}

void GUI_draw_Bitmap(GUI_TypeDef* GUI, uint8_t* Bitmap, uint16_t Width, uint16_t Height)
{
  uint16_t tmp_BytesWidth = (Width % 8 == 0) ? (Width / 8) : (Width / 8 + 1);
  uint32_t tmp_BitmapIndex = 0x0000;

  for(uint16_t i = 0; i < Height; i++)
  {
    for(uint16_t j = 0; j < tmp_BytesWidth; j++)
    {
      tmp_BitmapIndex = j + i * tmp_BytesWidth;
      if(GUI->Invert == GUI_INVERT)
      {
        GUI->Buffer[tmp_BitmapIndex] = ~Bitmap[tmp_BitmapIndex];
      }
      else if(GUI->Invert == GUI_NO_INVERT)
      {
        GUI->Buffer[tmp_BitmapIndex] = Bitmap[tmp_BitmapIndex];
      }
    }
  }
}

void GUI_clear(GUI_TypeDef* GUI)
{
  uint32_t tmp_FrameIndex = 0x0000;

  if(GUI->PixelState == GUI_PIXEL_RESET)
  {
    for(uint16_t i = 0x00; i < GUI->Height; i++)
    {
      for(uint16_t j = 0x00; j < GUI->WidthBytes; j++)
      {
        tmp_FrameIndex = j + i * GUI->WidthBytes;
        GUI->Buffer[tmp_FrameIndex] = 0x00;
      }
    }
  }
  else if(GUI->PixelState == GUI_PIXEL_SET)
  {
    for(uint16_t i = 0x00; i < GUI->Height; i++)
    {
      for(uint16_t j = 0x00; j < GUI->WidthBytes; j++)
      {
        tmp_FrameIndex = j + i * GUI->WidthBytes;
        GUI->Buffer[tmp_FrameIndex] = 0xFF;
      }
    }
  }
}
/***************************************************************************************************
 * END: Functions to design GUI.
 ***************************************************************************************************/




/***************************************************************************************************
 * BEGIN: Functions get information about GUI.
 ***************************************************************************************************/
uint16_t GUI_get_StartPositionX(GUI_TypeDef* GUI)
{
  return GUI->Start.X;
}
uint16_t GUI_get_StartPositionY(GUI_TypeDef* GUI)
{
  return GUI->Start.Y;
}
/***************************************************************************************************
 * END: Functions get information about GUI.
 ***************************************************************************************************/




/***************************************************************************************************
 * BEGIN: Functions Limit Coordinates and Sizes to Frame Size.
 ***************************************************************************************************/
/* Private Function */
uint16_t limit_CoordinateOnFrameBorderMin(uint16_t Coordinate)
{
  uint16_t returnValue = Coordinate < 0 ? 0 : Coordinate;
  return returnValue;
}

uint16_t GUI_limit_XOnFrameBorders(GUI_TypeDef* GUI, uint16_t X)
{
  uint16_t tmp_LimitMax = GUI->Width - 1;
  uint16_t tmp_Xnow = limit_CoordinateOnFrameBorderMin(X);

  /* Limit X Max */
  tmp_Xnow = tmp_Xnow > tmp_LimitMax ? tmp_LimitMax : tmp_Xnow;
  return tmp_Xnow;
}

uint16_t GUI_limit_YOnFrameBorders(GUI_TypeDef* GUI, uint16_t Y)
{
  uint16_t tmp_LimitMax = GUI->Height - 1;
  uint16_t tmp_Xnow = limit_CoordinateOnFrameBorderMin(Y);

  /* Limit Y Max*/
  tmp_Xnow = tmp_Xnow > tmp_LimitMax ? tmp_LimitMax : tmp_Xnow;
  return tmp_Xnow;
}

/* Private Function */
uint16_t limit_LengthOnFrameBorderMin(uint16_t Coordinate, uint16_t Length)
{
  uint16_t returnValue = Coordinate < 0 ? Length + Coordinate : Length;
  return returnValue;
}

/* Private Function */
uint16_t limit_LengthInFrameBorderMax(uint16_t LimitedCoordinate, uint16_t Length, uint16_t Limit)
{
  uint16_t returnValue = LimitedCoordinate + Length < Limit ? Length : Limit - LimitedCoordinate;
  return returnValue;
}

uint16_t GUI_limit_WidthOnFrameBorders(GUI_TypeDef* GUI, uint16_t X, uint16_t Width)
{
  uint16_t tmp_XNow = GUI_limit_XOnFrameBorders(GUI, X);
  uint16_t tmp_WidthNow = limit_LengthOnFrameBorderMin(X, Width);
  tmp_WidthNow = limit_LengthInFrameBorderMax(tmp_XNow, tmp_WidthNow, GUI->Width);
  return tmp_WidthNow;
}

uint16_t GUI_limit_HeightOnFrameBorders(GUI_TypeDef* GUI, uint16_t Y, uint16_t Height)
{
  uint16_t tmp_YNow = GUI_limit_YOnFrameBorders(GUI, Y);
  uint16_t tmp_HeightNow = limit_LengthOnFrameBorderMin(Y, Height);
  tmp_HeightNow = limit_LengthInFrameBorderMax(tmp_YNow, tmp_HeightNow, GUI->Height);
  return tmp_HeightNow;
}

uint16_t GUI_get_WidthAsMultipleOf8(uint16_t X, uint16_t Width)
{
  uint16_t tmp_NewWidth;
  tmp_NewWidth = Width + X % 8;
  if(tmp_NewWidth % 8 > 0)
  {
    tmp_NewWidth = tmp_NewWidth + 8 - tmp_NewWidth % 8;
  }
  return tmp_NewWidth;
}
uint16_t GUI_get_XAsMultipleOf8(uint16_t X)
{
  uint16_t tmp_XNew = X - X % 8;
  return tmp_XNew;
}
/***************************************************************************************************
 * END: Functions Limit Coordinates and Sizes to Frame Size.
 ***************************************************************************************************/
