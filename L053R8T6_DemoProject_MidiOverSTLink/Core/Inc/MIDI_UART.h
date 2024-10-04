/***************************************************************************//**
 * @defgroup				MIDI_UART   Module to transfer MIDI over UART.
 * @brief
 *
 * @defgroup        MIDI_UART_Header    Header
 * @brief						Study this part for a quick overview of this module.
 *
 * @addtogroup			MIDI_UART
 * @{
 *
 * @addtogroup      MIDI_UART_Header
 * @{
 *
 * @file            MIDI_UART.h
 *
 * @date            Aug 7, 2024
 * @author          Mario
 ******************************************************************************/

#ifndef INC_MIDIOVERSTLINK_H__MN
#define INC_MIDIOVERSTLINK_H__MN

#include "stm32l0xx_hal.h"
#include <stdbool.h>

#include "Buffer_PingPong.h"

#define MIDI_LEN_STANDARD_COMMAND 3
#define MIDI_STATUS_BYTE_MIN_VALUE  0x80
#define MIDI_STATUS_CHANNEL_MSK 0x0F /**< used to mask bytes, that are used to
                                          identify the MIDI-channel*/
/***************************************************************************//**
 * @name      Structure and Enumerations
 * @{
 ******************************************************************************/

/**
 * @brief     Enumerations for MIDI 1.0 specifications: Status Bytes
 */
typedef enum
{
  /* Channel Voice Messages */
  MIDI_STATUS_NOTE_OFF = 0x80,
  MIDI_STATUS_NOTE_ON = 0x90,
  MIDI_STATUS_POLYPHONIC_AFTERTOUCH = 0xA0,
  MIDI_STATUS_CONTROL_CHANGE = 0xB0,
  MIDI_STATUS_PROGRAM_CHANGE = 0xC0,
  MIDI_STATUS_CHANNEL_AFTERTOUCH = 0xD0,
  MIDI_STATUS_PICH_BEND_CHANGE = 0xE0,

  /* System Common Messages */
  MIDI_STATUS_SYSTEM_EXCLUSIVE = 0xF0,
  MIDI_STATUS_MIDI_TIME_CODE_QTR_FRAME = 0xF1,
  MIDI_STATUS_SONG_POSITION_POINTER = 0xF2,
  MIDI_STATUS_SONG_SELECT = 0xF3,
  MIDI_STATUS_TUNE_REQUEST = 0xF6,
  MIDI_STATUS_END_OF_SYS_EX = 0xF7,

  /* System Real-Time Messages */
  MIDI_STATUS_TIMING_CLOCK = 0xF8,
  MIDI_STATUS_START = 0xFA,
  MIDI_STATUS_CONTINUE = 0xFB,
  MIDI_STATUS_STOP = 0xFC,
  MIDI_STATUS_ACTIVE_SENSING = 0xFE,
  MIDI_STATUS_SYSTEM_RESET = 0xFF,
}MIDI_StatusBytes_Td;

/**
 * @brief     Enumerations for error handling of this module.
 */
typedef enum
{
  MIDI_ERROR_NONE = 0x00,

  MIDI_ERROR_BUFFER_OVERFLOW = 0x10,
  MIDI_ERROR_BUFFER_LIMITS_EXCEEDED = 0x11,
  MIDI_ERROR_BUFFERMODULE = 0x12,

  MIDI_ERROR_WAIT_FOR_TRANSMISSION_COMPLETE = 0x20,

  MIDI_ERROR_INVALID_HAL_HANDLE = 0x30,
  MIDI_ERROR_UART_NOT_INITIALIZED = 0x31,

  MIDI_ERROR_INVALID_STATUS_BYTE = 0x40,
  MIDI_ERROR_INVALID_BYTE_1 = 0x41,
  MIDI_ERROR_INVALID_BYTE_2 = 0x42,
  MIDI_ERROR_INVALID_SYSEX_DATA = 0x43,

  MIDI_ERROR_RX_BUFFER_EMPTY = 0x50,
  MIDI_ERROR_RX_BUFFER_TOGGLE_FAILED = 0x51,

  MIDI_ERROR_BUFFER_TX_NULL = 0x60,

  MIDI_ERROR_POINTER_IS_NULL = 0x70,

  MIDI_ERROR_INVALID_DATA = 0x80,
  MIDI_ERROR_INVALID_STATUS = 0x81,

  /* This code must not be used to be exported. It is
   * reserved for internal use only as a momentary
   * transfer value */
  MIDI_ERROR_INTERNAL = 0x90,
}MIDI_error_Td;

