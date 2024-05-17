/***************************************************************************//**
 * @addtogroup		EPD_SPI_Helper
 *
 * @file			SPI_DMA.c
 *
 * @date			Oct 20, 2023
 * @author		Mario Niehren
 ******************************************************************************/

#include "SPI_DMA.h"

/***************************************************************************//**
 * @name			Initialize
 * @{
 ******************************************************************************/

void SPIDMA_init_PinDC(SPIDMA_StructTd* SPI, GPIO_TypeDef* Port, uint16_t Pin, GPIO_PinState CommandState)
{
	/** @internal		1.	Save DC Port to SPI structure */
  SPI->Pins.GPIO_DC = Port;
  /** @internal		2.	Save DC Pin to SPI structure */
  SPI->Pins.PIN_DC = Pin;
  /** @internal		3.	Save the state the DC pin needs, to be in command state.  */
  SPI->Pins.CommandState_DC = CommandState;

  /** @internal		4.	Check CommandState and set Data State of the structure to the opposite. */
  if(CommandState == GPIO_PIN_SET)
  {
    SPI->Pins.DataState_DC = GPIO_PIN_RESET;
  }
  else if(CommandState == GPIO_PIN_RESET)
  {
    SPI->Pins.DataState_DC = GPIO_PIN_SET;
  }

  /**	@internal		5.	Set Pin to data state */
  HAL_GPIO_WritePin(SPI->Pins.GPIO_DC, SPI->Pins.PIN_DC, SPI->Pins.DataState_DC);
}

void SPIDMA_init_PinCS(SPIDMA_StructTd* SPI, GPIO_TypeDef* Port, uint16_t Pin, GPIO_PinState ActiveState)
{
	/** @internal		1.	Save CS Port to SPI structure */
  SPI->Pins.GPIO_CS = Port;
  /** @internal		2.	Save CS Pin to SPI structure */
  SPI->Pins.PIN_CS = Pin;
  /** @internal		3.	Save the state that is required to be active. */
  SPI->Pins.Active_CS = ActiveState;

  /** @internal		4.	Check ActiveState and set Inactive State of the structure to the opposite. */
  if(ActiveState == GPIO_PIN_SET)
  {
    SPI->Pins.Inactive_CS = GPIO_PIN_RESET;
  }
  else if(ActiveState == GPIO_PIN_RESET)
  {
    SPI->Pins.Inactive_CS = GPIO_PIN_SET;
  }

  /**	@internal		5.	Set Pin to inactive state */
  HAL_GPIO_WritePin(SPI->Pins.GPIO_CS, SPI->Pins.PIN_CS, SPI->Pins.Inactive_CS);
}

void SPIDMA_init_SPIHandle(SPIDMA_StructTd* SPI, SPI_HandleTypeDef* hspi)
{
	/** @internal		1. Store HAL hspi pointer to the users SPI structure. */
  SPI->hspi = hspi;
}

/** @} ************************************************************************/
/* end of name "Initialize"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Transmit SPI
 * @{
 ******************************************************************************/

void SPIDMA_transmit_Command(SPIDMA_StructTd* SPI, uint8_t* Data, uint16_t Length)
{
	/** @internal			1.	Set CS-Pin to active state */
  HAL_GPIO_WritePin(SPI->Pins.GPIO_CS, SPI->Pins.PIN_CS, SPI->Pins.Active_CS);
  /** @internal			2.	Set DC-Pin to command state*/
  HAL_GPIO_WritePin(SPI->Pins.GPIO_DC, SPI->Pins.PIN_DC, SPI->Pins.CommandState_DC);
  /** @internal			3.	Set SPI busy flag (this will be reset by interrupt management) */
  SPI->Busy = true;
  /** @internal			4.	Start SPI transmission on DMA */
  HAL_SPI_Transmit_DMA(SPI->hspi, Data, Length);
}

void SPIDMA_transmit_Data(SPIDMA_StructTd* SPI, uint8_t* Data, uint16_t Length)
{
	/** @internal			1.	Set CS-Pin to active state */
  HAL_GPIO_WritePin(SPI->Pins.GPIO_CS, SPI->Pins.PIN_CS, SPI->Pins.Active_CS);
  /** @internal			2.	Set DC-Pin to data state*/
  HAL_GPIO_WritePin(SPI->Pins.GPIO_DC, SPI->Pins.PIN_DC, SPI->Pins.DataState_DC);
  /** @internal			3.	Set SPI busy flag (this will be reset by interrupt management) */
  SPI->Busy = true;
  /** @internal			4.	Start SPI transmission on DMA */
  HAL_SPI_Transmit_DMA(SPI->hspi, Data, Length);
}

void SPIDMA_wait_WhileTransmitting(SPIDMA_StructTd* SPI)
{
	/** @internal			1.	Stay in while loop until SPI is not busy anymore */
  while(SPI->Busy == true)
  {
    ;
  }
}

/** @} ************************************************************************/
/* end of name "Transmit SPI"
 ******************************************************************************/


/***************************************************************************//**
 * @name			Interrupt management
 * @{
 ******************************************************************************/

void SPIDMA_manage_Interrupt(SPIDMA_StructTd* SPI, SPI_HandleTypeDef* hspi)
{
	/** @internal			1.	Check if interrupted HAL-SPI handle is valid. Leave function if not. */
  if(hspi->Instance == SPI->hspi->Instance)
  {
    /** @internal			2.	Reset SPI Busy flag */
    SPI->Busy = false;
    /** @internal			3. Set CS Pin to inactive */
    HAL_GPIO_WritePin(SPI->Pins.GPIO_CS, SPI->Pins.PIN_CS, SPI->Pins.Inactive_CS);
    /** @internal			4.	(Re)set DC Pin to data state as default */
    HAL_GPIO_WritePin(SPI->Pins.GPIO_DC, SPI->Pins.PIN_DC, SPI->Pins.DataState_DC);
  }
}

/** @} ************************************************************************/
/* end of name "Interrupt management"
 ******************************************************************************/
