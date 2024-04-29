/*
 * Fonts.h
 *
 *  Created on: Oct 18, 2023
 *      Author: Mario Niehren
 *
 * Requirements for fonts used with GUI.h:
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

#ifndef MN_FONTS_H_INC
#define MN_FONTS_H_INC

#include <stdint.h>

typedef enum
{
  FONTS_CONSTANT_WIDTH,
  FONTS_VARIABLE_WIDTH
}Fonts_Widthtype_TypeDef;

typedef struct
{
  uint8_t Width;
  int16_t BitmapIndex;
}Fonts_Descriptor_TypeDef;

typedef struct
{    
  const uint8_t* Bitmap;
  Fonts_Descriptor_TypeDef* Descriptor;
  uint16_t Height;
  Fonts_Widthtype_TypeDef Widthtype;
}Fonts_TypeDef;

/***************************************************************************************************
 * BEGIN: Segoe Script
 ***************************************************************************************************/
extern Fonts_TypeDef SegoeScript31px;

/***************************************************************************************************
 * BEGIN: Corbel
 ***************************************************************************************************/
extern Fonts_TypeDef Corbel20px;
extern Fonts_TypeDef Corbel22px;
  
#endif /* MN_FONTS_H_INC */