/**
 * @brief     Enumerations for user application.
 */
typedef enum
{
  MIDI_NOT_VELOCITY_SENSITIVE = 0x40,
}MIDI_user_Td;

/**
 * @brief     Structure used for each MIDI Port.
 */
typedef struct
{
  UART_HandleTypeDef* huart;    /**<  HAL UART handle used for MIDI
                                      transmission */
  DMA_HandleTypeDef* hdmaUartRx; /**<  HAL UART DMA handle used for MIDI
                                      transmission */
  BufferPingPong_structTd Buffer;  /**< Buffer data structure for data
                                      management */

  bool    TxComplete;
  bool    RxComplete;
}MIDI_structTd;
/** @} ************************************************************************/
/* end of name "Structure and Enumerations"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize this module.
 * @{
 ******************************************************************************/

/**
 * @brief     Link the HAL UART handle used for the MIDI-Port
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     huart       pointer to the HAL UART handle
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td MIDI_init_UART(MIDI_structTd* MIDIPort, UART_HandleTypeDef* huart);

/**
 * @brief     Link the HAL UART DMA handle used for the MIDI-Port
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     hdmaUart   pointer to the DMA handle used for UART.
 *            @note       If the HAL Files are generated in seperate .h/.c
 *                        pairs, the HAL-Handle may not be exported as default.
 *                        In this case, look for the handle in usart.c and copy
 *                        it to usart.h with "extern". It should look like this
 *                        (depending on the used uart and dma port):
 *                        @code
 *                        // USER CODE BEGIN Private defines
 *                        extern DMA_HandleTypeDef hdma_usart2_rx;
 *                        // USER CODE END Private defines
 *                        @endcode
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td MIDI_init_DMARxHandle(MIDI_structTd* MIDIPort, DMA_HandleTypeDef* hdmaUartRx);
/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Process
 * @brief     Use these functions to process this module
 * @{
 ******************************************************************************/

/**
 * @brief     Call this function for the first transmission. Following
 *            transmissions will be triggered by the update function.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td MIDI_start_Transmission(MIDI_structTd* MIDIPort);

/**
 * @brief     Call this function to send scheduled data over UART and to
 *            copy received data to the Rx Buffer.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td MIDI_update_Transmission(MIDI_structTd* MIDIPort);

/**
 * @brief     Call this function in HAL UART Rx Callback to set a flag, it the
 *            UART Transmission is complete.
 *            @code
 *            void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart)
 *            {
 *              MIDI_manage_RxInterrupt(&MIDIPort, huart, Size);
 *            }
 *            @endcode
 * @note      Make sure to use the correct UART Callback. The normal
 *            HAL_UART_TxCpltCallback() will not work, because it does not
 *            interrupt on idle state if the Rx Size was not reached.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     huart       pointer to the interrupted HAL UART handle
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td MIDI_manage_RxInterrupt(MIDI_structTd* MIDIPort, UART_HandleTypeDef *huart, uint16_t Size);

/**
 * @brief     Call this function in HAL UART Tx Callback to set a flag, it the
 *            UART Transmission is complete.
 *            @code
 *            void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
 *            {
 *              MIDI_manage_TxInterrupt(&MIDIPort, huart);
 *            }
 *            @endcode
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     huart       pointer to the interrupted HAL UART handle
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td MIDI_manage_TxInterrupt(MIDI_structTd* MIDIPort, UART_HandleTypeDef *huart);
/** @} ************************************************************************/
/* end of name "Process"
 ******************************************************************************/


