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

typedef struct
{
  MIDI_StatusBytes_Td StatusByte;
  uint8_t MIDIChannel;
  uint16_t Size;
}MIDI_internal_CommandDescriptor_Td;

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
 * @brief     This function can be used for a quick pointer check. It returns
 *            an error code if the pointer is NULL.
 * @param     ptr         Pointer to be checked
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
 * @brief     This function checks a pointer and stops the code if the
 *            pointer is NULL.
 * @param     ptr         Pointer to be checked
 * @return    none
 */
void errorcheck_stop_CodeIfPointerIsNull(void* ptr)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  Error = errorcheck_PointerIsNull(ptr, MIDI_ERROR_INTERNAL);
  errorcheck_stop_Code(Error);
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
 * @brief     This function validates the bytes that should be used to build
 *            a SysEx MIDI command.
 * @param     StatusByte  0x80 - 0xFF
 * @param     Data        pointer to DataBytes (0x00 - 0x7F)
 * @param     Size        of SysEx Data (excluding Start Byte 0xF0 and Stop Byte 0xF7)
 * @return    MIDI_ERROR_NONE if all bytes are valid
 */
MIDI_error_Td errorcheck_validate_SysExBytes(uint8_t StatusByte, uint8_t* Data, uint16_t Size)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  /* Validate Status Byte */
  if(StatusByte != MIDI_STATUS_SYSTEM_EXCLUSIVE)
  {
    Error = MIDI_ERROR_INVALID_STATUS_BYTE;
  }
  else
  {
    /* Validate Data Bytes */
    for(uint8_t i = 0; i < Size; i++)
    {
      if(Data[i] >= MIDI_STATUS_BYTE_MIN_VALUE)
      {
        Error = MIDI_ERROR_INVALID_SYSEX_DATA;
      }
    }
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
  Error = errorcheck_PointerIsNull(huart, MIDI_ERROR_INVALID_HAL_HANDLE);

  MIDIPort->huart = huart;

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_init_DMARxHandle(MIDI_structTd* MIDIPort, DMA_HandleTypeDef* hdmaUartRx)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;
  Error = errorcheck_PointerIsNull(hdmaUartRx, MIDI_ERROR_INVALID_HAL_HANDLE);

  MIDIPort->hdmaUartRx = hdmaUartRx;

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
#define DEBUG_HT_INTERRUPT 0
/* Description in .h */
MIDI_error_Td MIDI_start_Transmission(MIDI_structTd* MIDIPort)
{
  /* Declare variables */
  MIDI_error_Td Error = MIDI_ERROR_NONE;
  BufferPingPong_error_Td BufferError;
  BufferPingPong_structTd* Buffer = &MIDIPort->Buffer;
  UART_HandleTypeDef* huart = MIDIPort->huart;
#if DEBUG_HT_INTERRUPT
  DMA_HandleTypeDef* hdmaUartRx = MIDIPort->hdmaUartRx;
#endif /* DEBUG_HT_INTERRUPT */
  uint16_t RxSizeLimit = 0;
  uint8_t* RxData = NULL;

  /* Validate Pointers */
  errorcheck_stop_CodeIfPointerIsNull(huart);
#if DEBUG_HT_INTERRUPT
  errorcheck_stop_CodeIfPointerIsNull(hdmaUartRx);
#endif /* DEBUG_HT_INTERRUPT */

  /* Set Start Defaults */
  MIDIPort->RxComplete = false;
  MIDIPort->TxComplete = true;

  MIDIPort->Buffer.RxAIndex = 0;
  MIDIPort->Buffer.RxBIndex = 0;
  MIDIPort->Buffer.TxAIndex = 0;
  MIDIPort->Buffer.TxBIndex = 0;

  /* initialize Buffer */
  BufferError =  BufferPingPong_init_StartConditions(&MIDIPort->Buffer);
  Error =  errorcheck_validate_ExternalErrorCode(BufferError, BUFFER_PINGPONG_NONE,  MIDI_ERROR_BUFFER_LIMITS_EXCEEDED);
  errorcheck_stop_Code(Error);

  /* initiate first UART Rx Cycle */
  RxSizeLimit =  BufferPingPong_get_SizeOfTempRxBuffer();
  RxData = BufferPingPong_get_StartPtrOfTempRxBuffer(Buffer);

  errorcheck_stop_CodeIfPointerIsNull(RxData);
  HAL_UARTEx_ReceiveToIdle_DMA(huart, RxData, RxSizeLimit);
#if DEBUG_HT_INTERRUPT
  /* @todo: figure out, why USART TX does nor work anymore with Half
   *        Transfer Callback disabled for the first Rx Cycle. */
  __HAL_DMA_DISABLE_IT(hdmaUartRx, DMA_IT_HT);
#endif /* DEBUG_HT_INTERRUPT */

  /* inititate first Tx Cycle */
  Error =  MIDI_update_Transmission(MIDIPort);
  errorcheck_stop_Code(Error);

  return Error;
}

/** @cond *//* Function Prototypes */
MIDI_error_Td update_RxData(MIDI_structTd* MIDIPort);
MIDI_error_Td update_TxData(MIDI_structTd* MIDIPort);
/** @endcond *//* Function Prototypes */

/* Description in .h */
MIDI_error_Td MIDI_update_Transmission(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  Error = update_RxData(MIDIPort);
  Error = update_TxData(MIDIPort);

  return Error;
}

/** @cond *//* Function Prototypes */
uint16_t process_MIDICommandAtBufferPointer(MIDI_structTd* MIDIPort, uint8_t* CommandStartPtr);
/** @endcond *//* Function Prototypes */

/**
 * @brief     update Received Data
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td update_RxData(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;
  bool RxComplete = MIDIPort->RxComplete;
  BufferPingPong_structTd* Buffer = &MIDIPort->Buffer;

  if(RxComplete == true)
  {

    if(MIDIPort->Buffer.ReservedToReceive == BUFFER_PINGPONG_RX_A)
    {
      MIDIPort->RxComplete = false;
    }
    else if(MIDIPort->Buffer.ReservedToReceive == BUFFER_PINGPONG_RX_B)
    {
      MIDIPort->RxComplete = false;
    }

    /* Get Buffer access */
    uint8_t* RxDataPtr = ButterPingPong_fetch_StartPtrOfFilledRxBuffer(Buffer);
    uint16_t RxSize = BufferPingPong_fetch_SizeOfFilledRxBuffer(Buffer);
    BufferPingPong_toggle_RxBuffer(Buffer);

    MIDIPort->RxComplete = false;

    if(RxSize > 0)
    {
      /* decode first MIDI-command */
      uint16_t CommandIndexOffset = 0;
      uint16_t CommandSize = 0;
      uint8_t* CommandStartPtr = &RxDataPtr[CommandIndexOffset];

      CommandSize = process_MIDICommandAtBufferPointer(MIDIPort, CommandStartPtr);
      CommandIndexOffset = CommandIndexOffset + CommandSize;

      /* decode all following MIDI-commands*/
      while(CommandIndexOffset < RxSize)
      {
        CommandStartPtr = &RxDataPtr[CommandIndexOffset];

        CommandSize = process_MIDICommandAtBufferPointer(MIDIPort, CommandStartPtr);
        CommandIndexOffset = CommandIndexOffset + CommandSize;
      }
    }
    else
    {
      Error = MIDI_ERROR_RX_BUFFER_EMPTY;
    }
  }

  return Error;
}

