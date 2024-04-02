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

/**
 * @defgroup			MainDemo		Application Example
 * @brief					This is an application example for the button matrix.
 *
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/**
 * @addtogroup		MainDemo
 * @{
 */
/**
 * @brief			Includes we need for our project.
 */
#include "buttonMatrix.h"
#include <stdbool.h>
/** @} */

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @addtogroup		MainDemo
 *
 * @{
 */

/**
 * @name			Debugging Variables
 *
 * @brief			Use these variables to see if the code works by watching them in Live Expressions.
 * 						They are not required for functionality and only used for debugging as long as no further
 * 						functionality is added by the user.
 * @{
 */
bool ButtonsSoloPushed[8] = {[0 ... 7] = false};
bool ButtonsMutePushed[8] = {[0 ... 7] = false};
bool ButtonsSelectPushed[8] = {[0 ... 7] = false};
/** @} */

/**
 * @name			Button Matrix Variables
 *
 * @brief			These variables are required for the button matrix.
 * @{
 */
/**
 * @brief			Object for the Button Matrix
 */
buttonMatrix_structTd ButtonMatrix;

/**
 * @brief			Objects for each Solo Button
 */
buttonMatrix_Coordinates_Td ButtonSolo[8] = {
		{.PosIntLine = BUTTON_MATRIX_INT_0, .PosDrvLine = BUTTON_MATRIX_DRV_0},
		{.PosIntLine = BUTTON_MATRIX_INT_0, .PosDrvLine = BUTTON_MATRIX_DRV_1},
		{.PosIntLine = BUTTON_MATRIX_INT_0, .PosDrvLine = BUTTON_MATRIX_DRV_2},
		{.PosIntLine = BUTTON_MATRIX_INT_0, .PosDrvLine = BUTTON_MATRIX_DRV_3},
		{.PosIntLine = BUTTON_MATRIX_INT_0, .PosDrvLine = BUTTON_MATRIX_DRV_4},
		{.PosIntLine = BUTTON_MATRIX_INT_0, .PosDrvLine = BUTTON_MATRIX_DRV_5},
		{.PosIntLine = BUTTON_MATRIX_INT_0, .PosDrvLine = BUTTON_MATRIX_DRV_6},
		{.PosIntLine = BUTTON_MATRIX_INT_0, .PosDrvLine = BUTTON_MATRIX_DRV_7}
};

/**
 * @brief			Objects for each Mute Button
 */
buttonMatrix_Coordinates_Td ButtonMute[8] = {
		{.PosIntLine = BUTTON_MATRIX_INT_1, .PosDrvLine = BUTTON_MATRIX_DRV_0},
		{.PosIntLine = BUTTON_MATRIX_INT_1, .PosDrvLine = BUTTON_MATRIX_DRV_1},
		{.PosIntLine = BUTTON_MATRIX_INT_1, .PosDrvLine = BUTTON_MATRIX_DRV_2},
		{.PosIntLine = BUTTON_MATRIX_INT_1, .PosDrvLine = BUTTON_MATRIX_DRV_3},
		{.PosIntLine = BUTTON_MATRIX_INT_1, .PosDrvLine = BUTTON_MATRIX_DRV_4},
		{.PosIntLine = BUTTON_MATRIX_INT_1, .PosDrvLine = BUTTON_MATRIX_DRV_5},
		{.PosIntLine = BUTTON_MATRIX_INT_1, .PosDrvLine = BUTTON_MATRIX_DRV_6},
		{.PosIntLine = BUTTON_MATRIX_INT_1, .PosDrvLine = BUTTON_MATRIX_DRV_7}
};

/**
 * @brief			Objects for each Select Button
 */
buttonMatrix_Coordinates_Td ButtonSelect[8] = {
		{.PosIntLine = BUTTON_MATRIX_INT_2, .PosDrvLine = BUTTON_MATRIX_DRV_0},
		{.PosIntLine = BUTTON_MATRIX_INT_2, .PosDrvLine = BUTTON_MATRIX_DRV_1},
		{.PosIntLine = BUTTON_MATRIX_INT_2, .PosDrvLine = BUTTON_MATRIX_DRV_2},
		{.PosIntLine = BUTTON_MATRIX_INT_2, .PosDrvLine = BUTTON_MATRIX_DRV_3},
		{.PosIntLine = BUTTON_MATRIX_INT_2, .PosDrvLine = BUTTON_MATRIX_DRV_4},
		{.PosIntLine = BUTTON_MATRIX_INT_2, .PosDrvLine = BUTTON_MATRIX_DRV_5},
		{.PosIntLine = BUTTON_MATRIX_INT_2, .PosDrvLine = BUTTON_MATRIX_DRV_6},
		{.PosIntLine = BUTTON_MATRIX_INT_2, .PosDrvLine = BUTTON_MATRIX_DRV_7}
};
/** @} */
/** @} */

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
  /* USER CODE BEGIN 2 */

  /**
   * @addtogroup		MainDemo
   * @{
   */
  /**
   * @name			Button Matrix Initialization
   * @brief			All required init functions from the \ref ButtonMatrix "Button Matrix Module" are called here.
   * @{
   */
  ButtonMatrix_init_Structure(&ButtonMatrix);

  /**
   * @brief 		Setup interrupt pins. Remember to activate EXTI-lines and internal pull ups with falling edge interrupt.
   * @return		unused. It could be used to setup the button coordinates.
   * @{
   */
  ButtonMatrix_init_InterruptLinePin(&ButtonMatrix, GPIOB, GPIO_PIN_13);
  ButtonMatrix_init_InterruptLinePin(&ButtonMatrix, GPIOB, GPIO_PIN_14);
  ButtonMatrix_init_InterruptLinePin(&ButtonMatrix, GPIOB, GPIO_PIN_15);
  /** @} */

  /**
   * @brief 		Setup drive pins
   * @return		unused. It could be used to setup the button coordinates.
   * @{
   */
  ButtonMatrix_init_DriveLinePin(&ButtonMatrix, GPIOA, GPIO_PIN_0);
  ButtonMatrix_init_DriveLinePin(&ButtonMatrix, GPIOA, GPIO_PIN_1);
  ButtonMatrix_init_DriveLinePin(&ButtonMatrix, GPIOA, GPIO_PIN_2);
  ButtonMatrix_init_DriveLinePin(&ButtonMatrix, GPIOA, GPIO_PIN_3);
  ButtonMatrix_init_DriveLinePin(&ButtonMatrix, GPIOA, GPIO_PIN_4);
  ButtonMatrix_init_DriveLinePin(&ButtonMatrix, GPIOA, GPIO_PIN_5);
  ButtonMatrix_init_DriveLinePin(&ButtonMatrix, GPIOA, GPIO_PIN_6);
  ButtonMatrix_init_DriveLinePin(&ButtonMatrix, GPIOA, GPIO_PIN_7);
  /** @} */

  /** @brief		setup timer with 1ms threshold. This should result in 8ms total debounce time for each key.*/
  ButtonMatrix_init_TimerThresholdInMs(&ButtonMatrix, 1);
  /** @} */

  ButtonMatrix_start(&ButtonMatrix);
  /** @} */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		ButtonMatrix_update(&ButtonMatrix);
		for(uint8_t i = 0; i < 5; i++)
		{
			if(ButtonMatrix_check_ButtonPushed(&ButtonMatrix, &ButtonSolo[i]) == true)
			{
			  ButtonsSoloPushed[i] = true;
				/* add user code here */
			}
			else
			{
				ButtonsSoloPushed[i] = false;
				/* add user code here */
			}
			if(ButtonMatrix_check_ButtonPushed(&ButtonMatrix, &ButtonMute[i]) == true)
			{
			  ButtonsMutePushed[i] = true;
			  /* add user code here */
			}
			else
			{
				ButtonsMutePushed[i] = false;
				/* add user code here */
			}
			if(ButtonMatrix_check_ButtonPushed(&ButtonMatrix, &ButtonSelect[i]) == true)
			{
			  ButtonsSelectPushed[i] = true;
			  /* add user code here */
			}
			else
			{
				ButtonsSelectPushed[i] = false;
				/* add user code here */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DRIVE_PIN_0_Pin|DRIVE_PIN_1_Pin|DRIVE_PIN_2_Pin|DRIVE_PIN_3_Pin
                          |DRIVE_PIN_4_Pin|DRIVE_PIN_5_Pin|DRIVE_PIN_6_Pin|DRIVE_PIN_7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_LED_Pin */
  GPIO_InitStruct.Pin = USER_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USER_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DRIVE_PIN_0_Pin DRIVE_PIN_1_Pin DRIVE_PIN_2_Pin DRIVE_PIN_3_Pin
                           DRIVE_PIN_4_Pin DRIVE_PIN_5_Pin DRIVE_PIN_6_Pin DRIVE_PIN_7_Pin */
  GPIO_InitStruct.Pin = DRIVE_PIN_0_Pin|DRIVE_PIN_1_Pin|DRIVE_PIN_2_Pin|DRIVE_PIN_3_Pin
                          |DRIVE_PIN_4_Pin|DRIVE_PIN_5_Pin|DRIVE_PIN_6_Pin|DRIVE_PIN_7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : INT_PIN_0_Pin INT_PIN_1_Pin INT_PIN_2_Pin */
  GPIO_InitStruct.Pin = INT_PIN_0_Pin|INT_PIN_1_Pin|INT_PIN_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : INT_PIN_3_Pin INT_PIN_4_Pin */
  GPIO_InitStruct.Pin = INT_PIN_3_Pin|INT_PIN_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @addtogroup		MainDemo
 * @name				Interrupt Management
 * @brief				Button Matrix interrupt management function ButtonMatrix_manage_Interrupt() is called here.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	ButtonMatrix_manage_Interrupt(&ButtonMatrix, GPIO_Pin);
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