/***************************************************************************//**
 * @name      MIDI Send Functions
 * @brief     Use these functions to queue MIDI data to be sent by update
 *            function.
 * @note      The following descriptions are copied and slightly adapted
 *            from the document "Summary of MIDI Messages" by "MIDI
 *            Manufacturers Association". Link to Document:
 *            https://midi.org/summary-of-midi-1-0-messages
 * @note      These functions do not send the MIDI-Message immediately. They
 *            will be queued to the Buffer, that is used to send Data. The
 *            actual data transfer will be initiated by the
 *            MIDI_update_Transmission() function.
 * @{
 ******************************************************************************/

/**
 * @brief     Note Off event. This message is sent when a note is released.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     used MIDI channel (1-16)
 * @param     Note        number (0-127 | note c' = 60)
 * @param     Velocity    of the note (0-127 | No Velocity default: 64)
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td MIDI_queue_NoteOff(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Velocity);

/**
 * @brief     Note On event. This message is sent when a note is played.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     Midi Channel (0-15)
 * @param     Note        number (0-127 | note c' = 60)
 * @param     Velocity    of the note (0-127)
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td MIDI_queue_NoteOn(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Velocity);

/**
 * @brief     Polyphonic Key Pressure (Aftertouch). This message is most often
 *            sent by pressing down on the key after it "bottoms out".
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     Midi Channel (0-15)
 * @param     Note        number (0-127 | note c' = 60)
 * @param     Value       of pressure
 * @return    none
 */
MIDI_error_Td MIDI_queue_PolyphonicAftertouch(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Value);

/**
 * @brief     Control Change. This message is sent when a controller value
 *            changes. Controllers include devices such as pedals and levers.
 *            Controller numbers 120-127 are reserved as "Channel Mode Messages".
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     used MIDI channel
 * @param     Number      of the control (0-119)(120-127 are reserved for
 *                        Channel Mode Messages. See MIDI documentation for
 *                        details)
 * @param     Value       of the control (0-127)
 * @return    none
 */
MIDI_error_Td MIDI_queue_ControlChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number, uint8_t Value);

/**
 * @brief     Program Change. This message sent when the patch number
 *            changes.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     used MIDI channel
 * @param     Number      of the new program
 * @return    none
 */
MIDI_error_Td MIDI_queue_ProgramChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number, uint8_t Value);

/**
 * @brief     Channel Pressure (After-touch). This message is most often sent by
 *            pressing down on the key after it "bottoms out". This message is
 *            different from polyphonic after-touch. Use this message to receive
 *            the single greatest pressure value (of all the current depressed
 *            keys)
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     used MIDI channel
 * @param     Value       of the pressure
 * @return    none
 */
MIDI_error_Td MIDI_queue_ChannelAftertouch(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Value);

/**
 * @brief     Pitch Bend Change. This message is sent to indicate a change
 *            in the pitch bender (wheel or lever, typically). The pitch bender
 *            is measured by a fourteen bit value. Center (no pitch change) is
 *            0x2000. Sensitivity is a function of the receiver, but may be set
 *            using RPN 0.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     LSB         least significant 7 bits
 * @param     MSB         most significant 7 bits
 * @return    none
 */
MIDI_error_Td MIDI_queue_PitchBendChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t LSB, uint8_t MSB);


/**
 * @brief     Queue data to send a SysEx message.
 * @note      Status byte and Termination byte are sent automatically, they
 *            should not be part of Data. Manufacturer ID's have to be
 *            part of the data. This is how the SysEx will be send:
 *            | StatusByte | Data1 | ... | DataN | Termination |
 *            | ---------- | ----- | --- | ----- | ----------- |
 *            | OxF0       | 0-127 | ... | 0-127 | 0xF7        |
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Data        pointer to the Data to be transmitted as SysEx
 *                        command.
 * @param     Size        of the Data to be transmitted
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td MIDI_queue_SystemExclusive(MIDI_structTd* MIDIPort, uint8_t* Data, uint16_t Size);

/**
 * @brief     MIDI Time Code Quarter Frame.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     QtrFrame    Bit-structure: 0nnndddd, nnn = Message Type,
 *                        dddd = Values.
 * @return    none
 */
