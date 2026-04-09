#ifndef __GIMBAL_TASK_H
#define __GIMBAL_TASK_H

#include "main.h"
#include "All_init.h"

uint8_t MOTOR_PID_Gimbal_INIT(MOTOR_Typedef *motor);
uint8_t Gimbal_AIM_INIT();
uint8_t gimbal_task(CONTAL_Typedef *CONTAL, MOTOR_Typedef *MOTOR);

#endif
