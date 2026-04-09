#include "Gimbal_Task.h"
#include "controller.h"
#include "All_init.h"
#include "MY_Define.h"

uint8_t MOTOR_PID_Gimbal_INIT(MOTOR_Typedef *motor)
{
    float PID_S_Pitch[3] = {75.0f, 0.0f, 0.0f};
    float PID_P_Pitch[3] = {0.85f, 0.001f, 0.0f};

    PID_Init(&motor->M6020[PITCH].PID_S, 20000.0f, 0.0f, 
              PID_S_Pitch, 1000.0f, 1000.0f, 
              0.7f, 0.7f, 2, 
              Integral_Limit|OutputFilter|ErrorHandle|
              Trapezoid_Intergral|ChangingIntegrationRate|
              Derivative_On_Measurement|DerivativeFilter);
    PID_Init(&motor->M6020[PITCH].PID_P, 10000.0f, 50.0f, 
              PID_P_Pitch, 2000.0f, 1000.0f, 
              0.7f, 0.7f, 2, 
              Integral_Limit|OutputFilter|ErrorHandle|
              Trapezoid_Intergral|ChangingIntegrationRate|
              Derivative_On_Measurement|DerivativeFilter);
    
    float PID_S_Yaw[3] = {50.0f, 0.0f, 0.0f};
    float PID_P_Yaw[3] = {2.0f, 0.0008f, 0.001f};

    PID_Init(&motor->M6020[YAW].PID_S, 15000.0f, 0.0f, 
              PID_S_Yaw, 1000.0f, 1000.0f, 
              0.7f, 0.7f, 2, 
              Integral_Limit|OutputFilter|ErrorHandle|
              Trapezoid_Intergral|ChangingIntegrationRate|
              Derivative_On_Measurement|DerivativeFilter);
    PID_Init(&motor->M6020[YAW].PID_P, 10000.0f, 50.0f, 
              PID_P_Yaw, 1000.0f, 1000.0f, 
              0.7f, 0.7f, 2, 
              Integral_Limit|OutputFilter|ErrorHandle|
              Trapezoid_Intergral|ChangingIntegrationRate|
              Derivative_On_Measurement|DerivativeFilter);
    return 0;
}

uint8_t Gimbal_AIM_INIT()
{

    return 0;
}

uint8_t gimbal_task(CONTAL_Typedef *CONTAL, MOTOR_Typedef *MOTOR)
{
    MOTOR->M6020[PITCH].DATA.Aim = CONTAL->HEAD.Pitch;
    MOTOR->M6020[YAW].DATA.Aim   = CONTAL->HEAD.Yaw;

    PID_Calculate(&MOTOR->M6020[PITCH].PID_P, 
                   MOTOR->M6020[PITCH].DATA.Angle_Infinite,
                   MOTOR->M6020[PITCH].DATA.Aim);
    PID_Calculate(&MOTOR->M6020[PITCH].PID_S, 
                   MOTOR->M6020[PITCH].DATA.Speed_now,
                   MOTOR->M6020[PITCH].PID_P.Output);

    PID_Calculate(&MOTOR->M6020[YAW].PID_P,
                   MOTOR->M6020[YAW].DATA.Angle_Infinite,
                   MOTOR->M6020[YAW].DATA.Aim);
    PID_Calculate(&MOTOR->M6020[YAW].PID_S,
                   MOTOR->M6020[YAW].DATA.Speed_now,
                   MOTOR->M6020[YAW].PID_P.Output);
    
    DJI_Current_Ctrl(&hcan, 0x1ff, 
                     (int16_t)MOTOR->M6020[YAW].PID_S.Output,
                     (int16_t)MOTOR->M6020[PITCH].PID_S.Output,
                     0,
                     0);
    //  DJI_Current_Ctrl(&hcan, 0x1ff, 
    //                  (int16_t)MOTOR->M6020[YAW].PID_S.Output,
    //                  0,
    //                  0,
    //                  0);
    return 0;
}



