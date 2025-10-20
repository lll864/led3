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
#include "usart.h"
#include "gpio.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

// 全局变量定义放在这里
uint8_t sendFlag = 1;        // 发送标志位
uint8_t rxData[32];          // DMA接收数据缓冲区
uint8_t uartRxFlag = 0;      // 接收完成标志
uint16_t rxDataIndex = 0;    // 接收数据索引

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// UART DMA接收完成回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
    uartRxFlag = 1;  // 设置接收完成标志
  }
}

// UART错误回调函数
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
    // 重新启动DMA接收
    HAL_UART_Receive_DMA(&huart1, rxData, sizeof(rxData));
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  // 启动UART DMA接收 - 放在外设初始化之后
  HAL_UART_Receive_DMA(&huart1, rxData, sizeof(rxData));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
    // 检查是否收到数据
    if(uartRxFlag)
    {
      uartRxFlag = 0;  // 清除标志
      
      // 处理接收到的数据
      for(rxDataIndex = 0; rxDataIndex < sizeof(rxData); rxDataIndex++)
      {
        if(rxData[rxDataIndex] == '#') 
        {
          sendFlag = 0;  // 暂停发送
        }
        else if(rxData[rxDataIndex] == '*')
        {
          sendFlag = 1;  // 继续发送
        }
      }
      
      // 重新启动DMA接收
      HAL_UART_Receive_DMA(&huart1, rxData, sizeof(rxData));
    }
    
    // 根据标志位决定是否发送数据
    if(sendFlag)
    {
      char data[] = "hello windows!\r\n";
      HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 0xFFFF);
      
      // LED闪烁指示
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
      HAL_Delay(100);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
      
      HAL_Delay(1000);  // 每秒发送一次
    }
    else
    {
      // 暂停状态时LED慢闪
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
      HAL_Delay(500);
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  // 系统时钟配置代码（由CubeMX生成）
  /* USER CODE BEGIN SystemClock_Config */
  
  /* USER CODE END SystemClock_Config */
}

/* USER CODE BEGIN 4 */

// 其他用户代码可以放在这里

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