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

typedef enum
{
  MIDI_NUMBYTES_NODATA = 0x01,
  MIDI_NUMBYTES_SHORT_MESSAGE = 0x02,
  MIDI_NUMBYTES_STANDARD_MESSAGE = 0x03,

  MIDI_NUMBYTES_UNFEDINED = 0xFF
}MIDI_internal_MessageNumBytes_Td;
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
 * @brief     Stop the code if the argument is not MIDI_ERROR_NONE.
 * @param     Error   Error to be checked
 * @return    none
 */
void errorcheck_stop_Code(MIDI_error_Td Error)
{
  if(Error != MIDI_ERROR_NONE)
  {
    errorcheck_ERROR(Error);;
  }
  else
  {
    ;
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
MIDI_error_Td errorcheck_validate_MIDIBytes(uint8_t StatusByte, uint8_t DataByte1, uint8_t DataByte2)
{
  MIDI_error_Td Error;

  if(StatusByte < MIDI_STATUS_BYTE_MIN_VALUE)
  {
    Error = MIDI_ERROR_INVALID_STATUS_BYTE;
  }
  else if(DataByte1 >= MIDI_STATUS_BYTE_MIN_VALUE)
  {
    Error = MIDI_ERROR_INVALID_BYTE_1;
  }
  else if(DataByte2 >= MIDI_STATUS_BYTE_MIN_VALUE)
  {
    Error = MIDI_ERROR_INVALID_BYTE_2;
  }
  else
  {
    Error = MIDI_ERROR_NONE;
  }

  return Error;
}

/**
 * @brief     If an external function returns an error that does not match the
 *            expectations, a local error code will be returned.
 * @param     ExternalError     received error code
 * @param     Expectation       expected error code
 * @param     LocalError        ErrorCode to be returned if expectations are not
 *                              achieved
 * @return    Valid local error code. MIDI_ERROR_NONE if all bytes are valid.
 */
MIDI_error_Td errorcheck_validate_ExternalErrorCode(int ExternalError, int Expectation,  MIDI_error_Td LocalError)
{
  MIDI_error_Td Error;

  if(ExternalError != Expectation)
  {
    Error = LocalError;
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
  BufferPingPong_error_Td BufferError;

  MIDIPort->RxComplete = false;
  MIDIPort->TxComplete = true;

  MIDIPort->Buffer.RxAIndex = 0;
  MIDIPort->Buffer.RxBIndex = 0;
  MIDIPort->Buffer.TxAIndex = 0;
  MIDIPort->Buffer.TxBIndex = 0;

  BufferError =  BufferPingPong_init_StartConditions(&MIDIPort->Buffer);
  Error =  errorcheck_validate_ExternalErrorCode(BufferError, BUFFER_PINGPONG_NONE,  MIDI_ERROR_BUFFER_LIMITS_EXCEEDED);

  errorcheck_stop_Code(Error);

  UART_HandleTypeDef* huart = MIDIPort->huart;
  Error = errorcheck_PointerIsNull(huart, MIDI_ERROR_UART_NOT_INITIALIZED);

  errorcheck_stop_Code(Error);

#if 0 /*DEBUG*/
  uint8_t RxSize = 3;
  uint8_t* RxData = &MIDIPort->OneByteRxBuffer[0];
  HAL_UART_Receive_DMA(huart, RxData, RxSize);
#endif
  BufferPingPong_structTd* Buffer = &MIDIPort->Buffer;
  uint16_t RxSizeLimit =  BufferPingPong_get_RxBufferHeadroomSize();
  uint8_t* RxData = BufferPingPong_get_RxBufferHeadroom(Buffer);
  HAL_UARTEx_ReceiveToIdle_DMA(huart, RxData, RxSizeLimit);

  errorcheck_stop_Code(Error);

  Error =  MIDI_update_Transmission(MIDIPort);

  errorcheck_stop_Code(Error);

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

    errorcheck_stop_Code(Error);

    uint8_t* TxData = BufferPingPong_get_TxStartPtrForTransmission(&MIDIPort->Buffer);
    Error = errorcheck_PointerIsNull(TxData, MIDI_ERROR_BUFFER_TX_NULL);

    errorcheck_stop_Code(Error);

    BufferPingPong_error_Td BufferError;
    BufferError = BufferPingPong_toggle_TxBuffer(&MIDIPort->Buffer);
    Error = errorcheck_validate_ExternalErrorCode(BufferError, BUFFER_PINGPONG_ERROR_NONE, MIDI_ERROR_BUFFERMODULE);

    errorcheck_stop_Code(Error);

    uint16_t size = BufferPingPong_get_TxSizeForTransmission(&MIDIPort->Buffer);

    if(size > 0)
    {
      HAL_UART_Transmit_DMA(huart, TxData, size);
      MIDIPort->TxComplete = false;
    }
  }

  return Error;
}

/** @cond *//* Function Prototypes */
uint16_t get_SizeOfExpectedCommandBlock(uint8_t StatusByte);
/** @endcond *//* Function Prototypes */

/* Description in .h */
MIDI_error_Td MIDI_manage_RxInterrupt(MIDI_structTd* MIDIPort, UART_HandleTypeDef *huart, uint16_t Size, DMA_HandleTypeDef* DMA)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;
  UART_HandleTypeDef* huartValid = MIDIPort->huart;

  if(huartValid == huart)
  {
#if 0 /* DEBUG */
    /* Analyze last RxByte*/
    uint8_t PrevRxByte = MIDIPort->OneByteRxBuffer;
    BufferPingPong_structTd* Buffer = &MIDIPort->Buffer;

    if(PrevRxByte >= MIDI_STATUS_BYTE_MIN_VALUE) /* Process Status Byte */
    {
      BufferPingPong_queue_RxByteToHeadroom(Buffer, PrevRxByte);
      uint16_t DataSize = get_SizeOfExpectedCommandBlock(PrevRxByte);
      MIDIPort->NumExpectedRxDataBytes = DataSize;

      if(DataSize == MIDI_NUMBYTES_NODATA)
      {
        BufferPingPong_latch_RxHeadroomToBuffer(Buffer);
        MIDIPort->RxComplete = true;
      }
    }
    else
    {
      uint16_t LengthHeadroom = BufferPingPong_queue_RxByteToHeadroom(Buffer, PrevRxByte);
      if(LengthHeadroom == MIDIPort->NumExpectedRxDataBytes)
      {
        BufferPingPong_latch_RxHeadroomToBuffer(Buffer);
        MIDIPort->RxComplete = true;
      }
    }

    /* Reload DMA UART RX */
    uint8_t RxSize = 3;
    uint8_t* RxData = &MIDIPort->OneByteRxBuffer[0];
    HAL_UART_Receive_DMA(huart, RxData, RxSize);
#endif
    BufferPingPong_structTd* Buffer = &MIDIPort->Buffer;

    uint16_t RxSizeLimit =  BufferPingPong_get_RxBufferHeadroomSize();
    uint8_t* RxData = BufferPingPong_get_RxBufferHeadroom(Buffer);

    if(Size == (RxSizeLimit/2))
    {
      BufferPingPong_save_NumReceivedHeadroomBytes(Buffer, Size);
      HAL_UARTEx_ReceiveToIdle_DMA(huartValid, RxData, RxSizeLimit);

    }
    else
    {
      BufferPingPong_save_NumReceivedHeadroomBytes(Buffer, Size);
      BufferPingPong_latch_RxHeadroomToBuffer(Buffer);
      HAL_UARTEx_ReceiveToIdle_DMA(huartValid, RxData, RxSizeLimit);
      MIDIPort->RxComplete = true;
    }
    //__HAL_DMA_DISABLE_IT(DMA, DMA_IT_HT);
  }
  else
  {
    Error = MIDI_ERROR_INVALID_UART_HANDLE;
  }

  return Error;
}


/**
 * @brief     Switch through all possible status bytes and return the size
 *            that is expected.
 * @param     StatusByte
 * @return    expected size of the command block (Status Byte + Data)
 */
uint16_t get_SizeOfExpectedCommandBlock(uint8_t StatusByte)
{
  uint16_t ExpectedDataSize;
  MIDI_StatusBytes_Td StatusID;

  if(StatusByte < MIDI_STATUS_SYSTEM_EXCLUSIVE)
  {
    StatusID = StatusByte & ~MIDI_STATUS_CHANNEL_MSK;
  }
  else
  {
    StatusID = StatusByte;
  }

  switch (StatusID)
  {
    case MIDI_STATUS_NOTE_OFF:
      ExpectedDataSize = MIDI_NUMBYTES_STANDARD_MESSAGE;
      break;
    case MIDI_STATUS_NOTE_ON:
      ExpectedDataSize = MIDI_NUMBYTES_STANDARD_MESSAGE;
      break;
    case MIDI_STATUS_POLYPHONIC_AFTERTOUCH:
      ExpectedDataSize = MIDI_NUMBYTES_STANDARD_MESSAGE;
      break;
    case MIDI_STATUS_CONTROL_CHANGE:
      ExpectedDataSize = MIDI_NUMBYTES_STANDARD_MESSAGE;
      break;
    case MIDI_STATUS_PROGRAM_CHANGE:
      ExpectedDataSize = MIDI_NUMBYTES_SHORT_MESSAGE;
      break;
    case MIDI_STATUS_CHANNEL_AFTERTOUCH:
      ExpectedDataSize = MIDI_NUMBYTES_SHORT_MESSAGE;
      break;
    case MIDI_STATUS_PICH_BEND_CHANGE:
      ExpectedDataSize = MIDI_NUMBYTES_STANDARD_MESSAGE;
      break;
    case MIDI_STATUS_SYSTEM_EXCLUSIVE:
      ExpectedDataSize = MIDI_NUMBYTES_UNFEDINED;
      break;
    case MIDI_STATUS_MIDI_TIME_CODE_QTR_FRAME:
      ExpectedDataSize = MIDI_NUMBYTES_SHORT_MESSAGE;
      break;
    case MIDI_STATUS_SONG_POSITION_POINTER:
      ExpectedDataSize = MIDI_NUMBYTES_STANDARD_MESSAGE;
      break;
    case MIDI_STATUS_SONG_SELECT:
      ExpectedDataSize = MIDI_NUMBYTES_SHORT_MESSAGE;
      break;
    case MIDI_STATUS_TUNE_REQUEST:
      ExpectedDataSize = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_END_OF_SYS_EX:
      ExpectedDataSize = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_TIMING_CLOCK:
      ExpectedDataSize = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_START:
      ExpectedDataSize = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_CONTINUE:
      ExpectedDataSize = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_STOP:
      ExpectedDataSize = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_ACTIVE_SENSING:
      ExpectedDataSize = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_SYSTEM_RESET:
      ExpectedDataSize = MIDI_NUMBYTES_NODATA;
      break;
    default:
      ExpectedDataSize = MIDI_NUMBYTES_STANDARD_MESSAGE;
  }
  return ExpectedDataSize;
}

/* Description in .h */
MIDI_error_Td MIDI_manage_TxInterrupt(MIDI_structTd* MIDIPort, UART_HandleTypeDef* huart)
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

  Error = errorcheck_validate_MIDIBytes(StatusByte, DataByte1, DataByte2);

  if(Error == MIDI_ERROR_NONE)
  {
    uint8_t TxData[MIDI_LEN_STANDARD_COMMAND];
    TxData[0] = StatusByte;
    TxData[1] = DataByte1;
    TxData[2] = DataByte2;

    BufferPingPong_structTd* Buffer = &MIDIPort->Buffer;

    BufferPingPong_error_Td BufferError;
    BufferError = BufferPingPong_queue_TxBytesForTransmission(Buffer, TxData, MIDI_LEN_STANDARD_COMMAND);
    Error = errorcheck_validate_ExternalErrorCode(BufferError, BUFFER_PINGPONG_ERROR_NONE, MIDI_ERROR_BUFFERMODULE);
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
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_NOTE_OFF | Channel;
  Error = queue_MIDIThreeBytes(MIDIPort, StatusByte, Number, Velocity);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_NoteOn(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number, uint8_t Velocity)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

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
