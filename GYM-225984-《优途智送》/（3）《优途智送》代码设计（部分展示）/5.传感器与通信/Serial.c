#include "ti_msp_dl_config.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "smartcar.h"

#include "motor.h" 
#include "Serial.h"


uint8_t Serial_RxData;    // 接收蓝牙数据缓存
uint8_t Serial_RxFlag;    // 接收完成标志位

// 波特率配置（与ti_msp_dl_config.h保持一致）
#define UART_BAUDRATE 9600
#define UART_CLOCK_FREQ 32000000UL  // 匹配CPUCLK_FREQ

/**
 * 蓝牙串口初始化（UART0，使用ti_msp_dl_config.h定义的引脚）
 */
void Serial_Init(void)
{
    // 初始化UART模块（使用SysConfig生成的初始化函数）
    SYSCFG_DL_UART_0_init();

    // 使能接收中断
    DL_UART_enableInterrupt(UART_0_INST, DL_UART_INTERRUPT_RX);
    DL_Interrupt_registerInterrupt(INT_UART0, UART0_IRQHandler);
    DL_Interrupt_enableInterrupt(INT_UART0);
    DL_Interrupt_enableMaster();

    // 初始化电机和小车
    motor_on();
    SmartCar_Init();
    Serial_SendString("蓝牙控制已就绪\n");
}

/**
 * 发送单个字节
 */
void Serial_SendByte(uint8_t Byte)
{
    while (!(DL_UART_getStatus(UART_0_INST) & DL_UART_STATUS_TX_READY));
    DL_UART_transmitData(UART_0_INST, Byte);
}

/**
 * 发送字节数组
 */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
    uint16_t i;
    for (i = 0; i < Length; i++)
    {
        Serial_SendByte(Array[i]);
    }
}

/**
 * 发送字符串
 */
void Serial_SendString(char *String)
{
    uint16_t i = 0;
    while (String[i] != '\0')
    {
        Serial_SendByte(String[i]);
        i++;
    }
}

/**
 * 计算X的Y次方（辅助函数）
 */
static uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

/**
 * 发送数字（指定位数）
 */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/**
 * 重定向printf到蓝牙串口
 */
int fputc(int ch, FILE *f)
{
    Serial_SendByte((uint8_t)ch);
    return ch;
}

/**
 * 格式化发送
 */
void Serial_Printf(char *format, ...)
{
    char String[100];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    Serial_SendString(String);
}

/**
 * 获取接收标志位
 */
uint8_t Serial_GetRxFlag(void)
{
    if (Serial_RxFlag)
    {
        Serial_RxFlag = 0;
        return 1;
    }
    return 0;
}

/**
 * 获取接收数据
 */
uint8_t Serial_GetRxData(void)
{
    return Serial_RxData;
}

/**
 * 蓝牙指令处理
 */
void Serial_ProcessData(void)
{
    uint8_t cmd = Serial_GetRxData();
    switch(cmd)
    {
        case 'F':
            Move_Forward();
            Serial_SendString("前进\n");
            break;
        case 'B':
            Move_Backward();
            Serial_SendString("后退\n");
            break;
        case 'L':
            Turn_Left();
            Serial_SendString("左转\n");
            break;
        case 'R':
            Turn_Right();
            Serial_SendString("右转\n");
            break;
        case 'A':
            CounterClockwise_Rotation();
            Serial_SendString("逆时针旋转\n");
            break;
        case 'C':
            Clockwise_Rotation();
            Serial_SendString("顺时针旋转\n");
            break;
        case 'S':
            Car_Stop();
            Serial_SendString("停止\n");
            break;
        default:
            Serial_SendString("未知指令，请重新发送\n");
            break;
    }
}

/**
 * UART0中断服务函数
 */
void UART0_IRQHandler(void)
{
    uint32_t status = DL_UART_getPendingInterrupt(UART_0_INST);

    if (status & DL_UART_INTERRUPT_RX)
    {
        Serial_RxData = DL_UART_receiveData(UART_0_INST);  // 读取接收数据
        Serial_RxFlag = 1;                                 // 置位接收标志
        DL_UART_clearInterruptStatus(UART_0_INST, DL_UART_INTERRUPT_RX);
    }
}