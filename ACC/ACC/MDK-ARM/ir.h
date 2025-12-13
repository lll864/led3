#ifndef __IR_H
#define __IR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* 格力空调模式定义 */
typedef enum {
    GREE_MODE_AUTO = 0,    // 自动模式
    GREE_MODE_COOL = 1,    // 制冷模式
    GREE_MODE_DRY = 2,     // 除湿模式
    GREE_MODE_FAN = 3,     // 送风模式
    GREE_MODE_HEAT = 4     // 制热模式
} GreeMode;

/* 格力空调风速定义 */
typedef enum {
    GREE_FAN_AUTO = 0,     // 自动风速
    GREE_FAN_LOW = 1,      // 低风速
    GREE_FAN_MEDIUM = 2,   // 中风速
    GREE_FAN_HIGH = 3      // 高风速
} GreeFanSpeed;

/* 美的空调模式定义 */
typedef enum {
    MIDEA_MODE_AUTO = 0,   // 自动模式
    MIDEA_MODE_COOL = 1,   // 制冷模式
    MIDEA_MODE_DRY = 2,    // 除湿模式
    MIDEA_MODE_FAN = 3,    // 送风模式
    MIDEA_MODE_HEAT = 4    // 制热模式
} MideaMode;

/* 美的空调风速定义 */
typedef enum {
    MIDEA_FAN_AUTO = 0,    // 自动风速
    MIDEA_FAN_LOW = 1,     // 低风速
    MIDEA_FAN_MEDIUM = 2,  // 中风速
    MIDEA_FAN_HIGH = 3     // 高风速
} MideaFanSpeed;

/* 空调控制结构体 */
typedef struct {
    uint8_t power_on;      // 电源开关：0关，1开
    uint8_t mode;          // 模式
    uint8_t temperature;   // 温度(16-30℃)
    uint8_t fan_speed;     // 风速
    uint8_t swing_vert;    // 上下扫风：0关，1开
    uint8_t swing_horiz;   // 左右扫风：0关，1开
    uint8_t sleep;         // 睡眠模式：0关，1开
    uint8_t energy_save;   // 节能模式：0关，1开
} AirConditioner;

/* 函数声明 */
void IR_Init(void);
void IR_SendBit(uint8_t bit);
void IR_SendByte(uint8_t byte);
void IR_SendGreeCommand(AirConditioner *ac);
void IR_SendMideaCommand(AirConditioner *ac);
void IR_Gree_PowerOn_24Cool(void);
void IR_Gree_PowerOff_24Cool(void);
void IR_Midea_PowerOn_24Cool(void);
void IR_Midea_PowerOff_24Cool(void);
uint8_t CalculateGreeChecksum(uint8_t *data, uint8_t length);

#ifdef __cplusplus
}
#endif

#endif /* __IR_H */
