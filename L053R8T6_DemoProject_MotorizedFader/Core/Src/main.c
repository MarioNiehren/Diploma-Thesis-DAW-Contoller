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
#include "tim.h"
#include "tsc.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "wiper.h"
#include "tscButton.h"
#include "TB6612FNG_MotorDriver.h"
#include "pidController.h"
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
Wiper_structTd Wiper[2];
TSCButton_structTd TSCButton[2];
TB6612FNGMotorDriver_structTd Motor[2];
PID_structTd PID[2];
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
  /* USER CODE BEGIN 2 */
  Wiper_init_ADC(&Wiper[0], &hadc);
  Wiper_init_Hysteresis(&Wiper[0]);

  Wiper_init_ADC(&Wiper[1], &hadc);
  Wiper_init_Hysteresis(&Wiper[1]);

  TSCButton_init_TSC(&TSCButton[0], &htsc, TSC_GROUP1_IO2);
  TSCButton_init_Threshold(&TSCButton[0], 1490);

  TSCButton_init_TSC(&TSCButton[1], &htsc, TSC_GROUP3_IO3);
  TSCButton_init_Threshold(&TSCButton[1], 1610);

  TSCButton_init_DischargeTimeMsAll(1);

  MotorDriver_init_PinIn1(&Motor[0], GPIOA, GPIO_PIN_8);
  MotorDriver_init_PinIn2(&Motor[0], GPIOA, GPIO_PIN_9);
  MotorDriver_init_PinSTBY(&Motor[0], GPIOA, GPIO_PIN_12); /* Pin is shared with Motor[1] */

  MotorDriver_init_PWM(&Motor[0], &htim2, TIM_CHANNEL_1);

  MotorDriver_init_PinIn1(&Motor[1], GPIOA, GPIO_PIN_10);
  MotorDriver_init_PinIn2(&Motor[1], GPIOA, GPIO_PIN_11);
  MotorDriver_init_PinSTBY(&Motor[1], GPIOA, GPIO_PIN_12); /* Pin is shared with Motor[0] */

  MotorDriver_init_PWM(&Motor[1], &htim2, TIM_CHANNEL_2);

  Wiper_start_All();
  TSCButton_start_All();
  MotorDriver_start_PWM(&Motor[0]);
  MotorDriver_start_PWM(&Motor[1]);

  double Kp = 0.1;   /* old: 0.32 */
  double Ki = 0.001; /* old: 0.0038 */
  double Kd = 0.07;  /* old: 0.021 */

  PID_init(&PID[0]);
  PID_set_OutputMinMax(&PID[0], (double)-500, (double)500);
  PID_set_KpKiKd(&PID[0], Kp, Ki, Kd);
  PID_set_LowPass(&PID[0], 0.8);
  PID_set_SampleTimeInMs(&PID[0], 2);

  PID_set_Target(&PID[0], (double)1000);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    Wiper_update_All();
    TSCButton_update_All();
    uint16_t PosFader2 = Wiper_get_SmoothValue(&Wiper[1]);
    PID_set_Target(&PID[0], (double)PosFader2);
    TSCButton_State_enumTd StateButton1;
    StateButton1 = TSCButton_get_State(&TSCButton[0]);
    if(StateButton1 == TSCBUTTON_TOUCHED)
    {
      PID_reset(&PID[0]);
      MotorDriver_stop(&Motor[0]);
    }
    else if(StateButton1 == TSCBUTTON_RELEASED)
    {
      uint16_t ADCSample = Wiper_get_SmoothValue(&Wiper[0]);
      PID_update(&PID[0], (double)ADCSample);
      int CCR = PID_get_OutputRound(&PID[0]);

      int CCR_StartLimit = 130;
      int CCR_StopRange = 30;

      if(CCR < -CCR_StartLimit) /* 100: only start motor if PID is in the controllable value range. */
      {
        MotorDriver_move_CounterClockWise(&Motor[0], -1*CCR);
      }
      else if(CCR < -CCR_StopRange && CCR >= -CCR_StartLimit)
      {
        MotorDriver_move_CounterClockWise(&Motor[0], CCR_StartLimit);
      }
      else if(CCR > CCR_StartLimit)
      {
        MotorDriver_move_ClockWise(&Motor[0], CCR);
      }
      else if(CCR > CCR_StopRange && CCR <= CCR_StartLimit)
      {
       MotorDriver_move_ClockWise(&Motor[0], CCR_StartLimit);
      }
      else
      {
       MotorDriver_stop(&Motor[0]);
      }
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
  Wiper_manage_Interrupt(hadc);
}

void HAL_TSC_ConvCpltCallback(TSC_HandleTypeDef* htsc)
{
  TSCButton_manage_Interrupt();
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
