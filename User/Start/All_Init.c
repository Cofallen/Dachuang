#include "All_Init.h"
#include "gps.h"

//遥控相关变量
DBUS_UNION_Typdef DBUS_V_UNION = {0};
DBUS_Typedef DBUS_V_DATA = { 0 };

//电机
MOTOR_Typedef ALL_MOTOR;

//视觉
VisionRxDataUnion VisionRxData = {0};

CONTAL_Typedef ALL_CONTAL = {0};

void All_Init(void)
{   
    DWT_Init(72);
    CAN_Filter_Init();

    HAL_TIM_Base_Start_IT(&htim2);

	// __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);//遥控串口
    // HAL_UART_Receive_DMA(&huart3, (uint8_t *)DBUS_V_UNION.GetData, 19);

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);//视觉串口
    HAL_UART_Receive_DMA(&huart2, (uint8_t *)VisionRxData.OriginData, sizeof(VisionRxData.OriginData));

    ALL_CONTAL.HEAD.Pitch_MAX =  800.0f;
    ALL_CONTAL.HEAD.Pitch_MIN = -800.0f;
    ALL_CONTAL.HEAD.Yaw_Init  = (float)ALL_MOTOR.M6020[YAW].DATA.Angle_Infinite;
    MOTOR_PID_Gimbal_INIT(&ALL_MOTOR);
	
	// 使用同 gps.c 中定义的大小，避免越界
    HAL_UART_Receive_DMA(&huart1, gps_rx_buffer, GPS_RX_SIZE);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);   // 使能空闲中断
}