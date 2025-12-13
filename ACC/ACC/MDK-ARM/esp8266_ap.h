#ifndef __ESP8266_AP_H
#define __ESP8266_AP_H

#include "main.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"

// AP配置
#define AP_SSID        "STM32_AP"      // 热点名称
#define AP_PASSWORD    "12345678"      // 密码
#define AP_CHANNEL     6               // WiFi信道
#define TCP_PORT       8080            // TCP端口

// 串口句柄（在main.c中定义）
extern UART_HandleTypeDef huart2;

// 函数声明
void ESP8266_Init(void);
void ESP8266_SendCmd(const char *cmd);
void ESP8266_SendDataToPhone(const char* data);
uint8_t ESP8266_WaitFor(const char *response, uint32_t timeout);
uint8_t ESP8266_TestAT(void);
uint8_t ESP8266_SetAPMode(void);
uint8_t ESP8266_StartServer(void);

#endif
