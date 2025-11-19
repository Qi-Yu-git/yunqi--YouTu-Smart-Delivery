#include "ti_msp_dl_config.h"
#include "motor.h"
#include "smartcar.h"
#include "Serial.h"

// 超声波引脚定义（需根据硬件实际连接修改）
#define TRIG_PORT    GPIO_A
#define TRIG_PIN     GPIO_PIN_0
#define ECHO_PORT    GPIO_A
#define ECHO_PIN     GPIO_PIN_1

// 避障参数
#define OBSTACLE_THRESHOLD  20  // 障碍物阈值（厘米）
#define SAFE_DISTANCE       30  // 安全距离（厘米）

// 全局变量
static uint32_t g_echo_start = 0;
static uint32_t g_echo_end = 0;
static uint8_t g_measure_done = 0;

/**
 * 初始化超声波传感器
 */
void Ultrasonic_Init(void) {
    // 配置TRIG为输出，ECHO为输入
    DL_GPIO_setPinMode(TRIG_PORT, TRIG_PIN, DL_GPIO_MODE_OUTPUT);
    DL_GPIO_setPinMode(ECHO_PORT, ECHO_PIN, DL_GPIO_MODE_INPUT);
    
    // 初始化TRIG为低电平
    DL_GPIO_clearPins(TRIG_PORT, TRIG_PIN);
    
    // 配置ECHO引脚中断（上升沿和下降沿触发）
    DL_GPIO_enableInterrupt(ECHO_PORT, ECHO_PIN);
    DL_GPIO_setInterruptConfig(ECHO_PORT, ECHO_PIN, 
        DL_GPIO_INTERRUPT_TRIGGER_BOTH_EDGES);
    DL_Interrupt_registerInterrupt(INT_GPIOA, Ultrasonic_IRQHandler);
    DL_Interrupt_enableInterrupt(INT_GPIOA);
}

/**
 * 发送超声波触发信号
 */
static void Ultrasonic_Trigger(void) {
    DL_GPIO_clearPins(TRIG_PORT, TRIG_PIN);
    swUartDelay(2);  // 2us低电平
    DL_GPIO_setPins(TRIG_PORT, TRIG_PIN);
    swUartDelay(10); // 10us高电平触发
    DL_GPIO_clearPins(TRIG_PORT, TRIG_PIN);
    g_measure_done = 0;
}

/**
 * 计算距离（厘米）
 * 声速：343.2m/s = 0.03432cm/us，往返距离需除以2
 */
static float Ultrasonic_GetDistance(void) {
    uint32_t duration = g_echo_end - g_echo_start;
    return (duration * 0.03432) / 2;
}

/**
 * 超声波中断服务函数（处理ECHO信号）
 */
void Ultrasonic_IRQHandler(void) {
    if (DL_GPIO_readPin(ECHO_PORT, ECHO_PIN)) {
        // 上升沿：记录开始时间（使用系统滴答定时器）
        g_echo_start = DPL_SYSTICK->CURRENT;
    } else {
        // 下降沿：记录结束时间
        g_echo_end = DPL_SYSTICK->CURRENT;
        g_measure_done = 1;
    }
    DL_GPIO_clearInterruptStatus(ECHO_PORT, ECHO_PIN);
}

/**
 * 避障控制逻辑
 */
void Ultrasonic_AvoidObstacle(void) {
    float distance;
    
    // 发送触发信号并等待测量完成
    Ultrasonic_Trigger();
    while (!g_measure_done);
    
    distance = Ultrasonic_GetDistance();
    Serial_Printf("距离: %.1f cm\n", distance);  // 调试信息
    
    // 根据距离执行避障动作
    if (distance < OBSTACLE_THRESHOLD) {
        // 距离过近：停止并右转
        Car_Stop();
        swUartDelay(500000);  // 延时500ms
        Turn_Right();
        swUartDelay(800000);  // 右转约0.8秒
    } else if (distance < SAFE_DISTANCE) {
        // 接近障碍物：减速前进
        LeftMotor_SetSpeed(30);
        RightMotor_SetSpeed(30);
    } else {
        // 安全距离：正常前进
        Move_Forward();
    }
}