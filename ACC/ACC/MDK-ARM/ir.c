#include "ir.h"
#include "delay.h"

/* 红外发射引脚定义 */
#define IR_TX_PIN          GPIO_PIN_8
#define IR_TX_PORT         GPIOA

/* 红外信号时序参数 (单位：微秒) */
#define IR_CARRIER_FREQ    38000     // 载波频率38kHz
#define IR_CARRIER_PERIOD  (1000000/IR_CARRIER_FREQ)  // 载波周期

/* 格力红外协议参数 */
#define GREE_LEADER_HIGH   9000      // 引导码高电平9ms
#define GREE_LEADER_LOW    4500      // 引导码低电平4.5ms
#define GREE_BIT0_HIGH     560       // 数据0高电平560us
#define GREE_BIT0_LOW      560       // 数据0低电平560us
#define GREE_BIT1_HIGH     560       // 数据1高电平560us
#define GREE_BIT1_LOW      1690      // 数据1低电平1690us
#define GREE_REPEAT_INTERVAL 108000  // 重复间隔108ms

/* 美的红外协议参数 */
#define MIDEA_LEADER_HIGH  3350      // 引导码高电平3.35ms
#define MIDEA_LEADER_LOW   3350      // 引导码低电平3.35ms
#define MIDEA_BIT0_HIGH    430       // 数据0高电平430us
#define MIDEA_BIT0_LOW     430       // 数据0低电平430us
#define MIDEA_BIT1_HIGH    430       // 数据1高电平430us
#define MIDEA_BIT1_LOW     1300      // 数据1低电平1300us

/* 私有函数声明 */
static void IR_SendCarrier(uint32_t us);
static void IR_SendSpace(uint32_t us);
static void IR_SendGreeData(uint8_t *data, uint8_t length);
static void IR_SendMideaData(uint8_t *data, uint8_t length);
static void IR_EncodeGreeFrame(AirConditioner *ac, uint8_t *frame);
static void IR_EncodeMideaFrame(AirConditioner *ac, uint8_t *frame);

/**
  * @brief  红外发射初始化
  * @retval None
  */
void IR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = IR_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IR_TX_PORT, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(IR_TX_PORT, IR_TX_PIN, GPIO_PIN_RESET);
}

/**
  * @brief  发送载波信号
  * @param  us: 载波持续时间(微秒)
  * @retval None
  */
static void IR_SendCarrier(uint32_t us)
{
    uint32_t i, cycles;
    
    // 计算需要发送的载波周期数
    cycles = us / IR_CARRIER_PERIOD;
    
    for(i = 0; i < cycles; i++)
    {
        HAL_GPIO_WritePin(IR_TX_PORT, IR_TX_PIN, GPIO_PIN_SET);
        delay_us(IR_CARRIER_PERIOD / 2);  // 半周期高电平
        
        HAL_GPIO_WritePin(IR_TX_PORT, IR_TX_PIN, GPIO_PIN_RESET);
        delay_us(IR_CARRIER_PERIOD / 2);  // 半周期低电平
    }
}

/**
  * @brief  发送空白间隔
  * @param  us: 间隔时间(微秒)
  * @retval None
  */
static void IR_SendSpace(uint32_t us)
{
    HAL_GPIO_WritePin(IR_TX_PORT, IR_TX_PIN, GPIO_PIN_RESET);
    delay_us(us);
}

/**
  * @brief  发送一个比特位（格力协议）
  * @param  bit: 要发送的比特位(0或1)
  * @retval None
  */
void IR_SendBit(uint8_t bit)
{
    if(bit)
    {
        IR_SendCarrier(GREE_BIT1_HIGH);
        IR_SendSpace(GREE_BIT1_LOW);
    }
    else
    {
        IR_SendCarrier(GREE_BIT0_HIGH);
        IR_SendSpace(GREE_BIT0_LOW);
    }
}

/**
  * @brief  发送一个字节（MSB first）
  * @param  byte: 要发送的字节
  * @retval None
  */