/** @cond *//* Function Prototypes */
MIDI_internal_CommandDescriptor_Td get_MIDICommandDescription(uint8_t StatusByte, uint8_t* Data);
MIDI_error_Td trigger_CallbackForReceivedMIDICommand(MIDI_structTd* MIDIPort, uint8_t* Data, MIDI_internal_CommandDescriptor_Td* MIDIDescriptor);
/** @endcond *//* Function Prototypes */
/**
 * @brief     Analyze the current command and call the corresponding callback
 *            function.
 * @param     MIDIPort        pointer to the users MIDI-Port data structure
 * @param     CommandStartPtr pointer to the command
 * @return    Size of the current command
 */
uint16_t process_MIDICommandAtBufferPointer(MIDI_structTd* MIDIPort, uint8_t* CommandStartPtr)
{
  uint16_t CommandSize = 0;
  MIDI_StatusBytes_Td StatusByte = CommandStartPtr[0];
  MIDI_internal_CommandDescriptor_Td MIDIDescriptor;

  MIDIDescriptor = get_MIDICommandDescription(StatusByte, CommandStartPtr);
  trigger_CallbackForReceivedMIDICommand(MIDIPort, CommandStartPtr, &MIDIDescriptor);
  CommandSize = MIDIDescriptor.Size;

  return CommandSize;
}

