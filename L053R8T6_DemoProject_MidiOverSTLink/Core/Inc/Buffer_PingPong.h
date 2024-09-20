/***************************************************************************//**
 * @defgroup				Buffer_PingPong     Ping-Pong Buffer used for Data Transfer
 * @brief
 *
 * @defgroup        Buffer_PingPong_Header    Header
 * @brief						Study this part for a quick overview of this module.
 *
 * @addtogroup      MIDI_UART
 * @{
 *
 * @addtogroup			Buffer_PingPong
 * @{
 *
 * @addtogroup      Buffer_PingPong_Header
 * @{
 *
 * @file            Buffer_PingPong.h
 *
 * @date            Aug 12, 2024
 * @author          Mario
 ******************************************************************************/

#ifndef INC_BUFFER_PINGPONG_H__MN
#define INC_BUFFER_PINGPONG_H__MN

#include <stddef.h>
#include <stdint.h>

/**
 * @brief   Define size of the RX Buffers here. Max Value: 65535 - 1 (16Bit,
 *          0xFFFF is reserved for Error).
 */
#define BUFFER_PINGPONG_RX_MAX  255

/**
 * @brief   Define size of the TX Buffers here. Max Value: 65535 - 1 (16Bit,
 *          0xFFFF is reserved for Error).
 */
#define BUFFER_PINGPONG_TX_MAX  255

/**
 * @brief   Value that is subtracted from value ranges to reserve the binary
 *          max vale (0xFF..) for errors.
 *
 */
#define BUFFER_PING_PONG_ERROR_VALUE_RESERVE  1

/***************************************************************************//**
 * @name      Structure and Enumerations
 * @{
 ******************************************************************************/

/**
 * @brief     Enumerations to describe the buffer and buffer-internal states.
 */
typedef enum
{
  BUFFER_PINGPONG_NONE = 0x00,

  BUFFER_PINGPONG_TX_A = 0x10,
  BUFFER_PINGPONG_TX_B = 0x11,
  BUFFER_PINGPONG_RX_A = 0x12,
  BUFFER_PINGPONG_RX_B = 0x13,
}BufferPingPong_Td;

typedef enum
{
  BUFFER_PINGPONG_ERROR_NONE = 0x00,

  BUFFER_PINGPONG_ERROR_TX_A_OVERFLOW = 0x10,
  BUFFER_PINGPONG_ERROR_TX_B_OVERFLOW = 0x11,
  BUFFER_PINGPONG_ERROR_RX_A_OVERFLOW = 0x12,
  BUFFER_PINGPONG_ERROR_RX_B_OVERFLOW = 0x13,

  BUFFER_PINGPONG_ERROR_NO_BUFFER_FOUND = 0x20,

  BUFFER_PINGPONG_ERROR_TOGGLE_RX_FAILED = 0x30,
  BUFFER_PINGPONG_ERROR_TOGGLE_TX_FAILED = 0x31,

  BUFFER_PINGPONG_ERROR_RX_MAX_TOO_HIGH = 0x40,
  BUFFER_PINGPONG_ERROR_TX_MAX_TOO_HIGH = 0x41,

  BUFFER_PINGPONG_ERROR_16BIT_RANGE_OVERFLOW = 0x50,

}BufferPingPong_error_Td;

/**
 * @brief     Structure to buffer data for DMA Transmission. The buffering
 *            works with a ping-pong principle.
 * @note      The user does not need to setup this data structure manually. It
 *            is part of the main MIDI structure.
 */
typedef struct
{
  BufferPingPong_Td LockedToReceive;  /**< buffer that is currently locked for
                                           the user*/
  BufferPingPong_Td LockedToSend;     /**< buffer that is currently locked for
                                           the user */

  uint8_t RxA[BUFFER_PINGPONG_RX_MAX]; /**< Array A to buffer Rx bytes */
  uint16_t RxAIndex;                   /**< Index counter for RxA-Array */

  uint8_t RxB[BUFFER_PINGPONG_RX_MAX]; /**< Array B to buffer Rx bytes */
  uint16_t RxBIndex;                   /**< Index counter for RxB-Array */

  uint8_t TxA[BUFFER_PINGPONG_TX_MAX]; /**< Array A to buffer Tx bytes */
  uint16_t TxAIndex;                   /**< Index counter for TxA-Array */

  uint8_t TxB[BUFFER_PINGPONG_TX_MAX]; /**< Array B to buffer Tx bytes */
  uint16_t TxBIndex;                   /**< Index counter for TxB-Array */

}BufferPingPong_structTd;
/** @} ************************************************************************/
/* end of name "Structure and Enumerations"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Initialize
 * @brief     Use these functions to initialize this module.
 * @{
 ******************************************************************************/

