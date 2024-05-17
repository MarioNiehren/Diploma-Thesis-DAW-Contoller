/**
 * @defgroup	EPD_Buffer	Buffer
 * @brief			This group is used to buffer data and frames for the \ref EPaperSSD1681 "E-Paper" module.
 *
 * @file			Buffer_Displays.h
 * @date			Oct 19, 2023
 * @author		Mario
 *
 * @addtogroup		EPD_Source
 * @{
 *
 * @addtogroup		EPD_Buffer
 * @{
 */

#ifndef BUFFER_DISPLAYS_H_
#define BUFFER_DISPLAYS_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
  BUFFER_EMPTY = 0x00,
  BUFFER_COMMAND = 0x01,
  BUFFER_DATA = 0x02,
  BUFFER_FRAME = 0x03,
  BUFFER_LUT = 0x04,
}Buffer_Transmission_TypeDef;


typedef struct
{
  uint16_t NumBytes;
  Buffer_Transmission_TypeDef Type;
  uint16_t BufferStartPos;
}Buffer_Descriptor_TypeDef;

typedef struct
{
  uint8_t Data;
  uint8_t Command;
  uint8_t Frame;
  uint8_t LUT;
}Buffer_Counter_TypeDef;

typedef struct
{
  uint8_t* Frame;
  uint8_t* LUT;
  uint8_t* Data;
  uint8_t* Command;
  Buffer_Descriptor_TypeDef* Descriptor;
  uint8_t LengthDescriptor;
  uint8_t DescriptorIndex;
  uint8_t SendDescriptorIndex;
  Buffer_Counter_TypeDef NumBytes;
  bool empty;
}Buffer_StructTd;

/***************************************************************************************************
 * BEGIN: Functions initialize.
 ***************************************************************************************************/
void Buffer_init_Command(Buffer_StructTd* Buffer, uint8_t* Command);
void Buffer_init_Data(Buffer_StructTd* Buffer, uint8_t* Data);
void Buffer_init_Frame(Buffer_StructTd* Buffer, uint8_t* Frame);
void Buffer_init_LUT(Buffer_StructTd* Buffer, uint8_t* LUT);
void Buffer_init_Desctriptor(Buffer_StructTd* Buffer, Buffer_Descriptor_TypeDef* Descriptor, uint8_t Length);
/***************************************************************************************************
 * END: Functions initialize.
 ***************************************************************************************************/

/***************************************************************************************************
 * BEGIN: Functions to describe Buffer.
 ***************************************************************************************************/
void Buffer_update_Descriptor(Buffer_StructTd* Buffer, uint16_t NumBytes, uint32_t BufferPos, Buffer_Transmission_TypeDef Type);
void Buffer_flush_Descriptor(Buffer_StructTd* Buffer);
/***************************************************************************************************
 * END: Functions to describe Buffer.
 ***************************************************************************************************/

/***************************************************************************************************
 * BEGIN: Functions to write to Buffer.
 ***************************************************************************************************/
void Buffer_write_Frame(Buffer_StructTd* Buffer, uint8_t* Data, uint32_t Size);
void Buffer_write_LUT(Buffer_StructTd* Buffer, uint8_t* Data, uint32_t Size);
void Buffer_write_Data(Buffer_StructTd* Buffer, uint8_t Data);
void Buffer_write_Command(Buffer_StructTd* Buffer, uint8_t Command);
/***************************************************************************************************
 * END: Functions to write to Buffer.
 ***************************************************************************************************/

/***************************************************************************************************
 * BEGIN: Functions to update Values.
 ***************************************************************************************************/
void Buffer_countUp_SendDescriptorIndex(Buffer_StructTd* Buffer);
/***************************************************************************************************
 * END: Functions to update Values.
 ***************************************************************************************************/

/***************************************************************************************************
 * BEGIN: Functions get informations about Buffer.
 ***************************************************************************************************/
Buffer_Transmission_TypeDef Buffer_get_DescriptorTypeNow(Buffer_StructTd* Buffer);
uint16_t Buffer_get_DescriptorNumBytesNow(Buffer_StructTd* Buffer);
uint16_t Buffer_get_DescriptorStartPositionNow(Buffer_StructTd* Buffer);
uint8_t* Buffer_get_StartPointer(Buffer_StructTd* Buffer, uint8_t* BufferLocal);
/***************************************************************************************************
 * END: Functions get informations about Buffer.
 ***************************************************************************************************/
/**@}*//* end of "EPD_Buffer" */
/**@}*//* end of "EPaperSSD1681" */
#endif /* BUFFER_DISPLAYS_H_ */