/**
 * @brief     Switch through all possible status bytes and return the description
 *            of the current MIDI-command
 * @param     StatusByte  of the MIDI-command
 * @param     Data*       start pointer of the command inside the buffer
 *                        (including StatusByte)
 * @return    MIDI description of current command
 */
MIDI_internal_CommandDescriptor_Td get_MIDICommandDescription(uint8_t StatusByte, uint8_t* Data)
{
  MIDI_internal_CommandDescriptor_Td MIDICommandDescription;
  MIDI_StatusBytes_Td StatusID;
  uint8_t MIDIChannel;

  if(StatusByte < MIDI_STATUS_SYSTEM_EXCLUSIVE)
  {
    StatusID = StatusByte & ~MIDI_STATUS_CHANNEL_MSK;
    MIDIChannel = StatusByte & MIDI_STATUS_CHANNEL_MSK;
  }
  else
  {
    StatusID = StatusByte;
  }

  MIDICommandDescription.StatusByte = StatusID;
  MIDICommandDescription.MIDIChannel = MIDIChannel;

  switch (StatusID)
  {
    case MIDI_STATUS_NOTE_OFF:
      MIDICommandDescription.Size = MIDI_NUMBYTES_STANDARD_MESSAGE;
      break;
    case MIDI_STATUS_NOTE_ON:
      MIDICommandDescription.Size = MIDI_NUMBYTES_STANDARD_MESSAGE;
      break;
    case MIDI_STATUS_POLYPHONIC_AFTERTOUCH:
      MIDICommandDescription.Size = MIDI_NUMBYTES_STANDARD_MESSAGE;
      break;
    case MIDI_STATUS_CONTROL_CHANGE:
      MIDICommandDescription.Size = MIDI_NUMBYTES_STANDARD_MESSAGE;
      break;
    case MIDI_STATUS_PROGRAM_CHANGE:
      MIDICommandDescription.Size = MIDI_NUMBYTES_SHORT_MESSAGE;
      break;
    case MIDI_STATUS_CHANNEL_AFTERTOUCH:
      MIDICommandDescription.Size = MIDI_NUMBYTES_SHORT_MESSAGE;
      break;
    case MIDI_STATUS_PICH_BEND_CHANGE:
      MIDICommandDescription.Size = MIDI_NUMBYTES_STANDARD_MESSAGE;
      break;
    case MIDI_STATUS_SYSTEM_EXCLUSIVE:
      /* Get Length of SysEx with Termination Byte*/
      uint16_t SysExLen = 0;
      while((Data[SysExLen]) != MIDI_STATUS_END_OF_SYS_EX)
      {
        SysExLen++;
      }
      SysExLen++; /* add 1 for Termination byte */
      MIDICommandDescription.Size = SysExLen;
      break;
    case MIDI_STATUS_MIDI_TIME_CODE_QTR_FRAME:
      MIDICommandDescription.Size = MIDI_NUMBYTES_SHORT_MESSAGE;
      break;
    case MIDI_STATUS_SONG_POSITION_POINTER:
      MIDICommandDescription.Size = MIDI_NUMBYTES_STANDARD_MESSAGE;
      break;
    case MIDI_STATUS_SONG_SELECT:
      MIDICommandDescription.Size = MIDI_NUMBYTES_SHORT_MESSAGE;
      break;
    case MIDI_STATUS_TUNE_REQUEST:
      MIDICommandDescription.Size = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_END_OF_SYS_EX:
      MIDICommandDescription.Size = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_TIMING_CLOCK:
      MIDICommandDescription.Size = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_START:
      MIDICommandDescription.Size = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_CONTINUE:
      MIDICommandDescription.Size = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_STOP:
      MIDICommandDescription.Size = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_ACTIVE_SENSING:
      MIDICommandDescription.Size = MIDI_NUMBYTES_NODATA;
      break;
    case MIDI_STATUS_SYSTEM_RESET:
      MIDICommandDescription.Size = MIDI_NUMBYTES_NODATA;
      break;
    default:
      errorcheck_stop_Code(MIDI_ERROR_INVALID_STATUS);
  }
  return MIDICommandDescription;
}

