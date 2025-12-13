#include "delay.h"

static uint8_t  fac_us = 0;
static uint16_t fac_ms = 0;

/**
  * @brief  延时初始化函数
  * @param  SYSCLK: 系统时钟频率，单位MHz
  * @retval None
  */
void delay_init(uint8_t SYSCLK)
{
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    fac_us = SYSCLK;
    fac_ms = (uint16_t)fac_us * 1000;
}

/**
  * @brief  微秒延时函数
  * @param  nus: 延时的微秒数
  * @retval None
  */
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;
    
    ticks = nus * fac_us;
    told = SysTick->VAL;
    
    while(1)
    {
        tnow = SysTick->VAL;
        if(tnow != told)
        {
            if(tnow < told)
                tcnt += told - tnow;
            else
                tcnt += reload - tnow + told;
            told = tnow;
            if(tcnt >= ticks) break;
        }
    }
}

/**
  * @brief  毫秒延时函数
  * @param  nms: 延时的毫秒数
  * @retval None
  */
void delay_ms(uint16_t nms)
{
    uint32_t i;
    for(i = 0; i < nms; i++) delay_us(1000);
}
