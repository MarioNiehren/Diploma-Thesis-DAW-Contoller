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
#include <string.h>
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

  memset(Buffer->RxA, 0x00, BUFFER_PINGPONG_RX_MAX);
  memset(Buffer->RxB, 0x00, BUFFER_PINGPONG_RX_MAX);
  memset(Buffer->TxA, 0x00, BUFFER_PINGPONG_TX_MAX);
  memset(Buffer->TxB, 0x00, BUFFER_PINGPONG_TX_MAX);
  memset(Buffer->RxHeadroom, 0x00, BUFFER_PINGPONG_RX_HEADROOM);

  return Error;
}
/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Rx Buffer on Application Side
 * @brief     This function are used to access the Buffer in the users
 *            application.
 * @{
 ******************************************************************************/

/* Description in .h */
uint8_t* ButterPingPong_fetch_StartPtrOfFilledRxBuffer(BufferPingPong_structTd* Buffer)
{
  uint8_t* StartPtr;
  BufferPingPong_Td LockedBuffer = Buffer->ReservedToReceive;

  if(LockedBuffer == BUFFER_PINGPONG_RX_A)
  {
    StartPtr = &Buffer->RxA[0];
  }
  else if(LockedBuffer == BUFFER_PINGPONG_RX_B)
  {
    StartPtr = &Buffer->RxB[0];
  }
  else
  {
    StartPtr = NULL;
  }

  return StartPtr;
}

/* Description in .h */
uint16_t BufferPingPong_fetch_SizeOfFilledRxBuffer(BufferPingPong_structTd* Buffer)
{
  uint16_t BufferSize;
  BufferPingPong_Td LockedBuffer = Buffer->ReservedToReceive;

  if(LockedBuffer == BUFFER_PINGPONG_RX_A)
  {
    BufferSize = Buffer->RxAIndex;
  }
  else if(LockedBuffer == BUFFER_PINGPONG_RX_B)
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
BufferPingPong_error_Td BufferPingPong_toggle_RxBuffer(BufferPingPong_structTd* Buffer)
{
  BufferPingPong_error_Td Error;
  BufferPingPong_Td ReservedBuffer = Buffer->ReservedToReceive;

  if(ReservedBuffer == BUFFER_PINGPONG_RX_A)
  {
    Buffer->ReservedToReceive = BUFFER_PINGPONG_RX_B;
    Buffer->RxBIndex = 0;
    Buffer->RxBufferToggled = true;
    Error = BUFFER_PINGPONG_ERROR_NONE;
  }
  else if(ReservedBuffer == BUFFER_PINGPONG_RX_B)
  {
    Buffer->ReservedToReceive = BUFFER_PINGPONG_RX_A;
    Buffer->RxAIndex = 0;
    Buffer->RxBufferToggled = true;
    Error = BUFFER_PINGPONG_ERROR_NONE;
  }
  else
  {
    Error = BUFFER_PINGPONG_ERROR_TOGGLE_RX_FAILED;
  }

  return Error;
}

/** @} ************************************************************************/
/* end of name " Rx Buffer on Application Side"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Rx Buffer on Transmission Side
 * @brief     This function are used to handle the Rx Buffers on transmission
 *            side. This can be anything, that actually receives data bytes and
 *            stores them in an any buffer.
 * @{
 ******************************************************************************/

/** @cond *//* Function Prototypes */
BufferPingPong_error_Td increase_RxBufferIndex(BufferPingPong_structTd* Buffer, uint16_t size);
/** @endcond *//* Function Prototypes */

/* Description in .h */
BufferPingPong_error_Td BufferPingPong_latch_TempRxBufferToRegularRxBuffer(BufferPingPong_structTd* Buffer, uint16_t Size)
{
  BufferPingPong_error_Td Error = BUFFER_PINGPONG_ERROR_NONE;

  uint8_t* StartPtrSource = &Buffer->RxHeadroom[0];
  uint8_t* StartPtrDest =  ButterPingPong_fetch_StartPtrOfFilledRxBuffer(Buffer);
  uint16_t SizeHeadroom = Size;
  uint16_t DestIndexOffset = BufferPingPong_fetch_SizeOfFilledRxBuffer(Buffer);

  for(uint8_t i = 0; i < SizeHeadroom; i++)
  {
    StartPtrDest[DestIndexOffset + i] = StartPtrSource[i];
  }

  increase_RxBufferIndex(Buffer, SizeHeadroom);
  return Error;
}

/* Description in .h */
BufferPingPong_error_Td increase_RxBufferIndex(BufferPingPong_structTd* Buffer, uint16_t size)
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
  else if(LockedBuffer == BUFFER_PINGPONG_RX_B)
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
uint8_t* BufferPingPong_get_StartPtrOfTempRxBuffer(BufferPingPong_structTd* Buffer)
{
  uint8_t* StartPtr = &Buffer->RxHeadroom[0];

  return StartPtr;
}

/* Description in .h */
uint16_t BufferPingPong_get_SizeOfTempRxBuffer()
{
  return BUFFER_PINGPONG_RX_HEADROOM;
}

/** @} ************************************************************************/
/* end of name "Rx Buffer on Transmission Side"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Tx Buffer on Application Side
 * @brief     This function are used to handle the Tx Buffers
 * @{
 ******************************************************************************/

/** @cond *//* Function Prototypes */
BufferPingPong_error_Td queue_BytesToTxBufferA(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size);
BufferPingPong_error_Td queue_BytesToTxBufferB(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size);
/** @endcond *//* Function Prototypes */

/* Description in .h */
BufferPingPong_error_Td BufferPingPong_queue_TxBytesToEmptyBuffer(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size)
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
/* end of name "Tx Buffer on Application Side"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Tx Buffer on Transmission Side
 * @brief     This function are used to handle the Tx Buffers
 * @{
 ******************************************************************************/

/* Description in .h */
uint8_t* BufferPingPong_get_StartPtrOfFilledTxBuffer(BufferPingPong_structTd* Buffer)
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
uint16_t BufferPingPong_get_SizeOfFilledTxBuffer(BufferPingPong_structTd* Buffer)
{
  uint8_t size;
  BufferPingPong_Td LockedBuffer = Buffer->ReservedToSend;
  if(LockedBuffer == BUFFER_PINGPONG_TX_A)
  {
    size = Buffer->TxBIndex;
    Buffer->TxBIndex = 0;
  }
  else if(LockedBuffer == BUFFER_PINGPONG_TX_B)
  {
    size = Buffer->TxAIndex;
    Buffer->TxAIndex = 0;
  }
  else
  {
    size  = 0;
  }

  return size;
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





/** @} ************************************************************************/
/* end of name "Tx Buffers"
 ******************************************************************************/


/**@}*//* end of defgroup "Buffer_PingPong_Source" */
/**@}*//* end of defgroup "Buffer_PingPong" */
