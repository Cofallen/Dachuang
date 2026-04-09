#ifndef __ALL_INIT_H
#define __ALL_INIT_H

#include "main.h"
#include "tim.h"
#include "usart.h"

#include "RUI_DBUS.h"
#include "Vision.h"
#include "DJI_Motor.h"
#include "bsp_dwt.h"
#include "can_bsp.h"
#include "Gimbal_Task.h"



extern DBUS_Typedef DBUS_V_DATA;
extern DBUS_UNION_Typdef DBUS_V_UNION;
extern MOTOR_Typedef ALL_MOTOR;
extern VisionRxDataUnion VisionRxData;
extern CONTAL_Typedef ALL_CONTAL;

void All_Init(void);

#endif // !__ALL_INIT_H
