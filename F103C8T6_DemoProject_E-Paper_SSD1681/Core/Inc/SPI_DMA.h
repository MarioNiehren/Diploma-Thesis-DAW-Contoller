/***************************************************************************//**
 * @defgroup			EPD_SPI_Helper	SPI helper functions
 * @brief					This module contains helper functions for E-Paper specific SPI
 * 								transmission with DMA.
 *
 * DC and CS Pins will be handled by these helpers, so there is no need to handle
 * them manually. These functions are optimized for the use with SPI E-Paper
 * Modules.
 *
 * @addtogroup		EPD_Source
 * @{
 *
 * @addtogroup		EPD_SPI_Helper
 * @{
 *
 * @file				SPI_DMA.h
 *
 * @date				Oct 20, 2023
 * @author			Mario Niehren
 ******************************************************************************/

#ifndef MN_INTERFACE_SPI_DMA_H
#define MN_INTERFACE_SPI_DMA_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <string.h>

/**
 * @brief		Structure to store conditions for DC and CS Pins.
 */
typedef struct SPIDMA_GPIOs_TypeDef
{
  /* DC-Pin information */
  GPIO_TypeDef* GPIO_DC;
  uint16_t PIN_DC;
  GPIO_PinState DataState_DC;			/**< Required Pin state for Data Transmission is saved here */
  GPIO_PinState CommandState_DC;	/**< Required Pin state for Command Transmission is saved here */

  /* CS Pin information */
  GPIO_TypeDef* GPIO_CS;
  uint16_t PIN_CS;
  GPIO_PinState Active_CS;				/**< Required Pin state to activate the target device. */
  GPIO_PinState Inactive_CS;			/**< Required Pin state to deactivate the target device. */

}SPIDMA_GPIOs_TypeDef;

/**
 * @brief		Structure for the SPI Transmission
 */
typedef struct SPIDMA_TypeDef
{
  SPI_HandleTypeDef* hspi;
  SPIDMA_GPIOs_TypeDef Pins;
  bool Busy;
}SPIDMA_TypeDef;

/***************************************************************************//**
 * @name			Initialize
 * @{
 ******************************************************************************/

/**
 * @brief			Init Pin DC (Data/Command)
 *
 * @note			The pin will be initialized in data mode as start condition.
 *
 * @param			SPI		pointer to the users SPI structure (not the HAL-SPI handle!)
 * @param			Port	pointer to the GPIO Port of the DC Pin
 * @param			Pin		GPIO Pin of the DC Pin
 * @param			CommandState	Pin State that is required to be in Command mode.
 * 							@arg	GPIO_PIN_SET 		if command mode is high active
 * 							@arg	GPIO_PIN_RESET	if command mode is low active
 * @return		none
 */
void SPIDMA_init_PinDC(SPIDMA_TypeDef* SPI, GPIO_TypeDef* Port, uint16_t Pin, GPIO_PinState CommandState);

/**
 * @brief			Init Pin CS (Chip Select)
 *
 * @note			The pin will be initialized in inactive state as start condition.
 *
 * @param			SPI		pointer to the users SPI structure (not the HAL-SPI handle!)
 * @param			Port	pointer to the GPIO Port of the CS Pin
 * @param			Pin		GPIO Pin of the CS Pin
 * @param			ActiveState	Pin State that is required to activate the SPI target device..
 * 							@arg	GPIO_PIN_SET 		if the target device is high active
 * 							@arg	GPIO_PIN_RESET	if the target device low active
 * @return		none
 */
void SPIDMA_init_PinCS(SPIDMA_TypeDef* SPI, GPIO_TypeDef* Port, uint16_t Pin, GPIO_PinState ActiveState);

/**
 * @brief			Link the HAL hspi to the users SPI structure
 *
 * @param			SPI		pointer to the users SPI structure (not the HAL-SPI handle!)
 * @param			hspi	HAL-SPI handle used for SPI data transmission.
 */
void SPIDMA_init_SPIHandle(SPIDMA_TypeDef* SPI, SPI_HandleTypeDef* hspi);

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Transmit SPI
 * @{
 ******************************************************************************/

/**
 * @brief			Transmit Command
 * @param			SPI		pointer to the users SPI structure
 * @param			Data	pointer to the data block to be sent
 * @param			Length	size of the data block to be sent
 * @return		none
 */
void SPIDMA_transmit_Command(SPIDMA_TypeDef* SPI, uint8_t* Data, uint16_t Length);

/**
 * @brief			Transmit Data
 * @param			SPI		pointer to the users SPI structure
 * @param			Data	pointer to the data block to be sent
 * @param			Length	size of the data block to be sent
 * @return		none
 */
void SPIDMA_transmit_Data(SPIDMA_TypeDef* SPI, uint8_t* Data, uint16_t Length);

/**
 * @brief			Wait until SPI transmission is complete
 * @warning		This function will slow down your code. Try to avoid using it
 * @param			SPI		pointer to the users SPI structure
 * @return		none
 */
void SPIDMA_wait_WhileTransmitting(SPIDMA_TypeDef* SPI);

/** @} ************************************************************************/
/* end of name "Transmit SPI"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Interrupt management
 * @{
 ******************************************************************************/

/**
 * @brief			Function to manage interrupt. Resets CS and DC Pins
 * @param			SPI		pointer to the users SPI structure
 * @param			hspi	pointer to interrupted HAL-SPI handle
 */
void SPIDMA_manage_Interrupt(SPIDMA_TypeDef* SPI, SPI_HandleTypeDef* hspi);

/** @} ************************************************************************/
/* end of name "Interrupt management"
 ******************************************************************************/

/**@}*//* end of defgroup "EPD_SPI_Helper" */
/**@}*//* end of defgroup "EPaperSSD1681" */

#endif /* MN_INTERFACE_SPI_DMA_H */
