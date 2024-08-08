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

#define MIDI_UART_MAX_BUFFER  255   /**< Max is 255 because index counter is
                                         a uint8_t. If a larger value is
                                         required, change data type of index
                                         counters to avoid overflow. */
#define MIDI_LEN_STANDARD_COMMAND 3
/***************************************************************************//**
 * @name      Structure and Enumerations
 * @{
 ******************************************************************************/

/**
 * @brief     Enumerations for MIDI 1.0 specifications: Status Bytes
 */
typedef enum
{
  MIDI_STATUS_NOTE_OFF = 0x80,
  MIDI_STATUS_NOTE_ON = 0x90,
  MIDI_STATUS_POLYPHONIC_AFTERTOUCH = 0xA0,
  MIDI_STATUS_CONTROL_CHANGE = 0xB0,
  MIDI_STATUS_PROGRAM_CHANGE = 0xC0,
  MIDI_STATUS_CHANNEL_AFTERTOUCH = 0xD0,
  MIDI_STATUS_PICH_BEND_CHANGE = 0xE0,

  MIDI_STATUS_SYSTEM_EXCLUSIVE = 0xF0,
  MIDI_STATUS_MIDI_TIME_CODE_QTR_FRAME = 0xF1,
  MIDI_STATUS_SONG_POSITION_POINTER = 0xF2,
  MIDI_STATUS_SONG_SELECT = 0xF3,
  MIDI_STATUS_TUNE_REQUEST = 0xF6,
  MIDI_STATUS_END_OF_SYS_EX = 0xF7,
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
  MIDI_ERROR_BUFFER_TX_NULL = 0x11,

  MIDI_ERROR_WAIT_FOR_TRANSMISSION_COMPLETE = 0x20,

  MIDI_ERROR_INVALID_UART_HANDLE = 0x30,
  MIDI_ERROR_UART_NOT_INITIALIZED = 0x31,

  MIDI_ERROR_INVALID_STATUS_BYTE = 0x40,
  MIDI_ERROR_INVALID_BYTE_1 = 0x41,
  MIDI_ERROR_INVALID_BYTE_2 = 0x42
}MIDI_error_Td;

/**
 * @brief     Enumerations to describe the buffer and buffer-internal states.
 */
typedef enum
{
  MIDI_BUFFER_NONE = 0x00,
  MIDI_BUFFER_TX_A = 0x10,
  MIDI_BUFFER_TX_B = 0x11,
  MIDI_BUFFER_RX_A = 0x12,
  MIDI_BUFFER_RX_B = 0x13,
}MIDI_buffer_Td;

typedef enum
{
  MIDI_BUFFER_ERROR_NONE = 0x00,
  MIDI_BUFFER_ERROR_TX_A_OVERFLOW = 0x10,
  MIDI_BUFFER_ERROR_TX_B_OVERFLOW = 0x10,
  MIDI_BUFFER_ERROR_NO_BUFFER_FOUND = 0x20,
}MIDI_buffer_error_Td;
/**
 * @brief     Structure to buffer data for DMA Transmission. The buffering
 *            works with a ping-pong principle.
 * @note      The user does not need to setup this data structure manually. It
 *            is part of the main MIDI structure.
 */
typedef struct
{
  MIDI_buffer_Td LockedToSend;
  MIDI_buffer_Td LockedToReceive;

  uint8_t TxA[MIDI_UART_MAX_BUFFER]; /**< Array to buffer MIDI
                                   bytes for transmission */
  uint16_t TxAIndex;      /**< Index counter for Tx-Array */
  uint8_t TxB[MIDI_UART_MAX_BUFFER]; /**< Array to buffer MIDI
                                   bytes for transmission */
  uint16_t TxBIndex;      /**< Index counter for Tx-Array */

  uint8_t RxA[MIDI_UART_MAX_BUFFER]; /**< Array to buffer received
                                   bytes */
  uint16_t RxAIndex;      /**< Index counter for Rx-Array */
  uint8_t RxB[MIDI_UART_MAX_BUFFER]; /**< Array to buffer received
                                   bytes */
  uint16_t RxBIndex;      /**< Index counter for Rx-Array */

}MIDI_buffer_structTd;

/**
 * @brief     Structure used for each MIDI Port.
 */
typedef struct
{
  UART_HandleTypeDef* huart;    /**< HAL UART handle used for MIDI transmission */

  MIDI_buffer_structTd Buffer;  /**< Buffer data structure for data management */

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
 *            void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 *            {
 *              MIDI_manage_RxInterrupt(&MIDIPort, huart);
 *            }
 *            @endcode
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     huart       pointer to the interrupted HAL UART handle
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td MIDI_manage_RxInterrupt(MIDI_structTd* MIDIPort, UART_HandleTypeDef *huart);

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
 * @name      Interaction
 * @brief     Use these functions to interact with the module
 * @{
 ******************************************************************************/

/**
 * @brief     Queue data to send a Note Off message.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     Midi Channel (1-16)
 * @param     Number      of the note (0-127)
 * @param     Velocity    of the note (0-127)
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td MIDI_queue_NoteOff(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number, uint8_t Velocity);

/**
 * @brief     Queue data to send a Note On message.
 * @param     MIDIPort    pointer to the users MIDI-Port data structure
 * @param     Channel     Midi Channel (0-15)
 * @param     Number      of the note (0-127)
 * @param     Velocity    of the note (0-127)
 * @return    MIDI_ERROR_NONE if everything is fine
 */
MIDI_error_Td MIDI_queue_NoteOn(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number, uint8_t Velocity);
/** @} ************************************************************************/
/* end of name "Interaction"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Get Functions
 * @brief     Use these functions to gt values
 * @{
 ******************************************************************************/

/** @} ************************************************************************/
/* end of name "Get Functions"
 ******************************************************************************/

/**@}*//* end of defgroup "MIDI_UART_Header" */
/**@}*//* end of defgroup "MIDI_UART" */

#endif /* INC_MIDIOVERSTLINK_H__MN */