/**
 * @brief     Switch through the possible Status Bytes to call the corresponding
 *            callback function
 * @param     MIDIPort        pointer to the users MIDI-Port data structure
 * @param     Data            pointer to the first command Byte in data buffer
 * @param     MIDIDescriptor  Description of the MIDI command that will trigger
 *                            the callback function
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td trigger_CallbackForReceivedMIDICommand(MIDI_structTd* MIDIPort, uint8_t* Data, MIDI_internal_CommandDescriptor_Td* MIDIDescriptor)
{
  /* Declare variables */
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  MIDI_StatusBytes_Td Status = MIDIDescriptor->StatusByte;
  uint8_t Channel = MIDIDescriptor->MIDIChannel;
  uint8_t Byte1;
  uint8_t Byte2;
  uint16_t Size = MIDIDescriptor->Size;

  /* Get bytes, that will be given to the callback function */
  if(Size == MIDI_NUMBYTES_NODATA)
  {
    ;
  }
  else if(Size == MIDI_NUMBYTES_SHORT_MESSAGE)
  {
    Byte1 = Data[1];
  }
  else
  {
    Byte1 = Data[1];
    Byte2 = Data[2];
  }

  /* Call the callback function of the MIDI-command type*/
  switch (Status)
  {
    case MIDI_STATUS_NOTE_OFF:
      MIDI_callback_NoteOff(MIDIPort, Channel, Byte1, Byte2);
      break;
    case MIDI_STATUS_NOTE_ON:
      MIDI_callback_NoteOn(MIDIPort, Channel, Byte1, Byte2);
      break;
    case MIDI_STATUS_POLYPHONIC_AFTERTOUCH:
      MIDI_callback_PolyphonicAftertouch(MIDIPort, Channel, Byte1, Byte2);
      break;
    case MIDI_STATUS_CONTROL_CHANGE:
      MIDI_callback_ControlChange(MIDIPort, Channel, Byte1, Byte2);
      break;
    case MIDI_STATUS_PROGRAM_CHANGE:
      MIDI_callback_ProgramChange(MIDIPort, Channel, Byte1);
      break;
    case MIDI_STATUS_CHANNEL_AFTERTOUCH:
      MIDI_callback_ChannelAftertouch(MIDIPort, Channel, Byte1);
      break;
    case MIDI_STATUS_PICH_BEND_CHANGE:
      MIDI_callback_PitchBendChange(MIDIPort, Channel, Byte1, Byte2);
      break;
    case MIDI_STATUS_SYSTEM_EXCLUSIVE:
      MIDI_callback_SystemExclusive(MIDIPort, Data, Size);
      break;
    case MIDI_STATUS_MIDI_TIME_CODE_QTR_FRAME:
      MIDI_callback_MIDITimeCodeQuarterFrame(MIDIPort, Byte1);
      break;
    case MIDI_STATUS_SONG_POSITION_POINTER:
      MIDI_callback_SongPositionPointer(MIDIPort, Byte1, Byte2);
      break;
    case MIDI_STATUS_SONG_SELECT:
      MIDI_callback_SongSelect(MIDIPort, Byte1);
      break;
    case MIDI_STATUS_TUNE_REQUEST:
      MIDI_callback_TuneRequest(MIDIPort);
      break;
    case MIDI_STATUS_END_OF_SYS_EX:
      MIDI_callback_EndOfSysEx(MIDIPort);
      break;
    case MIDI_STATUS_TIMING_CLOCK:
      MIDI_callback_TimingClock(MIDIPort);
      break;
    case MIDI_STATUS_START:
      MIDI_callback_Start(MIDIPort);
      break;
    case MIDI_STATUS_CONTINUE:
      MIDI_callback_Continue(MIDIPort);
      break;
    case MIDI_STATUS_STOP:
      MIDI_callback_Stop(MIDIPort);
      break;
    case MIDI_STATUS_ACTIVE_SENSING:
      MIDI_callback_ActiveSensing(MIDIPort);
      break;
    case MIDI_STATUS_SYSTEM_RESET:
      MIDI_callback_Reset(MIDIPort);
      break;
    default:
      Error = MIDI_ERROR_INVALID_STATUS;
  }

  return Error;
}