void IR_SendByte(uint8_t byte)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        IR_SendBit((byte >> (7 - i)) & 0x01);
    }
}

/**
  * @brief  格力校验码计算
  * @param  data: 数据数组
  * @param  length: 数据长度
  * @retval 校验码
  */
uint8_t CalculateGreeChecksum(uint8_t *data, uint8_t length)
{
    uint8_t checksum = 0;
    for(uint8_t i = 0; i < length; i++)
    {
        checksum += data[i];
    }
    return ~checksum;  // 取反作为校验码
}

/**
  * @brief  编码格力空调数据帧
  * @param  ac: 空调配置结构体指针
  * @param  frame: 存储编码后的数据帧(67位，9字节)
  * @retval None
  */
static void IR_EncodeGreeFrame(AirConditioner *ac, uint8_t *frame)
{
    uint8_t temp_data[9] = {0};  // 67位数据需要9字节存储
    uint8_t i;
    uint8_t checksum = 0;
    
    // 1-3位：模式标志 (模式+1转换为3位二进制)
    // 制冷模式(1): 二进制001
    temp_data[0] = (ac->mode + 1) << 5;  // 放在字节0的高3位
    
    // 第4位：开关 (0关1开)
    if(ac->power_on)
        temp_data[0] |= (1 << 4);
    else
        temp_data[0] &= ~(1 << 4);
    
    // 5-6位：风速 (转换为2位二进制)
    temp_data[0] |= (ac->fan_speed & 0x03) << 2;
    
    // 第7位：扫风 (上下扫风)
    if(ac->swing_vert)
        temp_data[0] |= (1 << 1);
    
    // 第8位：睡眠
    if(ac->sleep)
        temp_data[0] |= 0x01;
    
    // 字节1：温度数据(9-12位) + 定时数据开始
    // 温度(16-30℃)，转换为4位二进制：温度-16
    uint8_t temp_binary = ac->temperature - 16;
    temp_data[1] = temp_binary << 4;  // 放在字节1的高4位
    
    // 定时数据(13-20位)：默认为0
    // temp_data[1]的低4位和temp_data[2]的高4位组成定时数据
    
    // 字节2：继续定时数据
    temp_data[2] = 0x00;  // 定时数据默认为0
    
    // 第21位：加湿 (默认0)
    // 第22位：灯光 (默认0)
    // 第23位：负离子 (默认0)
    // 第24位：节电 (0关1开)
    if(ac->energy_save)
        temp_data[2] |= 0x01;
    
    // 字节3：第25位换气
    if(ac->swing_horiz)  // 使用左右扫风作为换气标志
        temp_data[3] = 0x80;
    
    // 26-35位：固定值0001010010
    temp_data[3] |= 0x14;  // 00010100
    temp_data[4] = 0x80;   // 10000000 (26-35位继续)
    
    // 第36位：上下扫风
    if(ac->swing_vert)
        temp_data[4] |= 0x40;
    
    // 37-39位：000
    // 第40位：左右扫风
    if(ac->swing_horiz)
        temp_data[4] |= 0x01;
    
    // 字节5：41-43位：000
    // 44-45位：温度显示 (11表示显示温度)
    temp_data[5] = 0xC0;  // 11000000
    
    // 46-61位：固定值0001000000000000
    temp_data[5] |= 0x10;  // 00010000 (低4位)
    temp_data[6] = 0x00;   // 00000000
    temp_data[7] = 0x00;   // 00000000
    
    // 第62位：节能 (与第24位相同)
    if(ac->energy_save)
        temp_data[7] |= 0x02;
    
    // 第63位：0
    // temp_data[7]已经处理
    
    // 计算校验码(64-67位)
    // 校验码 = [(模式 – 1) + (温度 – 16) + 5 + 左右扫风 + 换气 + 节能]取二进制后四位，再逆序
    checksum = (ac->mode - 1) + (ac->temperature - 16) + 5;
    if(ac->swing_horiz) checksum += 1;  // 左右扫风
    // 换气与左右扫风相同
    if(ac->energy_save) checksum += 1;  // 节能
    
    // 取低4位
    checksum &= 0x0F;
    
    // 逆序
    uint8_t reversed = 0;
    for(i = 0; i < 4; i++)
    {
        reversed <<= 1;
        reversed |= (checksum >> i) & 0x01;
    }
    
    // 放入第64-67位
    temp_data[7] |= (reversed & 0x0F);
    temp_data[8] = 0x00;  // 最后一个字节，只用了部分位
    
    // 复制到输出帧
    for(i = 0; i < 9; i++)
    {
        frame[i] = temp_data[i];
    }
}