/**
 * @brief     Call this function once in the beginning to initialize the start
 *            conditions of the buffers data structure. BUFFER_PINGPONG_RX_MAX
 *            and BUFFER_PINGPONG_TX_MAX will be checked, if the value is valid.
 * @param     Buffer      pointer to the users Buffer
 * @return    BUFFER_PINGPONG_NONE if everything is fine.
 */
BufferPingPong_error_Td BufferPingPong_init_StartConditions(BufferPingPong_structTd* Buffer);

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Rx Buffers
 * @brief     This function are used to handle the Rx Buffers
 * @{
 ******************************************************************************/

/**
 * @brief     Get the start point of the first free place in the Buffer Array,
 *            that is ready to receive.
 * @param     Buffer      pointer to the users Buffer
 * @return    Start pointer of the buffer to be send, or NULL in case of an
 *            error
 */
uint8_t* BufferPingPong_get_RxStartPtrForTransmission(BufferPingPong_structTd* Buffer);

/**
 * @brief     Toggle the buffer that is used to receive data, so the other one
 *            gets free for processing.
 * @param     Buffer      pointer to the users Buffer
 * @return    BUFFER_PINGPONG_NONE if everything is fine
 */
BufferPingPong_error_Td BufferPingPong_toggle_RxBuffer(BufferPingPong_structTd* Buffer);

/**
 * @brief     Get the size of the currently locked Rx Buffer to handle data
 *            in the receiving side.
 * @param     Buffer      pointer to the users Buffer
 * @return    size of the buffered data. 0xFFFF in case of an error.
 */
uint16_t BufferPingPong_get_SizeOfLockedRxBuffer(BufferPingPong_structTd* Buffer);

/**
 * @brief     Get the start point of the RxArray that is currently locked to
 *            receive data.
 * @param     Buffer      pointer to the users Buffer
 * @return    Start Pointer of the error. NULL in case of an error.
 */
uint8_t* ButterPingPong_get_StartPtrOfLockedRxBuffer(BufferPingPong_structTd* Buffer);

/**
 * @brief     Use this function to queue received data to the buffer that is
 *            currently locked for the user.
 * @param     Buffer      pointer to the users Buffer
 * @param     Data        pointer to the data to be buffered
 * @param     Size        Number of bytes to be buffered
 * @return    BUFFER_PINGPONG_NONE if everything is fine
 */
BufferPingPong_error_Td BufferPingPong_queue_RxBytesForTransmission(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size);

/**
 * @brief     Use this function to count up the Index of the currently used
 *            RxBuffer.
 * @param     Buffer      pointer to the users Buffer
 * @param     size is the number that will be added to the index
 * @return    BUFFER_PINGPONG_NONE if everything is fine
 */
BufferPingPong_error_Td BufferPingPong_increase_RxBufferIndex(BufferPingPong_structTd* Buffer, uint16_t size);

/** @} ************************************************************************/
/* end of name "Rx Buffers"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Tx Buffers
 * @brief     This function are used to handle the Tx Buffers
 * @{
 ******************************************************************************/

/**
 * @brief     Get the start point of the first free place in the Buffer Array,
 *            that is ready to send.
 * @param     Buffer      pointer to the users Buffer
 * @return    Start pointer of the buffer to be send, or NULL in case of an
 *            error
 */
uint8_t* BufferPingPong_get_TxStartPtrForTransmission(BufferPingPong_structTd* Buffer);

/**
 * @brief     Toggle the buffer that is used to send data, so the other one
 *            gets free for processing.
 * @param     Buffer      pointer to the users Buffer
 * @return    BUFFER_PINGPONG_NONE if everything is fine
 */
BufferPingPong_error_Td BufferPingPong_toggle_TxBuffer(BufferPingPong_structTd* Buffer);

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
uint16_t BufferPingPong_get_TxSizeForTransmission(BufferPingPong_structTd* Buffer);

/**
 * @brief     Use this function to queue data for transmission to the buffer
 *            that is currently not locked for the user.
 * @param     Buffer      pointer to the users Buffer
 * @param     Data        pointer to the data to be buffered
 * @param     Size        Number of bytes to be buffered
 * @return    BUFFER_PINGPONG_NONE if everything is fine
 */
BufferPingPong_error_Td BufferPingPong_queue_TxBytesForTransmission(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size);

/** @} ************************************************************************/
/* end of name "Tx Buffers"
 ******************************************************************************/







/**@}*//* end of defgroup "Buffer_PingPong_Header" */
/**@}*//* end of defgroup "Buffer_PingPong" */
/**@}*//* end of defgroup "MIDI_UART" */

#endif /* INC_BUFFER_PINGPONG_H__MN */