/**
 * @brief     update Data to Transmit
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td update_TxData(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  /* Get relevant data from MIDI-Port*/
  bool TxComplete = MIDIPort->TxComplete;
  BufferPingPong_structTd* Buffer = &MIDIPort->Buffer;
  UART_HandleTypeDef* huart = MIDIPort->huart;

  /* update Tx */
  if(TxComplete == true)
  {
    /* Get Tx start point of the filled buffer*/
    uint8_t* TxData = NULL;
    TxData = BufferPingPong_get_StartPtrOfFilledTxBuffer(Buffer);
    errorcheck_stop_CodeIfPointerIsNull(TxData);

    /* Get Size of filled buffer */
    uint16_t size = BufferPingPong_get_SizeOfFilledTxBuffer(Buffer);

    /* Toggle buffer, so the filled buffer gets sent. */
    BufferPingPong_error_Td BufferError;
    BufferError = BufferPingPong_toggle_TxBuffer(Buffer);
    Error = errorcheck_validate_ExternalErrorCode(BufferError, BUFFER_PINGPONG_ERROR_NONE, MIDI_ERROR_BUFFERMODULE);
    errorcheck_stop_Code(Error);

    /* initiate transmission */
    MIDIPort->HALTxError = HAL_UART_Transmit_DMA(huart, TxData, size);
    if(MIDIPort->HALTxError == HAL_OK)
    {
      MIDIPort->TxComplete = false;
    }
  }

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_manage_RxInterrupt(MIDI_structTd* MIDIPort, UART_HandleTypeDef *huart, uint16_t Size)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;
  UART_HandleTypeDef* huartValid = MIDIPort->huart;
  DMA_HandleTypeDef* hdmaUartRx = MIDIPort->hdmaUartRx;

  if(huartValid == huart)
  {
    BufferPingPong_structTd* Buffer = &MIDIPort->Buffer;

    BufferPingPong_latch_TempRxBufferToRegularRxBuffer(Buffer, Size);

    uint16_t RxSizeLimit =  BufferPingPong_get_SizeOfTempRxBuffer();
    uint8_t* RxData = BufferPingPong_get_StartPtrOfTempRxBuffer(Buffer);



    MIDIPort->HALRxError = HAL_UARTEx_ReceiveToIdle_DMA(huartValid, RxData, RxSizeLimit);
    __HAL_DMA_DISABLE_IT(hdmaUartRx, DMA_IT_HT);

    MIDIPort->RxComplete = true;
  }
  else
  {
    Error = MIDI_ERROR_INVALID_HAL_HANDLE;
  }

  return Error;
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
    Error = MIDI_ERROR_INVALID_HAL_HANDLE;
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
 * @brief     Helper function to queue MIDI commands with 3 Bytes.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     StatusByte  0x80 - 0xFF
 * @param     DataByte1   0x00 - 0x7F
 * @param     DataByte2   0x00 - 0x7F
 * @return    MIDI_ERROR_NONE if all bytes are valid
 */