MIDI_error_Td MIDI_queue_MIDITimeCodeQuarterFrame(MIDI_structTd* MIDIPort, uint8_t QtrFrame);

/**
 * @brief     Song Position Pointer. This is an 14 bit register that
 *            holds the number of MIDI beats (1 beat = six MIDI clocks) since
 *            the start of the song.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     LSB         least significant 7 bits
 * @param     MSB         most significant 7 bits
 * @return    none
 */
MIDI_error_Td MIDI_queue_SongPositionPointer(MIDI_structTd* MIDIPort, uint8_t LSB, uint8_t MSB);

/**
 * @brief     Song Select. The Song Select species which sequence or song is to
 *            be played.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Song        Number to be selected
 * @return    none
 */
MIDI_error_Td MIDI_queue_SongSelect(MIDI_structTd* MIDIPort, uint8_t Song);

/**
 * @brief     Tune Request. Upon receiving a Tune Request, all analog
 *            synthesizers should tune their oscillators.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
MIDI_error_Td MIDI_queue_TuneRequest(MIDI_structTd* MIDIPort);

/**
 * @brief     End of Exclusive. Used to terminate a System Exclusive dump
 *            (see MIDI_callback_SystemExclusive())
 *            @note This function exists for completeness and maybe for use
 *                  in the future. It should not be necessary to use this
 *                  function in the current implementation, as the End of
 *                  Exclusive is already handled in
 *                  MIDI_callback_SystemExclusive().
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
MIDI_error_Td MIDI_queue_EndOfSysEx(MIDI_structTd* MIDIPort);

/**
 * @brief     Timing Clock. Sent 24 times per quarter note when
 *            synchronization is required.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
MIDI_error_Td MIDI_queue_TimingClock(MIDI_structTd* MIDIPort);

/**
 * @brief     Start. Start the current sequence playing. (This message will be
 *            followed with Timing Clocks).
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
MIDI_error_Td MIDI_queue_Start(MIDI_structTd* MIDIPort);

/**
 * @brief     Continue. Continue at the point the sequence was Stopped.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
MIDI_error_Td MIDI_queue_Continue(MIDI_structTd* MIDIPort);

/**
 * @brief     Stop. Stop the current sequence.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
MIDI_error_Td MIDI_queue_Stop(MIDI_structTd* MIDIPort);

/**
 * @brief     Active Sensing. This message is intended to be sent repeatedly
 *            to tell the receiver that a connection is alive. Use of this
 *            message is optional. When initially received, the receiver will
 *            expect to receive another Active Sensing message each 300ms (max),
 *            and if it does not then it will assume that the connection has
 *            been terminated. At termination, the receiver will turn off all
 *            voices and return to normal (non- active sensing) operation.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
MIDI_error_Td MIDI_queue_ActiveSensing(MIDI_structTd* MIDIPort);

/**
 * @brief     Reset. Reset all receivers in the system to power- up status. This
 *            should be used sparingly, preferably under manual control. In
 *            particular, it should not be sent on power-up.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
MIDI_error_Td MIDI_queue_Reset(MIDI_structTd* MIDIPort);

/** @} ************************************************************************/
/* end of name "MIDI Send Functions"
 ******************************************************************************/


