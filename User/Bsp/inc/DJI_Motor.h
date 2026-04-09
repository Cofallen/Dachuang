/*
 * @Descripttion: 
 * @version: 
 * @Author: Eugene
 * @Date: 2024-07-04 16:02:33
 * @LastEditors: Andy
 * @LastEditTime: 2024-07-05 15:45:32
 */
#ifndef __DJI_MOTOR_H
#define __DJI_MOTOR_H

#include "MY_define.h"
#include "can_bsp.h"
#include "controller.h"

typedef struct
{
    int8_t ONLINE_JUDGE_TIME;
    int16_t Angle_last; // 上一个角度值
    int16_t Angle_now;  // 现在的角度值
    int16_t Speed_last; // 上一个速度值
    int16_t Speed_now;  // 现在的速度值
    int16_t current;
    int8_t temperature;
    int32_t Angle_Infinite;
    int64_t Stuck_Time;
    uint16_t Stuck_Flag[2];
    int16_t Laps;
    float Error;
    float Aim;
    float Aim_last;
    float dt;
}DJI_MOTOR_DATA_Typedef;

typedef struct
{
    uint8_t PID_INIT;
    DJI_MOTOR_DATA_Typedef DATA;
    PID_t PID_P;
    PID_t PID_S;
}DJI_MOTOR_Typedef;

typedef struct 
{
    DJI_MOTOR_Typedef M3508[4];
    DJI_MOTOR_Typedef M6020[2];
}MOTOR_Typedef;

typedef struct
{
    struct 
    {
        float Pitch;
        float Pitch_MAX;
        float Pitch_MIN;
        float Yaw;
        float Yaw_Init;
    }HEAD;
    struct 
    {
        float Chassis_dt;
        float Gimbal_dt;
        float Monitor_dt;
        float Shoot_dt;
        float Gimbal_time;
        uint32_t Chassis_Count;
        uint32_t Gimbal_Count;
        uint32_t Monitor_Count;
        uint32_t Shoot_Count;
    }DWT_TIME;
}CONTAL_Typedef;

void DJI_Current_Ctrl(hcan_t* hcan, uint16_t stdid, int16_t num1, int16_t num2, int16_t num3, int16_t num4);
void MotorResolve(DJI_MOTOR_Typedef *motor, uint8_t *RxMessage);
void MotorRoundResolve(DJI_MOTOR_Typedef *motor);

#endif