MIDI_error_Td queue_MIDIThreeBytes(MIDI_structTd* MIDIPort, uint8_t StatusByte, uint8_t DataByte1, uint8_t DataByte2)
{
  MIDI_error_Td Error;

  Error = errorcheck_validate_MIDIBytes(StatusByte, DataByte1, DataByte2);

  if(Error == MIDI_ERROR_NONE)
  {
    uint8_t TxData[MIDI_NUMBYTES_STANDARD_MESSAGE];
    TxData[0] = StatusByte;
    TxData[1] = DataByte1;
    TxData[2] = DataByte2;

    BufferPingPong_structTd* Buffer = &MIDIPort->Buffer;

    BufferPingPong_error_Td BufferError;
    BufferError = BufferPingPong_queue_TxBytesToEmptyBuffer(Buffer, TxData, MIDI_NUMBYTES_STANDARD_MESSAGE);
    Error = errorcheck_validate_ExternalErrorCode(BufferError, BUFFER_PINGPONG_ERROR_NONE, MIDI_ERROR_BUFFERMODULE);
  }
  else
  {
    ;
  }
  return Error;
}

/**
 * @brief     Helper function to queue MIDI commands with 2 Bytes.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     StatusByte  0x80 - 0xFF
 * @param     DataByte    0x00 - 0x7F
 * @return    MIDI_ERROR_NONE if all bytes are valid
 */
MIDI_error_Td queue_MIDITwoBytes(MIDI_structTd* MIDIPort, uint8_t StatusByte, uint8_t DataByte)
{
  MIDI_error_Td Error;

  uint8_t placeholder = 0x00;
  Error = errorcheck_validate_MIDIBytes(StatusByte, DataByte, placeholder);

  if(Error == MIDI_ERROR_NONE)
  {
    uint8_t TxData[MIDI_NUMBYTES_SHORT_MESSAGE];
    TxData[0] = StatusByte;
    TxData[1] = DataByte;

    BufferPingPong_structTd* Buffer = &MIDIPort->Buffer;

    BufferPingPong_error_Td BufferError;
    BufferError = BufferPingPong_queue_TxBytesToEmptyBuffer(Buffer, TxData, MIDI_NUMBYTES_SHORT_MESSAGE);
    Error = errorcheck_validate_ExternalErrorCode(BufferError, BUFFER_PINGPONG_ERROR_NONE, MIDI_ERROR_BUFFERMODULE);
  }
  else
  {
    ;
  }
  return Error;
}

/**
 * @brief     Helper function to queue MIDI commands with 1 Byte.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     StatusByte  0x80 - 0xFF
 * @return    MIDI_ERROR_NONE if all bytes are valid
 */