/***************************************************************************//**
 * @name      MIDI Receive Callback Functions
 * @brief     Use these callback functions to handle received MIDI Data.
 * @note      The following descriptions are copied and slightly adapted
 *            from the document "Summary of MIDI Messages" by "MIDI
 *            Manufacturers Association". Link to Document:
 *            https://midi.org/summary-of-midi-1-0-messages
 * @note      How to receive data: These callback functions are empty
 *            prototypes. They will be called internal, if a specific command
 *            was received. The user has to fill the function in his own code
 *            to control, what will happen with the received data.
 *            Here is an example:
 *            @code
 *            void MIDI_callback_NoteOn(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Velocity)
 *            {
 *              // Do something here with the data received from this
 *              // functions arguments.
 *            }
 *            @endcode
 *
 * @{
 ******************************************************************************/

/* Channel Voice Messages */

/**
 * @brief     Note Off event. This message is received when a note is released.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     used MIDI channel
 * @param     Note        number
 * @param     Velocity    of the released note
 * @return    none
 */
void MIDI_callback_NoteOff(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Velocity);

/**
 * @brief     Note On event. This message is received when a note is played.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     used MIDI channel
 * @param     Note        number (0-127 | note c' = 60)
 * @param     Velocity    of the played note
 * @return    none
 */
void MIDI_callback_NoteOn(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Velocity);

/**
 * @brief     Polyphonic Key Pressure (Aftertouch). This message is most often
 *            received by pressing down on the key after it "bottoms out".
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     used MIDI channel
 * @param     Note        number (0-127 | note c' = 60)
 * @param     Value       of pressure
 * @return    none
 */
void MIDI_callback_PolyphonicAftertouch(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Value);

/**
 * @brief     Control Change. This message is received when a controller value
 *            changes. Controllers include devices such as pedals and levers.
 *            Controller numbers 120-127 are reserved as "Channel Mode Messages".
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     used MIDI channel
 * @param     Number      of the control (0-119)(120-127 are reserved for
 *                        Channel Mode Messages. See MIDI documentation for
 *                        details)
 * @param     Value       of the control (0-127)
 * @return    none
 */
void MIDI_callback_ControlChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number, uint8_t Value);

/**
 * @brief     Program Change. This message received when the patch number
 *            changes.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     used MIDI channel
 * @param     Number      of the new program
 * @return    none
 */
void MIDI_callback_ProgramChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note);

/**
 * @brief     Channel Pressure (After-touch). This message is most often
 *            received by pressing down on the key after it "bottoms out". This
 *            message is different from polyphonic after-touch. Use this message
 *            to receive the single greatest pressure value (of all the current
 *            depressed keys)
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     used MIDI channel
 * @param     Value       of the pressure
 * @return    none
 */
void MIDI_callback_ChannelAftertouch(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Value);

/**
 * @brief     Pitch Bend Change. This message is received to indicate a change
 *            in the pitch bender (wheel or lever, typically). The pitch bender
 *            is measured by a fourteen bit value. Center (no pitch change) is
 *            0x2000. Sensitivity is a function of the receiver, but may be set
 *            using RPN 0.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     LSB         least significant 7 bits
 * @param     MSB         most significant 7 bits
 * @return    none
 */
void MIDI_callback_PitchBendChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t LSB, uint8_t MSB);

/* System Common Messages */

/**
 * @brief     System Exclusive. This message type allows manufacturers to
 *            create their own messages (such as bulk dumps, patch parameters,
 *            and other non-spec data) and provides a mechanism for creating
 *            additional MIDI Specication messages. The Manufacturer's ID code
 *            (assigned by MMA or AMEI) is either 1 byte (0iiiiiii) or 3 bytes
 *            (0iiiiiii 0iiiiiii 0iiiiiii). Two of the 1 Byte IDs are reserved
 *            for extensions called Universal Exclusive Messages, which are not
 *            manufacturer-specic. If a device recognizes the ID code as its own
 *            (or as a supported Universal message) it will listen to the rest
 *            of the message (0ddddddd). Otherwise, the message will be ignored.
 *            (Note: Only Real-Time messages may be interleaved with a System
 *            Exclusive.) <br>
 *            | Byte 1   | Byte 2   | Byte 3   | Byte 4   | ... | Byte N   | Last Byte |
 *            |----------|----------|----------|----------|-----|----------|-----------|
 *            | 0iiiiiii |(0iiiiiii)|(0iiiiiii)| 0ddddddd | ... | 0ddddddd | 0xF7      |
 *
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Data        pointer to the complete SysEx Data String
 * @param     Size        of the SysEx Data String including End of SysEx byte
 * @return    none
 */
