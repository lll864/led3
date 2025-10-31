/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_u8g2.h"
#include "test.h"
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

/* ??? 16x16???? - ??? */
static const unsigned char li[] U8X8_PROGMEM = {
  0x80,0x00,0x80,0x00,0xFE,0x3F,0xC0,0x01,
  0xA0,0x02,0x90,0x04,0x8C,0x18,0x83,0x60,
  0xF0,0x07,0x00,0x02,0x00,0x01,0xFF,0x7F,
  0x80,0x00,0x80,0x00,0xA0,0x00,0x40,0x00
}; /*"?",0*/

static const unsigned char jia[] U8X8_PROGMEM = {
  0x80,0x00,0xFF,0x7F,0x80,0x00,0xFC,0x1F,
  0x00,0x00,0xF8,0x0F,0x08,0x08,0xF8,0x0F,
  0x10,0x04,0xFF,0x7F,0x08,0x00,0xFE,0x3E,
  0x88,0x22,0x84,0x22,0xA2,0x3E,0x41,0x22
}; /*"?",1*/

static const unsigned char yi[] U8X8_PROGMEM = {
  0x08,0x04,0x08,0x04,0x08,0x02,0x18,0x02,
  0x2A,0x11,0x8A,0x20,0xCA,0x7F,0x09,0x40,
  0x08,0x00,0x88,0x3F,0x88,0x20,0x88,0x20,
  0x88,0x20,0x88,0x20,0x88,0x3F,0x88,0x20
}; /*"?",2*/

// ?????
const char student_id[] = "632307031028";

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
  MX_I2C2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  u8g2_t u8g2;
  u8g2Init(&u8g2);
  
  // ?????????
  u8g2_ClearBuffer(&u8g2);
  u8g2_SendBuffer(&u8g2);
  HAL_Delay(100);
  
  // ????
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
  u8g2_DrawStr(&u8g2, 0, 10, "Testing...");
  u8g2_SendBuffer(&u8g2);
  HAL_Delay(1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  int x = 128;  // ???????
  
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
    // ??
    u8g2_ClearBuffer(&u8g2);
    
    // ????
    if(x > -48)  // 3??????48??
    {
      x--;  // ????
    }
    else
    {
      x = 128;  // ?????
    }
    
    // ?????"???" - ???????
    u8g2_DrawXBMP(&u8g2, x, 16, 16, 16, li);    // ?
    u8g2_DrawXBMP(&u8g2, x+16, 16, 16, 16, jia); // ?
    u8g2_DrawXBMP(&u8g2, x+32, 16, 16, 16, yi);  // ?
    
    // ????
    u8g2_SetFont(&u8g2, u8g2_font_8x13_tf);
    
    // ????????
    uint8_t str_width = strlen(student_id) * 8;
    uint8_t x_pos = (128 - str_width) / 2;
    u8g2_DrawStr(&u8g2, x_pos, 45, student_id);
    
    // ???????
    u8g2_SendBuffer(&u8g2);
    
    // ??????
    HAL_Delay(40);
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
  * in the RCC_OscInitStruct structure.
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/