/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "aht20.h"
#include "hpt.h"
#include "oled.h"

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t cmdOn[] = { 0x9000000A, 0x0004000A };
uint32_t cmdOff[] = { 0x8000000A, 0x0004000B };

uint32_t cmd[2];
uint32_t cmdSegIdx = 0;
uint32_t segBitIdx = 0;

uint64_t lastUs = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  uint64_t currentUs = HPT_GetUs();
  uint64_t deltaUs = HPT_DeltaUs(lastUs, currentUs);
  lastUs = currentUs;
  if(GPIO_Pin == GPIO_PIN_13)
  {
    if (deltaUs > 8000)
    {
      // beginning of the transmit
      segBitIdx = 0;
      cmdSegIdx++;
      if (cmdSegIdx == 2) {
        cmdSegIdx = 0;
        cmd[0] = 0;
        cmd[1] = 0;
      }
    }
    else if (deltaUs > 1700)
    {
      // bit-1
      if (segBitIdx < 32) {
        segBitIdx++;
        cmd[cmdSegIdx] |= (1UL << (32 - segBitIdx));   // write 1
      }
      
    }
    else if (deltaUs > 1000)
    {
      // bit-0
      if (segBitIdx < 32) {
        segBitIdx++;
        cmd[cmdSegIdx] &= ~(1UL << (32 - segBitIdx));   // write 0
      }
    }
    if (cmdSegIdx == 1 && segBitIdx == 32) {
      cmdSegIdx = 0;
      segBitIdx = 0;
    }
  }
}

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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  HAL_Delay(100);

  AHT20_Init();

  OLED_Init();
  OLED_PlotString(0, 0, "Test", OLED_FONT_1608, OLED_PLOTTING_FILL, OLED_BACKGROUND_FILL);
  OLED_Flush();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    float humidity = 0.0, temperature = 0.0;
    char send_buf[50] = {0};

    AHT20_Read(&temperature, &humidity);

    sprintf(send_buf, "T: %.1f H: %.1f", temperature, humidity);
    OLED_PlotString(0, 16, send_buf, OLED_FONT_1608, OLED_PLOTTING_FILL, OLED_BACKGROUND_FILL);
    sprintf(send_buf, "C: %0.8X %0.8X", cmd[0], cmd[1]);
    OLED_PlotString(0, 32, send_buf, OLED_FONT_0806, OLED_PLOTTING_FILL, OLED_BACKGROUND_FILL);
    OLED_Flush();
    HAL_Delay(1000);
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

/* USER CODE BEGIN 4 */

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
