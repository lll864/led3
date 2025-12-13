#include <aht20.h>

#define AHT20_ADDRESS 0x70

void AHT20_Init(){		//AHT20初始化函数
	uint8_t read;
	HAL_Delay(40);							//上电后等待40ms
	HAL_I2C_Master_Receive(&hi2c1,AHT20_ADDRESS,&read,1,HAL_MAX_DELAY);	//获取状态位校准
	
	if((read & 0x08)==0x00){	//判断返回值第3位是否为1
		uint8_t sendBE[3]={0xBE,0x08,0x00};	//初始化指令
		HAL_I2C_Master_Transmit(&hi2c1,AHT20_ADDRESS,sendBE,3,HAL_MAX_DELAY);	//发送初始化指令
		HAL_Delay(10);
	}
}

void AHT20_Read(float *T,float *RH){		//温湿度测量函数
	uint8_t sendAC[3]={0xAC,0x33,0x00};	//AC命令触发测量
	uint8_t readData[6];
	
	HAL_I2C_Master_Transmit(&hi2c1,AHT20_ADDRESS,sendAC,3,HAL_MAX_DELAY);	//发送测量指令
	HAL_Delay(80);
	HAL_I2C_Master_Receive(&hi2c1,AHT20_ADDRESS,readData,6,HAL_MAX_DELAY);	//返回数据接收
	
	if((readData[0] & 0x80)==0x00){	//判断第7位是否为1
		uint32_t data=0;
		data =((uint32_t)readData[3]>>4)+((uint32_t)readData[2]<<4)+((uint32_t)readData[1]<<12);//湿度数据拼接
		*RH=data*100.0f/(1<<20);	//湿度计算
		
		data =(((uint32_t)readData[3] & 0x0F)<<16)+((uint32_t)readData[4]<<8)+((uint32_t)readData[5]);//温度数据拼接
		*T=data*200.0f/(1<<20)-50;	//温度计算
	}                                                                                                                                                  
}
