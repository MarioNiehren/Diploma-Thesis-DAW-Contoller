/***************************************************************************//**
 * @defgroup        MIDI_UART_Source    Source
 * @brief						Study this part for details about this module.
 *
 * @addtogroup			MIDI_UART
 * @{
 *
 * @addtogroup      MIDI_UART_Source
 * @{
 *
 * @file            MIDI_UART.c
 *
 * @date            Aug 7, 2024
 * @author          Mario
 ******************************************************************************/

#include <MIDI_UART.h>

/***************************************************************************//**
 * @name      Error
 * @brief     Use these functions for error handling.
 * @{
 ******************************************************************************/

/**
 * @brief     This function can be used for a quick pointer check. It returns
 *            an error code if the pointer is NULL.
 * @param     ptr   Pointer to be checked
 * @param     ErrorCode   MIDI_error_Td type code, that will be returned if
 *                        pointer is NULL
 * @return    Error (MIDI_ERROR_NONE if pointer is not null)
 */
MIDI_error_Td errorcheck_PointerIsNull(void* ptr, MIDI_error_Td ErrorCode)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;
  if(ptr == NULL)
  {
    Error = ErrorCode;
  }
  else
  {
    Error = MIDI_ERROR_NONE;
  }
  return Error;
}

/**
 * @brief     This function is an endless loop, to trap the program if an
 *            error occurs.
 * @param     ErrorCode   MIDI_error_Td type code
 * @return    none
 */
void errorcheck_ERROR(MIDI_error_Td ErrorCode)
{
  while(1)
  {
    /* Check Error Code to find the problem */;
  }
}

/**
 * @brief     This function validates the bytes that should be used to build
 *            a standard MIDI command.
 * @param     StatusByte  0x80 - 0xFF
 * @param     DataByte 1      0x00 - 0x7F
 * @param     DataByte 2      0x00 - 0x7F
 * @return    MIDI_ERROR_NONE if all bytes are valid
 */
MIDI_error_Td errorcheck_ValidateMIDIBytes(uint8_t StatusByte, uint8_t DataByte1, uint8_t DataByte2)
{
  MIDI_error_Td Error;

  uint8_t StatusByteLimit = 0x80;
  uint8_t DataByteLimit = 0x7F;

  if(StatusByte < StatusByteLimit)
  {
    Error = MIDI_ERROR_INVALID_STATUS_BYTE;
  }
  else if(DataByte1 > DataByteLimit)
  {
    Error = MIDI_ERROR_INVALID_BYTE_1;
  }
  else if(DataByte2 > DataByteLimit)
  {
    Error = MIDI_ERROR_INVALID_BYTE_2;
  }
  else
  {
    Error = MIDI_ERROR_NONE;
  }

  return Error;
}
/** @} ************************************************************************/
/* end of name "Error"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Buffer
 * @brief     This function are used for internal buffer management
 * @{
 ******************************************************************************/
/**
 * @brief     Get the Buffer Array, that is ready to send. The returned array
 *            will be locked until this function gets called again.
 * @warning   Call this function exactly once for each transmission cycle.
 *            Otherwise the wrong array will be locked.
 * @param     Buffer      pointer to the users Buffer
 * @return    Start pointer of the buffer to be send, or NULL in case of an
 *            error
 */
uint8_t* Buffer_get_TxStartPtrForTransmission(MIDI_buffer_structTd* Buffer)
{
  uint8_t* ReturnPtr;
  MIDI_buffer_Td LockedBuffer = Buffer->LockedToSend;

  if(LockedBuffer == MIDI_BUFFER_TX_A)
  {
    ReturnPtr = Buffer->TxB;
    Buffer->LockedToSend = MIDI_BUFFER_TX_B;
  }
  else if(LockedBuffer == MIDI_BUFFER_TX_B || LockedBuffer == MIDI_BUFFER_NONE)
  {
    ReturnPtr = Buffer->TxA;
    Buffer->LockedToSend = MIDI_BUFFER_TX_A;
  }
  else
  {
    ReturnPtr = NULL;
  }

  return ReturnPtr;
}