/**
  * @brief  发送格力空调数据
  * @param  data: 数据数组
  * @param  length: 数据长度
  * @retval None
  */
static void IR_SendGreeData(uint8_t *data, uint8_t length)
{
    uint8_t i, j;
    
    // 发送引导码
    IR_SendCarrier(GREE_LEADER_HIGH);
    IR_SendSpace(GREE_LEADER_LOW);
    
    // 发送数据
    for(i = 0; i < length; i++)
    {
        for(j = 0; j < 8; j++)
        {
            uint8_t bit = (data[i] >> (7 - j)) & 0x01;
            IR_SendBit(bit);
        }
    }
    
    // 发送结束码
    IR_SendCarrier(560);
    
    // 重复间隔
    delay_ms(108);
}

/**
  * @brief  发送格力空调命令
  * @param  ac: 空调配置结构体指针
  * @retval None
  */
void IR_SendGreeCommand(AirConditioner *ac)
{
    uint8_t frame[9];
    
    // 编码数据帧
    IR_EncodeGreeFrame(ac, frame);
    
    // 发送数据帧（重复3次确保接收）
    for(uint8_t i = 0; i < 3; i++)
    {
        IR_SendGreeData(frame, 9);
        if(i < 2) delay_ms(10);
    }
}

/**
  * @brief  编码美的空调数据帧
  * @param  ac: 空调配置结构体指针
  * @param  frame: 存储编码后的数据帧
  * @retval None
  */
static void IR_EncodeMideaFrame(AirConditioner *ac, uint8_t *frame)
{
    // 美的空调协议简化实现
    // 实际协议更复杂，这里提供基本框架
    
    frame[0] = 0x23;  // 美的空调协议头
    frame[1] = 0xCB;
    frame[2] = 0x26;
    frame[3] = 0x01;
    
    // 模式设置
    frame[4] = 0x00;
    if(ac->mode == MIDEA_MODE_COOL)
        frame[4] |= 0x01;  // 制冷模式
    
    // 温度设置 (24℃)
    frame[5] = 0x20 | (ac->temperature & 0x0F);
    
    // 风速设置
    frame[6] = ac->fan_speed << 5;
    
    // 开关状态
    if(ac->power_on)
        frame[7] = 0x08;
    else
        frame[7] = 0x00;
    
    // 校验和
    uint8_t checksum = 0;
    for(uint8_t i = 0; i < 8; i++)
    {
        checksum += frame[i];
    }
    frame[8] = checksum;
}

/**
  * @brief  发送美的空调数据
  * @param  data: 数据数组
  * @param  length: 数据长度
  * @retval None
  */
static void IR_SendMideaData(uint8_t *data, uint8_t length)
{
    uint8_t i, j;
    
    // 发送引导码
    IR_SendCarrier(MIDEA_LEADER_HIGH);
    IR_SendSpace(MIDEA_LEADER_LOW);
    
    // 发送数据
    for(i = 0; i < length; i++)
    {
        for(j = 0; j < 8; j++)
        {
            uint8_t bit = (data[i] >> (7 - j)) & 0x01;
            if(bit)
            {
                IR_SendCarrier(MIDEA_BIT1_HIGH);
                IR_SendSpace(MIDEA_BIT1_LOW);
            }
            else
            {
                IR_SendCarrier(MIDEA_BIT0_HIGH);
                IR_SendSpace(MIDEA_BIT0_LOW);
            }
        }
    }
    
    // 发送结束码
    IR_SendCarrier(430);
}

