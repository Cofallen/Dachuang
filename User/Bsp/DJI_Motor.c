#include "DJI_Motor.h"


void DJI_Current_Ctrl(hcan_t* hcan, uint16_t stdid, int16_t num1, int16_t num2, int16_t num3, int16_t num4)
{
    uint8_t Data[8];

    Data[0] = ((num1) >> 8);
    Data[1] = (num1);
    Data[2] = ((num2) >> 8);
    Data[3] = (num2);
    Data[4] = ((num3) >> 8);
    Data[5] = (num3);
    Data[6] = ((num4) >> 8);
    Data[7] = (num4);

    canx_send_data(hcan, stdid, Data);
}


void MotorResolve(DJI_MOTOR_Typedef *motor, uint8_t *RxMessage)
{
    motor->DATA.Angle_last = motor->DATA.Angle_now;
    motor->DATA.Angle_now  = (uint16_t)RxMessage[0] << 8 | (uint16_t)RxMessage[1];
    motor->DATA.Speed_last = motor->DATA.Speed_now;
    motor->DATA.Speed_now  = (uint16_t)RxMessage[2] << 8 | (uint16_t)RxMessage[3];
    motor->DATA.current = ((uint16_t)RxMessage[4] << 8 | (uint16_t)RxMessage[5]);
    motor->DATA.temperature = RxMessage[6];
}

/**
 * @brief 根据电机编码器值计算运转圈数
 *
 * @param motor 电机结构体指针
 */
void MotorRoundResolve(DJI_MOTOR_Typedef *motor)
{
    if (motor->DATA.Angle_now - motor->DATA.Angle_last < -4096)
    {
        motor->DATA.Laps++;
    }
    if (motor->DATA.Angle_now - motor->DATA.Angle_last > 4096)
    {
        motor->DATA.Laps--;
    }
    motor->DATA.Angle_Infinite = motor->DATA.Laps * 8192 + motor->DATA.Angle_now;
}