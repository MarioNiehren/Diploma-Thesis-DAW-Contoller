/*
 * Buffer_Displays.c
 *
 *  Created on: Oct 19, 2023
 *      Author: Mario Niehren
 */

#include "Buffer_Displays.h"

/***************************************************************************************************
 * BEGIN: Functions initialize.
 ***************************************************************************************************/
void Buffer_init_Command(Buffer_StructTd* Buffer, uint8_t* Command)
{
  Buffer->Command = Command;
}
void Buffer_init_Data(Buffer_StructTd* Buffer, uint8_t* Data)
{
  Buffer->Data = Data;
}

void Buffer_init_Frame(Buffer_StructTd* Buffer, uint8_t* Frame)
{
  Buffer->Frame = Frame;
}

void Buffer_init_LUT(Buffer_StructTd* Buffer, uint8_t* LUT)
{
  Buffer->LUT = LUT;
}

void Buffer_init_Desctriptor(Buffer_StructTd* Buffer, Buffer_Descriptor_TypeDef* Descriptor, uint8_t Length)
{
  Buffer->Descriptor = Descriptor;
  Buffer->LengthDescriptor = Length;
}
/***************************************************************************************************
 * END: Functions initialize.
 ***************************************************************************************************/




/***************************************************************************************************
 * BEGIN: Functions to describe Buffer.
 ***************************************************************************************************/
void Buffer_update_Descriptor(Buffer_StructTd* Buffer, uint16_t NumBytes, uint32_t BufferPos, Buffer_Transmission_TypeDef Type)
{
  Buffer->Descriptor[Buffer->DescriptorIndex].NumBytes = NumBytes;
  Buffer->Descriptor[Buffer->DescriptorIndex].Type = Type;
  Buffer->Descriptor[Buffer->DescriptorIndex].BufferStartPos = BufferPos;
  Buffer->DescriptorIndex++;
}

void Buffer_flush_Descriptor(Buffer_StructTd* Buffer)
{
  for(uint8_t i = 0x00; i < Buffer->LengthDescriptor; i++)
  {
    Buffer->Descriptor[i].BufferStartPos = 0x00;
    Buffer->Descriptor[i].NumBytes = 0x00;
    Buffer->Descriptor[i].Type = BUFFER_EMPTY;
  }
  Buffer->DescriptorIndex = 0x00;
  Buffer->SendDescriptorIndex = 0x00;
  Buffer->NumBytes.Data = 0x00;
  Buffer->NumBytes.Command = 0x00;
  Buffer->NumBytes.Frame = 0x00;
  Buffer->NumBytes.LUT = 0x00;
  Buffer->empty = true;
}
/***************************************************************************************************
 * END: Functions to describe Buffer.
 ***************************************************************************************************/




/***************************************************************************************************
 * BEGIN: Functions to write to Buffer.
 ***************************************************************************************************/
void Buffer_write_Frame(Buffer_StructTd* Buffer, uint8_t* Data, uint32_t Size)
{
  uint32_t tmp_BufferStartPos = Buffer->NumBytes.Frame;
  for(uint32_t i = 0; i < Size; i++)
  {
    Buffer->Frame[Buffer->NumBytes.Frame] = Data[i];
    Buffer->NumBytes.Frame++;
  }
  Buffer->empty = false;
  Buffer_update_Descriptor(Buffer, Size, tmp_BufferStartPos, BUFFER_FRAME);
}

void Buffer_write_LUT(Buffer_StructTd* Buffer, uint8_t* Data, uint32_t Size)
{
  uint32_t tmp_BufferStartPos = Buffer->NumBytes.LUT;
  for(uint32_t i = 0; i < Size; i++)
  {
    Buffer->LUT[Buffer->NumBytes.LUT] = Data[i];
    Buffer->NumBytes.LUT++;
  }
  Buffer->empty = false;
  Buffer_update_Descriptor(Buffer, Size, tmp_BufferStartPos, BUFFER_LUT);
}

void Buffer_write_Data(Buffer_StructTd* Buffer, uint8_t Data)
{
  uint32_t tmp_BufferStartPos = Buffer->NumBytes.Data;
  Buffer->Data[Buffer->NumBytes.Data] = Data;
  Buffer->NumBytes.Data++;
  Buffer->empty = false;
  Buffer_update_Descriptor(Buffer, 1, tmp_BufferStartPos, BUFFER_DATA);
}

void Buffer_write_Command(Buffer_StructTd* Buffer, uint8_t Command)
{
  uint32_t tmp_BufferStartPos = Buffer->NumBytes.Command;
  Buffer->Command[Buffer->NumBytes.Command] = Command;
  Buffer->NumBytes.Command++;
  Buffer->empty = false;
  Buffer_update_Descriptor(Buffer, 1, tmp_BufferStartPos, BUFFER_COMMAND);
}
/***************************************************************************************************
 * END: Functions to write to Buffer.
 ***************************************************************************************************/




/***************************************************************************************************
 * BEGIN: Functions to update Values.
 ***************************************************************************************************/
void Buffer_countUp_SendDescriptorIndex(Buffer_StructTd* Buffer)
{
  Buffer->SendDescriptorIndex++;
}
/***************************************************************************************************
 * END: Functions to update Values.
 ***************************************************************************************************/




/***************************************************************************************************
 * BEGIN: Functions get informations about Buffer.
 ***************************************************************************************************/
Buffer_Transmission_TypeDef Buffer_get_DescriptorTypeNow(Buffer_StructTd* Buffer)
{
  return Buffer->Descriptor[Buffer->SendDescriptorIndex].Type;
}

uint16_t Buffer_get_DescriptorNumBytesNow(Buffer_StructTd* Buffer)
{
  return Buffer->Descriptor[Buffer->SendDescriptorIndex].NumBytes;
}

uint16_t Buffer_get_DescriptorStartPositionNow(Buffer_StructTd* Buffer)
{
  return Buffer->Descriptor[Buffer->SendDescriptorIndex].BufferStartPos;
}

uint8_t* Buffer_get_StartPointer(Buffer_StructTd* Buffer, uint8_t* BufferLocal)
{
  uint16_t tmp_StartPos = Buffer_get_DescriptorStartPositionNow(Buffer);
  return &BufferLocal[tmp_StartPos];
}
/***************************************************************************************************
 * END: Functions get informations about Buffer.
 ***************************************************************************************************/