/**
  * @brief  发送美的空调命令
  * @param  ac: 空调配置结构体指针
  * @retval None
  */
void IR_SendMideaCommand(AirConditioner *ac)
{
    uint8_t frame[9];
    
    // 编码数据帧
    IR_EncodeMideaFrame(ac, frame);
    
    // 发送数据帧（重复3次）
    for(uint8_t i = 0; i < 3; i++)
    {
        IR_SendMideaData(frame, 9);
        if(i < 2) delay_ms(40);
    }
}

/**
  * @brief  格力空调开机并设置为24度制冷
  * @retval None
  */
void IR_Gree_PowerOn_24Cool(void)
{
    AirConditioner gree_ac;
    
    // 设置格力空调参数
    gree_ac.power_on = 1;        // 开机
    gree_ac.mode = GREE_MODE_COOL; // 制冷模式
    gree_ac.temperature = 24;    // 24度
    gree_ac.fan_speed = GREE_FAN_AUTO; // 自动风速
    gree_ac.swing_vert = 0;      // 关闭上下扫风
    gree_ac.swing_horiz = 0;     // 关闭左右扫风
    gree_ac.sleep = 0;           // 关闭睡眠模式
    gree_ac.energy_save = 0;     // 关闭节能模式
    
    // 发送命令
    IR_SendGreeCommand(&gree_ac);
}

void IR_Gree_PowerOff_24Cool(void)
{
    AirConditioner gree_ac;
    
    // 设置格力空调参数
    gree_ac.power_on = 0;        // 关机
    gree_ac.mode = GREE_MODE_COOL; // 制冷模式
    gree_ac.temperature = 24;    // 24度
    gree_ac.fan_speed = GREE_FAN_AUTO; // 自动风速
    gree_ac.swing_vert = 0;      // 关闭上下扫风
    gree_ac.swing_horiz = 0;     // 关闭左右扫风
    gree_ac.sleep = 0;           // 关闭睡眠模式
    gree_ac.energy_save = 0;     // 关闭节能模式
    
    // 发送命令
    IR_SendGreeCommand(&gree_ac);
}

/**
  * @brief  美的空调开机并设置为24度制冷
  * @retval None
  */
void IR_Midea_PowerOn_24Cool(void)
{
    AirConditioner midea_ac;
    
    // 设置美的空调参数
    midea_ac.power_on = 1;          // 开机
    midea_ac.mode = MIDEA_MODE_COOL; // 制冷模式
    midea_ac.temperature = 24;      // 24度
    midea_ac.fan_speed = MIDEA_FAN_AUTO; // 自动风速
    midea_ac.swing_vert = 0;        // 关闭上下扫风
    midea_ac.swing_horiz = 0;       // 关闭左右扫风
    midea_ac.sleep = 0;             // 关闭睡眠模式
    midea_ac.energy_save = 0;       // 关闭节能模式
    
    // 发送命令
    IR_SendMideaCommand(&midea_ac);
}

void IR_Midea_PowerOff_24Cool(void)
{
    AirConditioner midea_ac;
    
    // 设置美的空调参数
    midea_ac.power_on = 0;          // 开机
    midea_ac.mode = MIDEA_MODE_COOL; // 制冷模式
    midea_ac.temperature = 24;      // 24度
    midea_ac.fan_speed = MIDEA_FAN_AUTO; // 自动风速
    midea_ac.swing_vert = 0;        // 关闭上下扫风
    midea_ac.swing_horiz = 0;       // 关闭左右扫风
    midea_ac.sleep = 0;             // 关闭睡眠模式
    midea_ac.energy_save = 0;       // 关闭节能模式
    
    // 发送命令
    IR_SendMideaCommand(&midea_ac);
}
