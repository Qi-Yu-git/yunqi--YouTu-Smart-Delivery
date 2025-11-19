#include "sw_uart.h"

// 延时函数（需根据系统时钟校准，确保波特率精度）
static void swUartDelay(uint32_t us) {
    uint32_t start = DPL_SYSTICK->CURRENT;
    while ((DPL_SYSTICK->CURRENT - start) < (SystemCoreClock / 1000000 * us));
}

// 软件UART发送一个字节
void swUartSendByte(uint8_t data) {
    uint8_t i;
    // 发送起始位（低电平）
    DL_GPIO_clearPins(GPIO_PA18);
    swUartDelay(1000000 / SW_UART_BAUDRATE);
    // 发送数据位（低位先送）
    for (i = 0; i < SW_UART_DATA_BITS; i++) {
        if (data & (1 << i)) {
            DL_GPIO_setPins(GPIO_PA18);
        } else {
            DL_GPIO_clearPins(GPIO_PA18);
        }
        swUartDelay(1000000 / SW_UART_BAUDRATE);
    }
    // 发送停止位（高电平）
    DL_GPIO_setPins(GPIO_PA18);
    swUartDelay(1000000 / SW_UART_BAUDRATE);
}

// 软件UART接收一个字节（简化版，需添加超时和校验）
uint8_t swUartReceiveByte(void) {
    uint8_t data = 0;
    uint8_t i;
    // 等待起始位（低电平）
    while (DL_GPIO_readPin(GPIO_PA8));
    swUartDelay(1000000 / SW_UART_BAUDRATE / 2); // 起始位中间采样
    // 接收数据位
    for (i = 0; i < SW_UART_DATA_BITS; i++) {
        if (DL_GPIO_readPin(GPIO_PA8)) {
            data |= (1 << i);
        }
        swUartDelay(1000000 / SW_UART_BAUDRATE);
    }
    // 跳过停止位
    swUartDelay(1000000 / SW_UART_BAUDRATE);
    return data;
}