MIDI_error_Td queue_MIDIStatusByte(MIDI_structTd* MIDIPort, uint8_t StatusByte)
{
  MIDI_error_Td Error;

  uint8_t placeholder = 0x00;
  Error = errorcheck_validate_MIDIBytes(StatusByte, placeholder, placeholder);

  if(Error == MIDI_ERROR_NONE)
  {
    uint8_t TxData = StatusByte;
    BufferPingPong_structTd* Buffer = &MIDIPort->Buffer;

    BufferPingPong_error_Td BufferError;
    BufferError = BufferPingPong_queue_TxBytesToEmptyBuffer(Buffer, &TxData, MIDI_NUMBYTES_NODATA);
    Error = errorcheck_validate_ExternalErrorCode(BufferError, BUFFER_PINGPONG_ERROR_NONE, MIDI_ERROR_BUFFERMODULE);
  }
  else
  {
    ;
  }
  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_NoteOff(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Velocity)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_NOTE_OFF | Channel;
  Error = queue_MIDIThreeBytes(MIDIPort, StatusByte, Note, Velocity);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_NoteOn(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Velocity)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_NOTE_ON | Channel;
  Error = queue_MIDIThreeBytes(MIDIPort, StatusByte, Note, Velocity);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_PolyphonicAftertouch(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Value)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_POLYPHONIC_AFTERTOUCH | Channel;
  Error = queue_MIDIThreeBytes(MIDIPort, StatusByte, Note, Value);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_ControlChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number, uint8_t Value)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_CONTROL_CHANGE | Channel;
  Error = queue_MIDIThreeBytes(MIDIPort, StatusByte, Number, Value);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_ProgramChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number, uint8_t Value)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_PROGRAM_CHANGE | Channel;
  Error = queue_MIDIThreeBytes(MIDIPort, StatusByte, Number, Value);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_ChannelAftertouch(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Value)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_CHANNEL_AFTERTOUCH | Channel;
  Error = queue_MIDITwoBytes(MIDIPort, StatusByte, Value);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_PitchBendChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t LSB, uint8_t MSB)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_PICH_BEND_CHANGE | Channel;
  Error = queue_MIDIThreeBytes(MIDIPort, StatusByte, LSB, MSB);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_SystemExclusive(MIDI_structTd* MIDIPort, uint8_t* Data, uint16_t Size)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_SYSTEM_EXCLUSIVE;

  uint8_t OffsetStartEndByte = 2;
  const uint16_t SysExLength = Size + OffsetStartEndByte;

  Error = errorcheck_validate_SysExBytes(StatusByte, Data, Size);

  if(Error == MIDI_ERROR_NONE)
  {
    uint8_t TxData[SysExLength];

    /* Build EysEx Data Block */
    TxData[0] = StatusByte;

    for(uint8_t i = 0; i < Size; i++)
    {
      uint8_t OffsetStatusbyte = 1;
      TxData[i + OffsetStatusbyte] = Data[i];
    }

    TxData[SysExLength - 1] = MIDI_STATUS_END_OF_SYS_EX;

    /* queue data for Transmission */
    BufferPingPong_structTd* Buffer = &MIDIPort->Buffer;

    BufferPingPong_error_Td BufferError;
    BufferError = BufferPingPong_queue_TxBytesToEmptyBuffer(Buffer, TxData, SysExLength);
    Error = errorcheck_validate_ExternalErrorCode(BufferError, BUFFER_PINGPONG_ERROR_NONE, MIDI_ERROR_BUFFERMODULE);
  }
  else
  {
    ;
  }
  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_MIDITimeCodeQuarterFrame(MIDI_structTd* MIDIPort, uint8_t QtrFrame)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_MIDI_TIME_CODE_QTR_FRAME;
  Error = queue_MIDITwoBytes(MIDIPort, StatusByte, QtrFrame);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_SongPositionPointer(MIDI_structTd* MIDIPort, uint8_t LSB, uint8_t MSB)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_SONG_POSITION_POINTER;
  Error = queue_MIDIThreeBytes(MIDIPort, StatusByte, LSB, MSB);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_SongSelect(MIDI_structTd* MIDIPort, uint8_t Song)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_SONG_SELECT;
  Error = queue_MIDITwoBytes(MIDIPort, StatusByte, Song);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_TuneRequest(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_TUNE_REQUEST;
  Error = queue_MIDIStatusByte(MIDIPort, StatusByte);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_EndOfSysEx(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_END_OF_SYS_EX;
  Error = queue_MIDIStatusByte(MIDIPort, StatusByte);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_TimingClock(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_TIMING_CLOCK;
  Error = queue_MIDIStatusByte(MIDIPort, StatusByte);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_Start(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_START;
  Error = queue_MIDIStatusByte(MIDIPort, StatusByte);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_Continue(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_CONTINUE;
  Error = queue_MIDIStatusByte(MIDIPort, StatusByte);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_Stop(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_STOP;
  Error = queue_MIDIStatusByte(MIDIPort, StatusByte);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_ActiveSensing(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_ACTIVE_SENSING;
  Error = queue_MIDIStatusByte(MIDIPort, StatusByte);

  return Error;
}

/* Description in .h */
MIDI_error_Td MIDI_queue_Reset(MIDI_structTd* MIDIPort)
{
  MIDI_error_Td Error = MIDI_ERROR_NONE;

  uint8_t StatusByte = MIDI_STATUS_SYSTEM_RESET;
  Error = queue_MIDIStatusByte(MIDIPort, StatusByte);

  return Error;
}
/** @} ************************************************************************/
/* end of name "Interaction"
 ******************************************************************************/

/* Channel Voice Messages */
__weak void MIDI_callback_NoteOff(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Velocity)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
  UNUSED(Channel);
  UNUSED(Note);
  UNUSED(Velocity);
}

__weak void MIDI_callback_NoteOn(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Velocity)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
  UNUSED(Channel);
  UNUSED(Note);
  UNUSED(Velocity);
}

__weak void MIDI_callback_PolyphonicAftertouch(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Value)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
  UNUSED(Channel);
  UNUSED(Note);
  UNUSED(Value);
}

__weak void MIDI_callback_ControlChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number, uint8_t Value)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
  UNUSED(Channel);
  UNUSED(Number);
  UNUSED(Value);
}

__weak void MIDI_callback_ProgramChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
  UNUSED(Channel);
  UNUSED(Number);
}

