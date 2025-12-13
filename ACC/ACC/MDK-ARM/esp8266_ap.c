#include "esp8266_ap.h"

// 发送命令
void ESP8266_SendCmd(const char *cmd)
{
    char buffer[100];
    sprintf(buffer, "%s\n", cmd);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), 1000);
    HAL_Delay(100);
}

// 等待响应
uint8_t ESP8266_WaitFor(const char *response, uint32_t timeout)
{
    uint8_t rx_data;
    char buffer[200] = {0};
    uint16_t index = 0;
    uint32_t start = HAL_GetTick();
    
    while((HAL_GetTick() - start) < timeout)
    {
        if(HAL_UART_Receive(&huart2, &rx_data, 1, 50) == HAL_OK)
        {
            if(index < sizeof(buffer)-1)
            {
                buffer[index++] = rx_data;
                buffer[index] = '\0';
                
                // 查找响应
                if(strstr(buffer, response) != NULL)
                    return 1;
                    
                // 检查错误
                if(strstr(buffer, "ERROR") != NULL || strstr(buffer, "FAIL") != NULL)
                    return 0;
            }
        }
    }
    return 0; // 超时
}

// 测试AT指令
uint8_t ESP8266_TestAT(void)
{
    ESP8266_SendCmd("AT");
    return ESP8266_WaitFor("OK", 1000);
}

// 设置AP模式
uint8_t ESP8266_SetAPMode(void)
{
    // 设置WiFi模式为AP
    ESP8266_SendCmd("AT+CWMODE=2");
    //if(!ESP8266_WaitFor("OK", 2000)) return 0;
    
    // 配置AP参数
    char ap_cmd[128];
    sprintf(ap_cmd, "AT+CWSAP=\"%s\",\"%s\",%d,3", AP_SSID, AP_PASSWORD, AP_CHANNEL);
    ESP8266_SendCmd(ap_cmd);
    //if(!ESP8266_WaitFor("OK", 5000)) return 0;
    
    // 启用多连接
    ESP8266_SendCmd("AT+CIPMUX=1");
    //if(!ESP8266_WaitFor("OK", 2000)) return 0;
    
    return 1;
}

// 启动TCP服务器
uint8_t ESP8266_StartServer(void)
{
    char cmd[50];
    
    // 启动服务器
    sprintf(cmd, "AT+CIPSERVER=1,%d", TCP_PORT);
    ESP8266_SendCmd(cmd);
    if(!ESP8266_WaitFor("OK", 3000)) return 0;
     
    return 1;
}

// 主初始化函数
void ESP8266_Init(void)
{
	char Recall[50];
	int e=0;
    
	sprintf(Recall,"等待...\r\n");	//拼接输出字符
	HAL_UART_Transmit(&huart2,(uint8_t*)Recall,strlen(Recall),HAL_MAX_DELAY);	//串口发送信息
    // 等待模块启动
    HAL_Delay(5000);
    
    // 清空串口缓冲区
    __HAL_UART_FLUSH_DRREGISTER(&huart2);
    
    // 1. 测试AT指令
	sprintf(Recall,"测试AT指令 \r\n");
	HAL_UART_Transmit(&huart2,(uint8_t*)Recall,strlen(Recall),HAL_MAX_DELAY);	//串口发送信息
	for(int m=3 ; m>0 ; m--){
		if(ESP8266_TestAT()){
			sprintf(Recall,"AT OK \r\n");	//拼接输出字符
			HAL_UART_Transmit(&huart2,(uint8_t*)Recall,strlen(Recall),HAL_MAX_DELAY);	//串口发送信息
			break;
		}
		else{
			sprintf(Recall,"AT Error \r\n");	//拼接输出字符
			HAL_UART_Transmit(&huart2,(uint8_t*)Recall,strlen(Recall),HAL_MAX_DELAY);	//串口发送信息
			e=1;
		}
	}
    
    // 2. 设置AP模式
	sprintf(Recall,"设置AP模式 \r\n");
	HAL_UART_Transmit(&huart2,(uint8_t*)Recall,strlen(Recall),HAL_MAX_DELAY);	//串口发送信息
	if(ESP8266_SetAPMode()){
		sprintf(Recall,"AP OK \r\n");	//拼接输出字符
		HAL_UART_Transmit(&huart2,(uint8_t*)Recall,strlen(Recall),HAL_MAX_DELAY);	//串口发送信息
	}else{
		sprintf(Recall,"AP Error \r\n");	//拼接输出字符
		HAL_UART_Transmit(&huart2,(uint8_t*)Recall,strlen(Recall),HAL_MAX_DELAY);	//串口发送信息
		e=1;
	}
    
    // 3. 启动服务器
	sprintf(Recall,"启动服务器 \r\n");
	HAL_UART_Transmit(&huart2,(uint8_t*)Recall,strlen(Recall),HAL_MAX_DELAY);	//串口发送信息
	if(ESP8266_StartServer()){
		sprintf(Recall,"Server OK \r\n");	//拼接输出字符
		HAL_UART_Transmit(&huart2,(uint8_t*)Recall,strlen(Recall),HAL_MAX_DELAY);	//串口发送信息
	}else{
		sprintf(Recall,"Server Error \r\n");	//拼接输出字符
		HAL_UART_Transmit(&huart2,(uint8_t*)Recall,strlen(Recall),HAL_MAX_DELAY);	//串口发送信息
		e=1;
	}
	
	if(e==0){
		sprintf(Recall,"ESP初始化完成！\r\n");	//拼接输出字符
		HAL_UART_Transmit(&huart2,(uint8_t*)Recall,strlen(Recall),HAL_MAX_DELAY);	//串口发送信息
	}
	else {
		sprintf(Recall,"ESP初始化失败！！！\r\n");
		HAL_UART_Transmit(&huart2,(uint8_t*)Recall,strlen(Recall),HAL_MAX_DELAY);	//串口发送信息
	}
}

//发送任意字符串到手机
void ESP8266_SendDataToPhone(const char* data)
{
    char cmd[50];
    int data_len = strlen(data);
    
	HAL_Delay(100);
    // 1. 发送AT指令：AT+CIPSEND=连接ID,数据长度
    sprintf(cmd, "AT+CIPSEND=0,%d", data_len);
	ESP8266_SendCmd(cmd);
    
	HAL_Delay(100);
    // 2. 发送数据
    ESP8266_SendCmd(data);
	sprintf(cmd,"数据已发送 \r\n");
	HAL_UART_Transmit(&huart2,(uint8_t*)cmd,strlen(cmd),HAL_MAX_DELAY);	//串口发送信息

}