/**
 * @brief     Get the length of the Array to be send.
 * @warning   This function must be called after
 *            Buffer_get_TxStartPtrForTransmission(), otherwise it will return
 *            the size of the wrong buffer.
 *            This function can only be called once for each cycle because the
 *            index counter will be reset.
 * @param     Buffer      pointer to the users Buffer
 * @return    size of the buffer that will be send. 0 if the buffer is empty or
 *            in case of an error.
 */
uint16_t Buffer_get_TxSize(MIDI_buffer_structTd* Buffer)
{
  uint8_t size;
  MIDI_buffer_Td LockedBuffer = Buffer->LockedToSend;
  if(LockedBuffer == MIDI_BUFFER_TX_A)
  {
    size = Buffer->TxAIndex;
    Buffer->TxAIndex = 0;
  }
  else if(LockedBuffer == MIDI_BUFFER_TX_B)
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

/**
 * @brief     Use this function to queue data to the buffer that is currently
 *            not locked.
 * @param     Buffer      pointer to the users Buffer
 * @param     Data        pointer to the data to be buffered
 * @param     Size        Number of bytes to be buffered
 */
MIDI_buffer_error_Td Buffer_queue_BytesForTransmission(MIDI_buffer_structTd* Buffer, uint8_t* Data, uint8_t Size)
{
  MIDI_buffer_error_Td Error = MIDI_BUFFER_ERROR_NONE;
  MIDI_buffer_Td LockedBuffer = Buffer->LockedToSend;

  if(LockedBuffer == MIDI_BUFFER_TX_A)
  {
    uint16_t IndexTxA = Buffer->TxAIndex;
    uint16_t TotalSize = IndexTxA + Size;

    if(TotalSize <=  MIDI_UART_MAX_BUFFER)
    {
      for(uint16_t i = 0; i < Size; i++)
      {
        uint16_t ValidIndex = IndexTxA + i;
        Buffer->TxA[ValidIndex] = Data[i];
      }
      Buffer->TxAIndex = IndexTxA + Size;
    }
    else
    {
      Error = MIDI_BUFFER_ERROR_TX_A_OVERFLOW;
    }
  }
  else if(LockedBuffer == MIDI_BUFFER_TX_A)
  {
    uint16_t IndexTxB = Buffer->TxBIndex;
    uint16_t TotalSize = IndexTxB + Size;

    if(TotalSize <=  MIDI_UART_MAX_BUFFER)
    {
      for(uint16_t i = 0; i < Size; i++)
      {
        uint16_t ValidIndex = IndexTxB + i;
        Buffer->TxB[ValidIndex] = Data[i];
      }
      Buffer->TxBIndex = IndexTxB + Size;
    }
    else
    {
      Error = MIDI_BUFFER_ERROR_TX_A_OVERFLOW;
    }
  }
  else
  {
    Error = MIDI_BUFFER_ERROR_NO_BUFFER_FOUND;
  }

  return Error;
}

/** @} ************************************************************************/
/* end of name "Buffer"
 ******************************************************************************/

/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize this module.
 * @{
 ******************************************************************************/

/* Description in .h */
MIDI_error_Td MIDI_init_UART(MIDI_structTd* MIDIPort, UART_HandleTypeDef* huart)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;
  Error = errorcheck_PointerIsNull(huart, MIDI_ERROR_INVALID_UART_HANDLE);

  MIDIPort->huart = huart;

  return Error;
}

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Process
 * @brief     Use these functions to process this module
 * @{
 ******************************************************************************/

/* Description in .h */
MIDI_error_Td MIDI_start_Transmission(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  MIDIPort->RxComplete = false;
  MIDIPort->TxComplete = true;

  MIDIPort->Buffer.RxAIndex = 0;
  MIDIPort->Buffer.RxBIndex = 0;
  MIDIPort->Buffer.TxAIndex = 0;
  MIDIPort->Buffer.TxBIndex = 0;

  MIDIPort->Buffer.LockedToSend = MIDI_BUFFER_NONE;
  MIDIPort->Buffer.LockedToReceive = MIDI_BUFFER_NONE;

  UART_HandleTypeDef* huart = MIDIPort->huart;
  Error = errorcheck_PointerIsNull(huart, MIDI_ERROR_UART_NOT_INITIALIZED);
  if(Error != MIDI_ERROR_NONE)
  {
    errorcheck_ERROR(Error);
  }

  Error =  MIDI_update_Transmission(MIDIPort);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_update_Transmission(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;
  MIDI_structTd* Port = MIDIPort;

  bool TxComplete = Port->TxComplete;

  if(TxComplete == true)
  {
    UART_HandleTypeDef* huart = MIDIPort->huart;
    Error = errorcheck_PointerIsNull(huart, MIDI_ERROR_UART_NOT_INITIALIZED);
    if(Error != MIDI_ERROR_NONE)
    {
      errorcheck_ERROR(Error);
    }

    uint8_t* TxData = Buffer_get_TxStartPtrForTransmission(&MIDIPort->Buffer);
    Error = errorcheck_PointerIsNull(TxData, MIDI_ERROR_BUFFER_TX_NULL);
    if(Error != MIDI_ERROR_NONE)
    {
      errorcheck_ERROR(Error);
    }

    uint16_t size = Buffer_get_TxSize(&MIDIPort->Buffer);

    if(size > 0)
    {
      HAL_UART_Transmit_DMA(huart, TxData, size);
      MIDIPort->TxComplete = false;
    }
  }

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_manage_RxInterrupt(MIDI_structTd* MIDIPort, UART_HandleTypeDef *huart)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;
  UART_HandleTypeDef* huartValid = MIDIPort->huart;
  if(huartValid == huart)
  {

  }
  else
  {
    Error = MIDI_ERROR_INVALID_UART_HANDLE;
  }
  return Error;
}
/* Description in .h */
MIDI_error_Td MIDI_manage_TxInterrupt(MIDI_structTd* MIDIPort, UART_HandleTypeDef *huart)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;
  UART_HandleTypeDef* huartValid = MIDIPort->huart;

  if(huartValid == huart)
  {
    MIDIPort->TxComplete = true;
  }
  else
  {
    Error = MIDI_ERROR_INVALID_UART_HANDLE;
  }
  return Error;
}
/** @} ************************************************************************/
/* end of name "Process"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Interaction
 * @brief     Use these functions to interact with the module
 * @{
 ******************************************************************************/

/**
 * @brief     Helper function to queue MIDI commands in the standard 3 Bytes
 *            format.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     StatusByte  0x80 - 0xFF
 * @param     DataByte 1      0x00 - 0x7F
 * @param     DataByte 2      0x00 - 0x7F
 * @return    MIDI_ERROR_NONE if all bytes are valid
 */
MIDI_error_Td queue_MIDIThreeBytes(MIDI_structTd* MIDIPort, uint8_t StatusByte, uint8_t DataByte1, uint8_t DataByte2)
{
  MIDI_error_Td Error;

  Error = errorcheck_ValidateMIDIBytes(StatusByte, DataByte1, DataByte2);

  if(Error == MIDI_ERROR_NONE)
  {
    uint8_t TxData[MIDI_LEN_STANDARD_COMMAND];
    TxData[0] = StatusByte;
    TxData[1] = DataByte1;
    TxData[2] = DataByte2;

    MIDI_buffer_structTd* Buffer = &MIDIPort->Buffer;
    Error = Buffer_queue_BytesForTransmission(Buffer, TxData, MIDI_LEN_STANDARD_COMMAND);
  }
  else
  {
    ;
  }
  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_NoteOff(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number, uint8_t Velocity)
{
  MIDI_error_Td Error = MIDI_BUFFER_NONE;

  uint8_t StatusByte = MIDI_STATUS_NOTE_OFF | Channel;
  Error = queue_MIDIThreeBytes(MIDIPort, StatusByte, Number, Velocity);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_NoteOn(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number, uint8_t Velocity)
{
  MIDI_error_Td Error = MIDI_BUFFER_NONE;

  uint8_t StatusByte = MIDI_STATUS_NOTE_ON | Channel;
  Error = queue_MIDIThreeBytes(MIDIPort, StatusByte, Number, Velocity);

  return Error;
}
/** @} ************************************************************************/
/* end of name "Interaction"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Get Functions
 * @brief     Use these functions to get values
 * @{
 ******************************************************************************/

/** @} ************************************************************************/
/* end of name "Get Functions"
 ******************************************************************************/

/**@}*//* end of defgroup "MIDI_UART_Source" */
/**@}*//* end of defgroup "MIDI_UART" */