__weak void MIDI_callback_ChannelAftertouch(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Value)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
  UNUSED(Channel);
  UNUSED(Value);
}

__weak void MIDI_callback_PitchBendChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t LSB, uint8_t MSB)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
  UNUSED(Channel);
  UNUSED(LSB);
  UNUSED(MSB);
}


/* System Common Messages */
__weak void MIDI_callback_SystemExclusive(MIDI_structTd* MIDIPort, uint8_t* Data, uint16_t Size)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
  UNUSED(Data);
  UNUSED(Size);
}

__weak void MIDI_callback_MIDITimeCodeQuarterFrame(MIDI_structTd* MIDIPort, uint8_t QtrFrame)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
  UNUSED(QtrFrame);
}

__weak void MIDI_callback_SongPositionPointer(MIDI_structTd* MIDIPort, uint8_t LSB, uint8_t MSB)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
  UNUSED(LSB);
  UNUSED(MSB);
}

__weak void MIDI_callback_SongSelect(MIDI_structTd* MIDIPort, uint8_t Song)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
  UNUSED(Song);
}

__weak void MIDI_callback_TuneRequest(MIDI_structTd* MIDIPort)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
}

__weak void MIDI_callback_EndOfSysEx(MIDI_structTd* MIDIPort)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
}

/* System Real-Time Messages */
__weak void MIDI_callback_TimingClock(MIDI_structTd* MIDIPort)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
}

__weak void MIDI_callback_Start(MIDI_structTd* MIDIPort)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
}

__weak void MIDI_callback_Continue(MIDI_structTd* MIDIPort)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
}

__weak void MIDI_callback_Stop(MIDI_structTd* MIDIPort)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
}

__weak void MIDI_callback_ActiveSensing(MIDI_structTd* MIDIPort)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
}

__weak void MIDI_callback_Reset(MIDI_structTd* MIDIPort)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(MIDIPort);
}

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
