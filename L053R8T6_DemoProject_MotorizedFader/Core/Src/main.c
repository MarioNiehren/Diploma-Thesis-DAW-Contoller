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
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "tsc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motorizedFader.h"
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

MotorizedFader_structTd Fader[2];
uint8_t NumFaders = 2;
uint16_t FaderValue[2];
uint16_t PrevFaderValue[2];
TSCButton_State_enumTd FaderState[2];

uint32_t timNow = 0;
uint32_t timOld = 0;
uint32_t WhileSpeed = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_TSC_Init();
  MX_ADC_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  /* Fader Values */
  /* TSC */
  uint16_t TouchThreshold[2] = {450, 450};

  /* PID */
  double Kp = 0.15;   /* old: 0.32 */
  double Ki = 0.0001; /* old: 0.0038 */
  double Kd = 0.025;  /* old: 0.021 */
  double TauLowPass = 0.1;
  uint32_t PIDSampelTime = 3;

  /* PWM Limits */
  uint16_t CCRLimit = 500;
  int StartForceCCR = 120;
  int StopRangeCCR = 25;

  /* Initialize general fader settings */
  MotorizedFader_init_Structure(&Fader[0]);
  MotorizedFader_init_StartForce(&Fader[0], StartForceCCR);
  MotorizedFader_init_StopRange(&Fader[0], StopRangeCCR);

  MotorizedFader_init_Structure(&Fader[1]);
  MotorizedFader_init_StartForce(&Fader[1], StartForceCCR);
  MotorizedFader_init_StopRange(&Fader[1], StopRangeCCR);

  /* Initialize faders wiper */
  MotorizedFader_init_Wiper(&Fader[0], &hadc);
  MotorizedFader_init_Wiper(&Fader[1], &hadc);

  /* Initialize faders TSC */
  MotorizedFader_init_TouchTSC(&Fader[0], &htsc, TSC_GROUP1_IO2);
  MotorizedFader_init_TouchThreshold(&Fader[0], TouchThreshold[0]);

  MotorizedFader_init_TouchTSC(&Fader[1], &htsc, TSC_GROUP3_IO3);
  MotorizedFader_init_TouchThreshold(&Fader[1], TouchThreshold[1]);

  MotorizedFader_init_TouchDischargeTimeMsAll(2);

  /* Initialize faders Motor */
  MotorizedFader_init_MotorPinIn1(&Fader[0], GPIOA, GPIO_PIN_8);
  MotorizedFader_init_MotorPinIn2(&Fader[0], GPIOA, GPIO_PIN_9);
  MotorizedFader_init_MotorPinSTBY(&Fader[0], GPIOA, GPIO_PIN_12); /* Pin is shared with Fader[1] */
  MotorizedFader_init_MotorPWM(&Fader[0], &htim2, TIM_CHANNEL_1);

  MotorizedFader_init_MotorPinIn1(&Fader[1], GPIOA, GPIO_PIN_10);
  MotorizedFader_init_MotorPinIn2(&Fader[1], GPIOA, GPIO_PIN_11);
  MotorizedFader_init_MotorPinSTBY(&Fader[1], GPIOA, GPIO_PIN_12); /* Pin is shared with Fader[0] */
  MotorizedFader_init_MotorPWM(&Fader[1], &htim2, TIM_CHANNEL_2);

  /* Initialize faders PID */
  MotorizedFader_init_PID(&Fader[0]);
  MotorizedFader_init_PIDMaxCCR(&Fader[0], CCRLimit);
  MotorizedFader_init_PIDKpKiKd(&Fader[0], Kp, Ki, Kd);
  MotorizedFader_init_PIDLowPass(&Fader[0], TauLowPass);
  MotorizedFader_init_PIDSampleTimeInMs(&Fader[0], PIDSampelTime);

  MotorizedFader_init_PID(&Fader[1]);
  MotorizedFader_init_PIDMaxCCR(&Fader[1], CCRLimit);
  MotorizedFader_init_PIDKpKiKd(&Fader[1], Kp, Ki, Kd);
  MotorizedFader_init_PIDLowPass(&Fader[1], TauLowPass);
  MotorizedFader_init_PIDSampleTimeInMs(&Fader[1], PIDSampelTime);

  MotorizedFader_start_All();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* check loop speed */
    timNow = HAL_GetTick();
    WhileSpeed = timNow - timOld;
    timOld = timNow;

    MotorizedFader_update_All();

    /* Link Faders */
    for(uint8_t Index = 0; Index < NumFaders;  Index++)
    {
      /* store previous fader value */
      PrevFaderValue[Index] = FaderValue[Index];

      /* Get current fader value */
      FaderValue[Index] = MotorizedFader_get_WiperValue(&Fader[Index]);
      FaderState[Index] = MotorizedFader_get_TSCState(&Fader[Index]);
    }

    if(FaderState[1] == TSCBUTTON_TOUCHED)
    {
      MotorizedFader_set_Target(&Fader[0], FaderValue[1]);
      MotorizedFader_set_Target(&Fader[1], FaderValue[1]);
    }

    if(FaderState[0] == TSCBUTTON_TOUCHED)
    {
      MotorizedFader_set_Target(&Fader[1], FaderValue[0]);
      MotorizedFader_set_Target(&Fader[0], FaderValue[0]);
    }
    /* Link Faders End */

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/***************************************************************************//**
 * @name			Interrupt Handlers
 * @brief			Here are all Interrupt and callback funcitons.
 * @{
 ******************************************************************************/

/* details about this callback in: stm32l0xx_hal_adc.c */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  MotorizedFader_manage_WiperInterrupt(hadc);
}

void HAL_TSC_ConvCpltCallback(TSC_HandleTypeDef* htsc)
{
  MotorizedFader_manage_TSCInterrupt();
}

/** @} ************************************************************************/
/* end of name "Interrupt Handlers"
 ******************************************************************************/
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
