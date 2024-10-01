/***************************************************************************//**
 * @defgroup        Buffer_PingPong_Source    Source
 * @brief						Study this part for details about this module.
 *
 * @addtogroup			Buffer_PingPong
 * @{
 *
 * @addtogroup      Buffer_PingPong_Source
 * @{
 *
 * @file            Buffer_PingPong.c
 *
 * @date            Aug 12, 2024
 * @author          Mario
 ******************************************************************************/

#include <Buffer_PingPong.h>

/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize this module.
 * @{
 ******************************************************************************/

/* Description in .h */
BufferPingPong_error_Td BufferPingPong_init_StartConditions(BufferPingPong_structTd* Buffer)
{
  BufferPingPong_error_Td Error;

  if(BUFFER_PINGPONG_RX_MAX > (UINT16_MAX - BUFFER_PING_PONG_ERROR_VALUE_RESERVE))
  {
    Error = BUFFER_PINGPONG_ERROR_RX_MAX_TOO_HIGH;
  }
  else if(BUFFER_PINGPONG_TX_MAX > (UINT16_MAX - BUFFER_PING_PONG_ERROR_VALUE_RESERVE))
  {
    Error = BUFFER_PINGPONG_ERROR_TX_MAX_TOO_HIGH;
  }
  else if(BUFFER_PINGPONG_RX_HEADROOM > (UINT16_MAX - BUFFER_PING_PONG_ERROR_VALUE_RESERVE))
  {
    Error = BUFFER_PINGPONG_ERROR_RX_HEADROOM_TOO_HIGH;
  }
  else
  {
    Error = BUFFER_PINGPONG_ERROR_NONE;
  }

  Buffer->ReservedToReceive = BUFFER_PINGPONG_RX_A;
  Buffer->ReservedToSend = BUFFER_PINGPONG_NONE;

  Buffer->TxAIndex = 0;
  Buffer->TxBIndex = 0;
  Buffer->RxAIndex = 0;
  Buffer->RxBIndex = 0;
  Buffer->RxHeadroomIndex = 0;

  return Error;
}
/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Rx Buffers
 * @brief     This function are used to handle the Rx Buffers
 * @{
 ******************************************************************************/

/* Description in .h */
uint8_t* BufferPingPong_get_RxStartPtrToReceiveData(BufferPingPong_structTd* Buffer)
{
  uint8_t* ReturnPtr;
  BufferPingPong_Td RxBuffer = Buffer->ReservedToReceive;
  uint16_t Index;

  if(RxBuffer == BUFFER_PINGPONG_RX_A)
  {
    Index = Buffer->RxAIndex;
    ReturnPtr = &Buffer->RxA[Index];
  }
  else if(RxBuffer == BUFFER_PINGPONG_RX_B)
  {
    Index = Buffer->RxBIndex;
    ReturnPtr = &Buffer->RxB[Index];
  }
  else
  {
    ReturnPtr = NULL;
  }

  return ReturnPtr;
}

/* Description in .h */
BufferPingPong_error_Td BufferPingPong_toggle_RxBuffer(BufferPingPong_structTd* Buffer)
{
  BufferPingPong_error_Td Error;
  BufferPingPong_Td ReservedBuffer = Buffer->ReservedToReceive;

  if(ReservedBuffer == BUFFER_PINGPONG_RX_A)
  {
    Buffer->ReservedToReceive = BUFFER_PINGPONG_RX_B;
    Buffer->RxBIndex = 0;
    Error = BUFFER_PINGPONG_ERROR_NONE;

  }
  else if(ReservedBuffer == BUFFER_PINGPONG_RX_B)
  {
    Buffer->ReservedToReceive = BUFFER_PINGPONG_RX_A;
    Buffer->RxAIndex = 0;
    Error = BUFFER_PINGPONG_ERROR_NONE;
  }
  else
  {
    Error = BUFFER_PINGPONG_ERROR_TOGGLE_RX_FAILED;
  }

  return Error;
}

/* Description in .h */
uint16_t BufferPingPong_get_SizeOfLockedRxBuffer(BufferPingPong_structTd* Buffer)
{
  uint16_t BufferSize;
  BufferPingPong_Td LockedBuffer = Buffer->ReservedToReceive;

  if(LockedBuffer == BUFFER_PINGPONG_RX_A)
  {
    BufferSize = Buffer->RxAIndex;
  }
  else if(LockedBuffer == BUFFER_PINGPONG_TX_B)
  {
    BufferSize = Buffer->RxBIndex;
  }
  else
  {
    BufferSize = BUFFER_PINGPONG_ERROR_16BIT_RANGE_OVERFLOW;
  }

  return BufferSize;
}

/* Description in .h */
uint8_t* ButterPingPong_get_StartPtrOfLockedRxBuffer(BufferPingPong_structTd* Buffer)
{
  uint8_t* StartPtr;
  BufferPingPong_Td LockedBuffer = Buffer->ReservedToReceive;

  if(LockedBuffer == BUFFER_PINGPONG_RX_A)
  {
    StartPtr = Buffer->RxA;
  }
  else if(LockedBuffer == BUFFER_PINGPONG_TX_B)
  {
    StartPtr = Buffer->RxB;
  }
  else
  {
    StartPtr = NULL;
  }

  return StartPtr;
}

/** @cond *//* Function Prototypes */
BufferPingPong_error_Td queue_BytesToRxBufferA(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size);
BufferPingPong_error_Td queue_BytesToRxBufferB(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size);
/** @endcond *//* Function Prototypes */

/* Description in .h */
BufferPingPong_error_Td BufferPingPong_queue_RxBytes(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size)
{
  BufferPingPong_error_Td Error = BUFFER_PINGPONG_ERROR_NONE;
  BufferPingPong_Td ReservedBuffer = Buffer->ReservedToReceive;

  if(ReservedBuffer == BUFFER_PINGPONG_RX_A)
  {
    Error = queue_BytesToRxBufferA(Buffer, Data, Size);
  }
  else if(ReservedBuffer == BUFFER_PINGPONG_RX_B)
  {
    Error = queue_BytesToRxBufferB(Buffer, Data, Size);
  }
  else
  {
    Error = BUFFER_PINGPONG_ERROR_NO_BUFFER_FOUND;
  }

  return Error;
}

/**
 * @brief     Queue bytes to the RxA Buffer.
 * @param     Buffer      pointer to the users Buffer
 * @param     Data        pointer to the data to be buffered
 * @param     Size        Number of bytes to be buffered
 * @return    MIDI_BUFFER_ERROR_NONE if everything is fine
 */
BufferPingPong_error_Td queue_BytesToRxBufferA(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size)
{
  BufferPingPong_error_Td Error;

  uint16_t IndexRxA = Buffer->RxAIndex;
  uint16_t TotalSize = IndexRxA + Size;

  if(TotalSize <=  BUFFER_PINGPONG_RX_MAX)
  {
    Error = BUFFER_PINGPONG_ERROR_NONE;

    for(uint16_t i = 0; i < Size; i++)
    {
      uint16_t ValidIndex = IndexRxA + i;
      Buffer->RxA[ValidIndex] = Data[i];
    }
    Buffer->RxAIndex = IndexRxA + Size;
  }
  else
  {
    Error = BUFFER_PINGPONG_ERROR_RX_A_OVERFLOW;
  }

  return Error;
}
/**
 * @brief     Queue bytes to the RxB Buffer.
 * @param     Buffer      pointer to the users Buffer
 * @param     Data        pointer to the data to be buffered
 * @param     Size        Number of bytes to be buffered
 * @return    MIDI_BUFFER_ERROR_NONE if everything is fine
 */
BufferPingPong_error_Td queue_BytesToRxBufferB(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size)
{
  BufferPingPong_error_Td Error;

  uint16_t IndexRxB = Buffer->RxBIndex;
  uint16_t TotalSize = IndexRxB + Size;

  if(TotalSize <=  BUFFER_PINGPONG_RX_MAX)
  {
    Error = BUFFER_PINGPONG_ERROR_NONE;

    for(uint16_t i = 0; i < Size; i++)
    {
      uint16_t ValidIndex = IndexRxB + i;
      Buffer->RxB[ValidIndex] = Data[i];
    }
    Buffer->RxBIndex = IndexRxB + Size;
  }
  else
  {
    Error = BUFFER_PINGPONG_ERROR_RX_B_OVERFLOW;
  }

  return Error;
}

/* Description in .h */
BufferPingPong_error_Td BufferPingPong_increase_RxBufferIndex(BufferPingPong_structTd* Buffer, uint16_t size)
{
  BufferPingPong_error_Td Error;
  BufferPingPong_Td LockedBuffer = Buffer->ReservedToReceive;


  if(LockedBuffer == BUFFER_PINGPONG_RX_A)
  {
    uint16_t PrevIndex = Buffer->RxAIndex;
    uint16_t NewIndex = PrevIndex + size;
    uint16_t OverflowLimit = UINT16_MAX - BUFFER_PING_PONG_ERROR_VALUE_RESERVE;

    if(NewIndex > OverflowLimit)
    {
      Error = BUFFER_PINGPONG_ERROR_RX_A_OVERFLOW;
    }
    else
    {
      Buffer->RxAIndex = NewIndex;
      Error = BUFFER_PINGPONG_ERROR_NONE;
    }
  }
  else if(LockedBuffer == BUFFER_PINGPONG_TX_B)
  {
    uint16_t PrevIndex = Buffer->RxBIndex;
    uint16_t NewIndex = PrevIndex + size;
    uint16_t OverflowLimit = UINT16_MAX - BUFFER_PING_PONG_ERROR_VALUE_RESERVE;

    if(NewIndex > OverflowLimit)
    {
      Error = BUFFER_PINGPONG_ERROR_RX_B_OVERFLOW;
    }
    else
    {
      Buffer->RxBIndex = NewIndex;
      Error = BUFFER_PINGPONG_ERROR_NONE;
    }
  }
  else
  {
    Error = BUFFER_PINGPONG_ERROR_NO_BUFFER_FOUND;
  }

  return Error;
}

/* Description in .h */
/* Maybe deprecated */
uint16_t BufferPingPong_queue_RxByteToHeadroom(BufferPingPong_structTd* Buffer, uint8_t Data)
{
  uint16_t Index = Buffer->RxHeadroomIndex;
  uint16_t BufferedBytes = Index + 1;

  if(BufferedBytes > BUFFER_PINGPONG_RX_HEADROOM)
  {
    BufferedBytes = BUFFER_PINGPONG_OVERFLOW;
    Buffer->RxHeadroomIndex = 0x00;
  }
  else
  {
    Buffer->RxHeadroom[Index] = Data;
    Buffer->RxHeadroomIndex++;
  }

  return BufferedBytes;
}

/* Description in .h */
BufferPingPong_error_Td BufferPingPong_latch_RxHeadroomToBuffer(BufferPingPong_structTd* Buffer)
{
  BufferPingPong_error_Td Error = BUFFER_PINGPONG_ERROR_NONE;

  uint8_t* StartPtrSource = &Buffer->RxHeadroom[0];
  uint8_t* StartPtrDest =  ButterPingPong_get_StartPtrOfLockedRxBuffer(Buffer);
  uint16_t SizeHeadroom = Buffer->RxHeadroomIndex;

  for(uint8_t i = 0; i < SizeHeadroom; i++)
  {
    StartPtrDest[i] = StartPtrSource[i];
  }

  Buffer->RxHeadroomIndex = 0x00;

  return Error;
}

/* Description in .h */
uint8_t* BufferPingPong_get_RxBufferHeadroom(BufferPingPong_structTd* Buffer)
{
  uint8_t* StartPtr = &Buffer->RxHeadroom[0];

  return StartPtr;
}

/* Description in .h */
uint16_t BufferPingPong_get_RxBufferHeadroomSize()
{
  return BUFFER_PINGPONG_RX_HEADROOM;
}

/* Description in .h */
BufferPingPong_error_Td BufferPingPong_save_NumReceivedHeadroomBytes(BufferPingPong_structTd* Buffer, uint16_t Size)
{
  BufferPingPong_error_Td Error = BUFFER_PINGPONG_ERROR_NONE;

  Buffer->RxHeadroomIndex = Buffer->RxHeadroomIndex + Size;

  return Error;
}
/** @} ************************************************************************/
/* end of name "Rx Buffers"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Tx Buffers
 * @brief     This function are used to handle the Tx Buffers
 * @{
 ******************************************************************************/

/* Description in .h */
uint8_t* BufferPingPong_get_TxStartPtrForTransmission(BufferPingPong_structTd* Buffer)
{
  uint8_t* ReturnPtr;
  BufferPingPong_Td LockedBuffer = Buffer->ReservedToSend;

  if(LockedBuffer == BUFFER_PINGPONG_TX_A)
  {
    ReturnPtr = Buffer->TxB;
  }
  else if(LockedBuffer == BUFFER_PINGPONG_TX_B || LockedBuffer == BUFFER_PINGPONG_NONE)
  {
    ReturnPtr = Buffer->TxA;
  }
  else
  {
    ReturnPtr = NULL;
  }

  return ReturnPtr;
}

/* Description in .h */
BufferPingPong_error_Td BufferPingPong_toggle_TxBuffer(BufferPingPong_structTd* Buffer)
{
  BufferPingPong_error_Td Error;
  BufferPingPong_Td LockedBuffer = Buffer->ReservedToSend;

  if(LockedBuffer == BUFFER_PINGPONG_TX_A)
  {
    Buffer->ReservedToSend = BUFFER_PINGPONG_TX_B;
    Error = BUFFER_PINGPONG_ERROR_NONE;

  }
  else if(LockedBuffer == BUFFER_PINGPONG_TX_B || LockedBuffer == BUFFER_PINGPONG_NONE)
  {
    Buffer->ReservedToSend = BUFFER_PINGPONG_TX_A;
    Error = BUFFER_PINGPONG_ERROR_NONE;
  }
  else
  {
    Error = BUFFER_PINGPONG_ERROR_TOGGLE_TX_FAILED;
  }

  return Error;
}

/* Description in .h */
uint16_t BufferPingPong_get_TxSizeForTransmission(BufferPingPong_structTd* Buffer)
{
  uint8_t size;
  BufferPingPong_Td LockedBuffer = Buffer->ReservedToSend;
  if(LockedBuffer == BUFFER_PINGPONG_TX_A)
  {
    size = Buffer->TxAIndex;
    Buffer->TxAIndex = 0;
  }
  else if(LockedBuffer == BUFFER_PINGPONG_TX_B)
  {
    size = Buffer->TxBIndex;
    Buffer->TxBIndex = 0;
  }
  else
  {
    size  = 0;
  }

  return size;
}

/** @cond *//* Function Prototypes */
BufferPingPong_error_Td queue_BytesToTxBufferA(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size);
BufferPingPong_error_Td queue_BytesToTxBufferB(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size);
/** @endcond *//* Function Prototypes */

/* Description in .h */
BufferPingPong_error_Td BufferPingPong_queue_TxBytesForTransmission(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size)
{
  BufferPingPong_error_Td Error = BUFFER_PINGPONG_ERROR_NONE;
  BufferPingPong_Td LockedBuffer = Buffer->ReservedToSend;

  if(LockedBuffer == BUFFER_PINGPONG_TX_A)
  {
    Error = queue_BytesToTxBufferB(Buffer, Data, Size);
  }
  else if(LockedBuffer == BUFFER_PINGPONG_TX_B)
  {
    Error = queue_BytesToTxBufferA(Buffer, Data, Size);
  }
  else
  {
    Error = BUFFER_PINGPONG_ERROR_NO_BUFFER_FOUND;
  }

  return Error;
}

/**
 * @brief     Queue bytes to the TxA Buffer.
 * @param     Buffer      pointer to the users Buffer
 * @param     Data        pointer to the data to be buffered
 * @param     Size        Number of bytes to be buffered
 * @return    MIDI_BUFFER_ERROR_NONE if everything is fine
 */
BufferPingPong_error_Td queue_BytesToTxBufferA(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size)
{
  BufferPingPong_error_Td Error;

  uint16_t IndexTxA = Buffer->TxAIndex;
  uint16_t TotalSize = IndexTxA + Size;

  if(TotalSize <=  BUFFER_PINGPONG_TX_MAX)
  {
    Error = BUFFER_PINGPONG_ERROR_NONE;

    for(uint16_t i = 0; i < Size; i++)
    {
      uint16_t ValidIndex = IndexTxA + i;
      Buffer->TxA[ValidIndex] = Data[i];
    }
    Buffer->TxAIndex = IndexTxA + Size;
  }
  else
  {
    Error = BUFFER_PINGPONG_ERROR_TX_A_OVERFLOW;
  }

  return Error;
}
/**
 * @brief     Queue bytes to the TxB Buffer.
 * @param     Buffer      pointer to the users Buffer
 * @param     Data        pointer to the data to be buffered
 * @param     Size        Number of bytes to be buffered
 * @return    MIDI_BUFFER_ERROR_NONE if everything is fine
 */
BufferPingPong_error_Td queue_BytesToTxBufferB(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size)
{
  BufferPingPong_error_Td Error;

  uint16_t IndexTxB = Buffer->TxBIndex;
  uint16_t TotalSize = IndexTxB + Size;

  if(TotalSize <=  BUFFER_PINGPONG_TX_MAX)
  {
    Error = BUFFER_PINGPONG_ERROR_NONE;

    for(uint16_t i = 0; i < Size; i++)
    {
      uint16_t ValidIndex = IndexTxB + i;
      Buffer->TxB[ValidIndex] = Data[i];
    }
    Buffer->TxBIndex = IndexTxB + Size;
  }
  else
  {
    Error = BUFFER_PINGPONG_ERROR_TX_B_OVERFLOW;
  }

  return Error;
}

/** @} ************************************************************************/
/* end of name "Tx Buffers"
 ******************************************************************************/


/**@}*//* end of defgroup "Buffer_PingPong_Source" */
/**@}*//* end of defgroup "Buffer_PingPong" */
