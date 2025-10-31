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
#include "i2c.h"
#include "tim.h"
#include "gpio.h"
 
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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
unsigned char BMP1[] = 
{0x20,0x20,0x20,0xBC,0xA0,0xA0,0xA0,0xBF,0xA4,0xA4,0xA4,0xA4,0x24,0x20,0x20,0x00,/*"?",0*/
0x00,0x00,0x00,0xFF,0x0A,0x0A,0x0A,0x0A,0x0A,0x4A,0x8A,0x7F,0x00,0x00,0x00,0x00,/*"?",0*/
0x10,0x88,0xC4,0x33,0x04,0xF4,0x94,0x94,0xF4,0x9F,0xF4,0x94,0x94,0xF4,0x04,0x00,
0x01,0x00,0xFF,0x00,0x42,0x32,0x02,0x72,0x82,0x86,0x9A,0x82,0xE2,0x0A,0x32,0x00,/*"?",1*/
0x00,0x04,0x04,0x04,0xFF,0x54,0x54,0x54,0x54,0x54,0xFF,0x04,0x04,0x04,0x00,0x00,
0x11,0x11,0x89,0x85,0x93,0x91,0x91,0xFD,0x91,0x91,0x93,0x85,0x89,0x11,0x11,0x00,/*"?",2*/
0x10,0x60,0x00,0xFC,0x04,0x04,0xE4,0xA4,0x25,0x26,0xA4,0x24,0xE4,0x04,0x04,0x00,
0x84,0x42,0x31,0x8F,0x40,0x30,0x0F,0x10,0x09,0x06,0x19,0x00,0x3F,0x40,0xF0,0x00,/*"?",3*/
0x40,0x22,0x14,0x08,0xF4,0x02,0x00,0x04,0x04,0x04,0xFC,0x04,0x04,0x04,0x00,0x00,
0x08,0x44,0x82,0x41,0x3F,0x00,0x40,0x41,0x41,0x41,0x7F,0x41,0x41,0x41,0x40,0x00,/*"?",4*/
0x00,0x00,0x00,0xBE,0x2A,0x2A,0x2A,0xEA,0x2A,0x2A,0x2A,0x3E,0x00,0x00,0x00,0x00,
0x00,0x44,0x42,0x49,0x49,0x49,0x49,0x7F,0x49,0x49,0x49,0x49,0x41,0x40,0x00,0x00,/*"?",5*/
0x00,0x04,0xFF,0x24,0x24,0x24,0xFF,0x04,0x00,0xFE,0x22,0x22,0x22,0xFE,0x00,0x00,
0x88,0x48,0x2F,0x09,0x09,0x19,0xAF,0x48,0x30,0x0F,0x02,0x42,0x82,0x7F,0x00,0x00,/*"?",6*/
0x00,0xFC,0x04,0x04,0x04,0xFC,0x04,0x04,0x04,0xFC,0x04,0x04,0x04,0xFC,0x00,0x00,
0x00,0x7F,0x28,0x24,0x23,0x20,0x20,0x20,0x20,0x21,0x22,0x22,0x22,0x7F,0x00,0x00};/*"?",7*/

// ??????
int scroll_position = 128;  // ???????
int scroll_speed = 1;       // ????
uint8_t scrolling = 1;      // ????

// ?? - ??????????
char student_id[] = "632307030128";

/* USER CODE END PV */
 
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
// ??????????
void Scroll_Display(void);
/* USER CODE END PFP */
 
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// ????????
void Scroll_Display(void)
{
    static uint32_t last_time = 0;
    uint32_t current_time = HAL_GetTick();
    
    // ?50ms??????,??????
    if (current_time - last_time > 50) {
        last_time = current_time;
        
        if (scrolling) {
            // ??
            OLED_CLS();
            
            // ???????"???"
            // ???????????????0,1,2
            // ????,?????????????
            OLED_ShowCN(scroll_position, 1, 0);      // ?
            OLED_ShowCN(scroll_position + 16, 1, 1); // ?  
            OLED_ShowCN(scroll_position + 32, 1, 2); // ?
            
            // ???????
            OLED_ShowStr(30, 5, student_id, 1);
            
            // ??????
            scroll_position -= scroll_speed;
            
            // ????????,????(??????)
            if (scroll_position < -48) {  // ???????48??
                scroll_position = 128;
            }
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
  MX_I2C2_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  OLED_CLS();
  
  // ????2?,??????
  OLED_ShowCN(0, 1, 0);      // ?
  OLED_ShowCN(16, 1, 1);     // ?  
  OLED_ShowCN(32, 1, 2);     // ?
  OLED_ShowStr(30, 5, student_id, 1);
  HAL_Delay(2000);  // ??2??????
  /* USER CODE END 2 */
 
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    
    // ????
    Scroll_Display();
    
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
  * where the assert_param error has occurred.
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