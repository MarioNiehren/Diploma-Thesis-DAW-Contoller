/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MIDI_UART.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* structures to handle HUI-Commands */
typedef struct
{
  uint8_t Zone;
  uint8_t PortOn;
  uint8_t PortOff;
}HUI_Rx_Td;

typedef struct
{
  uint8_t Zone;
  uint8_t Port;
  bool ToggleOn;
  bool ToggleOff;
}HUI_solo_td;

/* Instances of the previous declared structures */
HUI_Rx_Td HUIRx = {
    .Zone = 0xFF,
    .PortOn = 0xFF,
    .PortOff = 0xFF,
};

HUI_solo_td SoloCh1 = {
    .Zone = 0x00,
    .Port = 0x03,
    .ToggleOn = false,
    .ToggleOff = false,
};

/* used MIDI instance */
MIDI_structTd MIDIPort1;

/* used to read nucleo user button */
GPIO_PinState PrevButtonState = GPIO_PIN_SET;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HUI_send_SwitchCommandOn(MIDI_structTd* MIDIPort, uint8_t Zone, uint8_t Port);
void HUI_send_SwitchCommandOff(MIDI_structTd* MIDIPort, uint8_t Zone, uint8_t Port);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  MIDI_init_UART(&MIDIPort1, &huart2);
  MIDI_init_DMARxHandle(&MIDIPort1, &hdma_usart2_rx);
  MIDI_start_Transmission(&MIDIPort1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    MIDI_update_Transmission(&MIDIPort1);

    GPIO_PinState ButtonState = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
    if(ButtonState == GPIO_PIN_RESET)
    {
      if(ButtonState != PrevButtonState)
      {
        PrevButtonState = GPIO_PIN_RESET;
        HUI_send_SwitchCommandOn(&MIDIPort1, SoloCh1.Zone, SoloCh1.Port);
      }
    }
    else
    {
      if(ButtonState != PrevButtonState)
      {
        PrevButtonState = GPIO_PIN_SET;
        HUI_send_SwitchCommandOff(&MIDIPort1, SoloCh1.Zone, SoloCh1.Port);
      }
    }

    if(SoloCh1.ToggleOn == true)
    {
      HAL_GPIO_WritePin(LED_ON_BOARD_GPIO_Port, LED_ON_BOARD_Pin, GPIO_PIN_SET);
      SoloCh1.ToggleOn = false;
    }

    if(SoloCh1.ToggleOff == true)
    {
      HAL_GPIO_WritePin(LED_ON_BOARD_GPIO_Port, LED_ON_BOARD_Pin, GPIO_PIN_RESET);
      SoloCh1.ToggleOff = false;
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HUI_send_SwitchCommandOn(MIDI_structTd* MIDIPort, uint8_t Zone, uint8_t Port)
{
  uint8_t Channel = 0x00;
  uint8_t ZoneSelect = 0x0F;
  uint8_t PortSelect = 0x2F;
  uint8_t PortOffset = 0x40;
  uint8_t PortValue = Port + PortOffset;

  MIDI_queue_ControlChange(&MIDIPort1, Channel, ZoneSelect, Zone);
  MIDI_queue_ControlChange(&MIDIPort1, Channel, PortSelect, PortValue);
}

void HUI_send_SwitchCommandOff(MIDI_structTd* MIDIPort, uint8_t Zone, uint8_t Port)
{
  uint8_t Channel = 0x00;
  uint8_t ZoneSelect = 0x0F;
  uint8_t PortSelect = 0x2F;

  MIDI_queue_ControlChange(&MIDIPort1, Channel, ZoneSelect, Zone);
  MIDI_queue_ControlChange(&MIDIPort1, Channel, PortSelect, Port);
}

void MIDI_callback_NoteOn(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Note, uint8_t Velocity)
{
  if(MIDIPort == &MIDIPort1 && Channel == 0x00 && Note == 0x00 && Velocity == 0x00)
  {
    /* Answer Ping */
    uint8_t PingChannel = 0x00;
    uint8_t PingByte1  = 0x00;
    uint8_t PingByte2 = 0x7F;
    MIDI_queue_NoteOn(&MIDIPort1, PingChannel, PingByte1, PingByte2);
  }
}

void MIDI_callback_ControlChange(MIDI_structTd* MIDIPort, uint8_t Channel, uint8_t Number, uint8_t Value)
{
  if(MIDIPort == &MIDIPort1)
  {
    const uint8_t HUI_Channel = 0x00;
    const uint8_t HUI_ZoneSelect = 0x0C;
    const uint8_t HUI_PortSelect = 0x2C;
    const uint8_t HUI_Port_ValueMsk = 0x40;
    const uint8_t HUI_Port_NumberMsk = 0xF0;
    const uint8_t HUI_ResetValue = 0xFF;

    if(Channel == HUI_Channel)
    {
      /* Get HUI-Zone*/
      if(Number == HUI_ZoneSelect)
      {
        HUIRx.Zone = Value;
      }
      /* Get HUI-Port */
      if(Number == HUI_PortSelect)
      {
        if((Value & HUI_Port_ValueMsk) == HUI_Port_ValueMsk)
        {
          /* Extract port number from byte 0x4n */
          HUIRx.PortOn = Value & ~HUI_Port_NumberMsk;
        }
        else
        {
          /* Extract port number from byte 0x0n */
          HUIRx.PortOff = Value & ~HUI_Port_NumberMsk;
        }
      }

      /* set Solo Flag */
      if(HUIRx.Zone == SoloCh1.Zone)
      {
        if(HUIRx.PortOn == SoloCh1.Port)
        {
          SoloCh1.ToggleOn = true;
          HUIRx.Zone = HUI_ResetValue;
          HUIRx.PortOn = HUI_ResetValue;
        }
        if(HUIRx.PortOff == SoloCh1.Port)
        {
          SoloCh1.ToggleOff = true;
          HUIRx.Zone = HUI_ResetValue;
          HUIRx.PortOff = HUI_ResetValue;
        }
      }
    }
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  MIDI_manage_RxInterrupt(&MIDIPort1, huart, Size);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  MIDI_manage_TxInterrupt(&MIDIPort1, huart);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
