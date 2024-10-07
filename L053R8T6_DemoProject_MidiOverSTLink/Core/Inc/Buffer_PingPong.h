/***************************************************************************//**
 * @defgroup				Buffer_PingPong     Ping-Pong Buffer used for Data Transfer
 * @brief
 *
 * # Structure of the Buffer implementation.
 *
 * | Layer        | Process         | Fill      | Details                                             |
 * | ------------ | --------------- | --------- | --------------------------------------------------- |
 * | Application  | Get Rx data     | Tx buffer | data will be accessed by user here, e.g. in an update function |
 * | Transmission | Send Tx data    | Rx buffer | data will actually be received and transmitted here, e.g. when the I/O Hardware gets directly accessed |
 *
 * @defgroup        Buffer_PingPong_Header    Header
 * @brief						Study this part for a quick overview of this module.
 *
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
#include <stdbool.h>

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
 * @brief   Define size of the RX Buffer headroom. It is used to buffer byte
 *          by byte until an expected Data Block is full. Then it will be
 *          latched to the buffer.
 */
#define BUFFER_PINGPONG_RX_HEADROOM  100

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
  BUFFER_PINGPONG_ERROR_RX_HEADROOM_TOO_HIGH = 0x41,

  BUFFER_PINGPONG_ERROR_16BIT_RANGE_OVERFLOW = 0x50,

  BUFFER_PINGPONG_OVERFLOW = 0xFF

}BufferPingPong_error_Td;

/**
 * @brief     Structure to buffer data for DMA Transmission. The buffering
 *            works with a ping-pong principle.
 * @note      The user does not need to setup this data structure manually. It
 *            is part of the main MIDI structure.
 */
