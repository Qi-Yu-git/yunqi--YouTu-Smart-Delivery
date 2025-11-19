#include "ti_msp_dl_config.h"             
#include "motor.h"
#include "smartcar.h"

void SmartCar_Init(void)  // 初始化
{
    // 初始化GPIO和PWM模块
    SYSCFG_DL_GPIO_init();
    SYSCFG_DL_PWM_0_init();
    SYSCFG_DL_PWM_1_init();
    motor_on();  // 使能电机
}

void Move_Forward(void)  // 小车向前
{
    LeftMotor_SetSpeed(50);
    RightMotor_SetSpeed(50);
}

void Move_Backward(void)  // 小车向后
{
    LeftMotor_SetSpeed(-50);
    RightMotor_SetSpeed(-50);
}

void Car_Stop(void)  // 小车停止
{
    LeftMotor_SetSpeed(0);
    RightMotor_SetSpeed(0);
}

void Turn_Left(void)  // 向左转
{
    LeftMotor_SetSpeed(10);  // 左轮减速
    RightMotor_SetSpeed(50); // 右轮正常速度
}

void Turn_Right(void)  // 向右转
{
    LeftMotor_SetSpeed(50);  // 左轮正常速度
    RightMotor_SetSpeed(10); // 右轮减速
}

void Clockwise_Rotation(void)  // 顺时针旋转
{
    LeftMotor_SetSpeed(50);   // 左轮正转
    RightMotor_SetSpeed(-50); // 右轮反转
}

void CounterClockwise_Rotation(void)  // 逆时针旋转
{
    LeftMotor_SetSpeed(-50);  // 左轮反转
    RightMotor_SetSpeed(50);  // 右轮正转
}