void MIDI_callback_SystemExclusive(MIDI_structTd* MIDIPort, uint8_t* Data, uint16_t Size);

/**
 * @brief     MIDI Time Code Quarter Frame.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     QtrFrame    Bit-structure: 0nnndddd, nnn = Message Type,
 *                        dddd = Values.
 * @return    none
 */
void MIDI_callback_MIDITimeCodeQuarterFrame(MIDI_structTd* MIDIPort, uint8_t QtrFrame);

/**
 * @brief     Song Position Pointer. This is an 14 bit register that
 *            holds the number of MIDI beats (1 beat = six MIDI clocks) since
 *            the start of the song.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     LSB         least significant 7 bits
 * @param     MSB         most significant 7 bits
 * @return    none
 */
void MIDI_callback_SongPositionPointer(MIDI_structTd* MIDIPort, uint8_t LSB, uint8_t MSB);

/**
 * @brief     Song Select. The Song Select species which sequence or song is to
 *            be played.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Song        Number to be selected
 * @return    none
 */
void MIDI_callback_SongSelect(MIDI_structTd* MIDIPort, uint8_t Song);

/**
 * @brief     Tune Request. Upon receiving a Tune Request, all analog
 *            synthesizers should tune their oscillators.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
void MIDI_callback_TuneRequest(MIDI_structTd* MIDIPort);

/**
 * @brief     End of Exclusive. Used to terminate a System Exclusive dump
 *            (see MIDI_callback_SystemExclusive())
 *            @note This callback exists for completeness and maybe use
 *                  in the future. It will not be called in the current
 *                  implementation, as the End of Sys Ex is already part of
 *                  MIDI_callback_SystemExclusive().
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
void MIDI_callback_EndOfSysEx(MIDI_structTd* MIDIPort);

/* System Real-Time Messages */

/**
 * @brief     Timing Clock. Received 24 times per quarter note when
 *            synchronization is required.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
void MIDI_callback_TimingClock(MIDI_structTd* MIDIPort);

/**
 * @brief     Start. Start the current sequence playing. (This message will be
 *            followed with Timing Clocks).
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
void MIDI_callback_Start(MIDI_structTd* MIDIPort);

/**
 * @brief     Continue. Continue at the point the sequence was Stopped.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
void MIDI_callback_Continue(MIDI_structTd* MIDIPort);

/**
 * @brief     Stop. Stop the current sequence.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
void MIDI_callback_Stop(MIDI_structTd* MIDIPort);

/**
 * @brief     Active Sensing. This message is intended to be received repeatedly
 *            to tell the receiver that a connection is alive. Use of this
 *            message is optional. When initially received, the receiver will
 *            expect to receive another Active Sensing message each 300ms (max),
 *            and if it does not then it will assume that the connection has
 *            been terminated. At termination, the receiver will turn off all
 *            voices and return to normal (non- active sensing) operation.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
void MIDI_callback_ActiveSensing(MIDI_structTd* MIDIPort);

/**
 * @brief     Reset. Reset all receivers in the system to power- up status. This
 *            should be used sparingly, preferably under manual control. In
 *            particular, it should not be sent on power-up.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @return    none
 */
void MIDI_callback_Reset(MIDI_structTd* MIDIPort);
/** @} ************************************************************************/
/* end of name "MIDI Receive Callback Functions"
 ******************************************************************************/


/**@}*//* end of defgroup "MIDI_UART_Header" */
/**@}*//* end of defgroup "MIDI_UART" */

#endif /* INC_MIDIOVERSTLINK_H__MN */
