#include "motor.h" 
#include "ti_msp_dl_config.h"

// 开启电机（STBY置高）
void motor_on(void)
{
    DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_STBY_PIN);
}

// 关闭电机（STBY置低，清除所有电机引脚）
void motor_off(void)
{
    DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_STBY_PIN);
    DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_L_0_PIN);
    DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_L_1_PIN);
    DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_R_0_PIN);
    DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_R_1_PIN);
}

uint32_t comparevalue = 0;

// 统一PWM设置函数（内部使用）
void set_pwm(int left_oil, int right_oil)
{
    if (left_oil >= 0)
    {
        // 左电机正转
        comparevalue = 3199 - 3199 * (left_oil / 100.0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, comparevalue, DL_TIMER_CC_0_INDEX);
        DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_L_1_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_L_0_PIN);
    }
    else
    {
        // 左电机反转
        comparevalue = 3199 - 3199 * (-left_oil / 100.0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, comparevalue, DL_TIMER_CC_0_INDEX);
        DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_L_0_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_L_1_PIN);
    }
    DL_GPIO_clearPins(GPIO_LED_PORT, GPIO_LED_PIN_LED_1_PIN);

    if (right_oil >= 0)
    {
        // 右电机正转
        comparevalue = 3199 - 3199 * (right_oil / 100.0);
        DL_TimerG_setCaptureCompareValue(PWM_1_INST, comparevalue, DL_TIMER_CC_0_INDEX);
        DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_R_1_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_R_0_PIN);
    }
    else
    {
        // 右电机反转
        comparevalue = 3199 - 3199 * (-right_oil / 100.0);
        DL_TimerG_setCaptureCompareValue(PWM_1_INST, comparevalue, DL_TIMER_CC_0_INDEX);
        DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_R_0_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_R_1_PIN);
    }
}

// 左电机速度设置（供smartcar.c调用）
void LeftMotor_SetSpeed(int8_t Speed)
{
    if (Speed >= 0)
    {
        comparevalue = 3199 - 3199 * (Speed / 100.0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, comparevalue, DL_TIMER_CC_0_INDEX);
        DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_L_1_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_L_0_PIN);
    }
    else if (Speed == 0)
    {
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_L_0_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_L_1_PIN);
    }
    else
    {
        comparevalue = 3199 - 3199 * (-Speed / 100.0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, comparevalue, DL_TIMER_CC_0_INDEX);
        DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_L_0_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_L_1_PIN);
    }
}

// 右电机速度设置（供smartcar.c调用）
void RightMotor_SetSpeed(int8_t Speed)
{
    if (Speed >= 0)
    {
        comparevalue = 3199 - 3199 * (Speed / 100.0);
        DL_TimerG_setCaptureCompareValue(PWM_1_INST, comparevalue, DL_TIMER_CC_0_INDEX);
        DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_R_1_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_R_0_PIN);
    }
    else if (Speed == 0)
    {
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_R_0_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_R_1_PIN);
    }
    else
    {
        comparevalue = 3199 - 3199 * (-Speed / 100.0);
        DL_TimerG_setCaptureCompareValue(PWM_1_INST, comparevalue, DL_TIMER_CC_0_INDEX);
        DL_GPIO_setPins(GPIO_motor_PORT, GPIO_motor_PIN_R_0_PIN);
        DL_GPIO_clearPins(GPIO_motor_PORT, GPIO_motor_PIN_R_1_PIN);
    }
}