typedef struct
{
  BufferPingPong_Td ReservedToReceive;  /**< buffer that is currently locked for
                                           the user*/
  BufferPingPong_Td ReservedToSend;     /**< buffer that is currently locked for
                                           the user */

  uint8_t RxA[BUFFER_PINGPONG_RX_MAX]; /**< Array A to buffer Rx bytes */
  uint16_t RxAIndex;                   /**< Index counter for RxA-Array */

  uint8_t RxB[BUFFER_PINGPONG_RX_MAX]; /**< Array B to buffer Rx bytes */
  uint16_t RxBIndex;                   /**< Index counter for RxB-Array */

  uint8_t RxHeadroom[BUFFER_PINGPONG_RX_HEADROOM]; /**< Array to buffer single
                                            Rx Bytes. */
  uint16_t RxHeadroomIndex;            /**< Index counter for Rx Headroom */

  uint8_t TxA[BUFFER_PINGPONG_TX_MAX]; /**< Array A to buffer Tx bytes */
  uint16_t TxAIndex;                   /**< Index counter for TxA-Array */

  uint8_t TxB[BUFFER_PINGPONG_TX_MAX]; /**< Array B to buffer Tx bytes */
  uint16_t TxBIndex;                   /**< Index counter for TxB-Array */

  bool RxBufferToggled;
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
 * @name      Rx Buffer on Application Side
 * @brief     This function are used to access the Buffer in the users
 *            application.
 *
 * # How to implement
 * 1. Check if new data is available (This has to be checked manually, depending
 *    on the way, how the data reception works in the applications environment).
 *    Continue, if new data is available.
 * 2. Get the start pointer of the index, that was filled with data from the
 *    transmission side (BufferPingPong_fetch_SizeOfFilledRxBuffer())
 * 3. Get the size of data inside this buffer
 *    (ButterPingPong_fetch_StartPtrOfFilledRxBuffer())
 * 4. toggle the Rx Buffer, so the transmitter can fill the other buffer, while
 *    the filled buffer is  in use. (BufferPingPong_toggle_RxBuffer())
 *
 * @note      The Rx Buffer must not be toggled before the start pointer and the
 *            buffer size are fetched, otherwise the return of these functions
 *            will be wrong.
 * @{
 ******************************************************************************/

/**
 * @brief     Get the start point of the RxArray that contains new data.
 * @param     Buffer      pointer to the users Buffer
 * @return    Start Pointer of the error. NULL in case of an error.
 */
uint8_t* ButterPingPong_fetch_StartPtrOfFilledRxBuffer(BufferPingPong_structTd* Buffer);

/**
 * @brief     Get the size of the Rx Buffer that contains new data
 * @return    size of the buffered data. 0xFFFF in case of an error.
 */
uint16_t BufferPingPong_fetch_SizeOfFilledRxBuffer(BufferPingPong_structTd* Buffer);

/**
 * @brief     Toggle the buffer that is used to receive data, so the filled one
 *            gets free to use. Make sure to fetch the Start pointer and size
 *            of the filled buffer before toggling!
 * @param     Buffer      pointer to the users Buffer
 * @return    BUFFER_PINGPONG_NONE if everything is fine
 */
BufferPingPong_error_Td BufferPingPong_toggle_RxBuffer(BufferPingPong_structTd* Buffer);

/** @} ************************************************************************/
/* end of name " Rx Buffer on Application Side"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Rx Buffer on Transmission Side
 * @brief     This function are used to handle the Rx Buffers on transmission
 *            side. This can be anything, that actually receives data bytes and
 *            stores them in an any buffer.
 *
 * # How to implement
 * 1. Latch the temporary data, that contains data from the previous Rx Cycle,
 *    to the regular Rx Buffer. The temporary buffer can get overwritten now.
 *    (This step should be skipped for the first rx cycle, because the temporary
 *    buffer will be empty)
 * 2. Get the start pointer of the temporary data buffer. This buffer will be
 *    used to store data, until a reception cycle is finished.
 * 3. Get the Size of the maximum size of the temporary buffer to avoid
 *    overflow
 * 4. Use the received pointer and size to initiate a new data reception cycle.
 *    The following received data should be stored here.
 *
 * @{
 ******************************************************************************/

/**
 * @brief     Copy all received data from the temporary buffer to the regular
 *            buffer. After latching, the temporary buffer is ready to be
 *            filled again.
 * @param     Buffer      pointer to the users Buffer
 */
BufferPingPong_error_Td BufferPingPong_latch_TempRxBufferToRegularRxBuffer(BufferPingPong_structTd* Buffer, uint16_t Size);

/**
 * @brief     Get the start pointer of the temporary data buffer.
 * @param     Buffer      pointer to the users Buffer
 * @return    pointer to the temporary Rx buffer
 */
uint8_t* BufferPingPong_get_StartPtrOfTempRxBuffer(BufferPingPong_structTd* Buffer);

/**
 * @brief     Get the maximum size of the temporary data buffer.
 * @param     This function has no argument, because the maximum size is
 *            a global define. This value counts for all buffer instances.
 * @return    size of the temporary buffer (this is the value of
 *            BUFFER_PINGPONG_RX_HEADROOM)
 */
uint16_t BufferPingPong_get_SizeOfTempRxBuffer();

/** @} ************************************************************************/
/* end of name "Rx Buffer on Transmission Side"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Tx Buffer on Application Side
 * @brief     This function are used to handle the Tx Buffers
 * @{
 ******************************************************************************/

/**
 * @brief     Use this function to queue data for transmission to the buffer
 *            that is free to be filled
 * @param     Buffer      pointer to the users Buffer
 * @param     Data        pointer to the data to be buffered
 * @param     Size        Number of bytes to be buffered
 * @return    BUFFER_PINGPONG_NONE if everything is fine
 */
BufferPingPong_error_Td BufferPingPong_queue_TxBytesToEmptyBuffer(BufferPingPong_structTd* Buffer, uint8_t* Data, uint8_t Size);

/** @} ************************************************************************/
/* end of name "Tx Buffer on Application Side"
 ******************************************************************************/


/***************************************************************************//**
 * @name      Tx Buffer on Transmission Side
 * @brief     This function are used to handle the Tx Buffers
 *
 * # How to implement
 *
 * 1. Get the start pointer of the Tx Buffer, that is filled and should be sent
 * 2. Get the Size of this buffer
 * 3. Toggle the Tx buffers
 * 4. Use the Pointer and Size to start the new send cycle.
 *
 * @{
 ******************************************************************************/

/**
 * @brief     Get the start point of the first free place in the Buffer Array,
 *            that is ready to send.
 * @param     Buffer      pointer to the users Buffer
 * @return    Start pointer of the buffer to be send, or NULL in case of an
 *            error
 */
uint8_t* BufferPingPong_get_TxStartPtrOfFilledBuffer(BufferPingPong_structTd* Buffer);

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
 * @brief     Toggle the buffer that is used to send data, so the other one
 *            gets free for processing.
 * @param     Buffer      pointer to the users Buffer
 * @return    BUFFER_PINGPONG_NONE if everything is fine
 */
BufferPingPong_error_Td BufferPingPong_toggle_TxBuffer(BufferPingPong_structTd* Buffer);

/** @} ************************************************************************/
/* end of name "Tx Buffer on Application Side"
 ******************************************************************************/

/**@}*//* end of defgroup "Buffer_PingPong_Header" */
/**@}*//* end of defgroup "Buffer_PingPong" */
/**@}*//* end of defgroup "MIDI_UART" */

#endif /* INC_BUFFER_PINGPONG_H__MN */
