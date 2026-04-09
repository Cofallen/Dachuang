#include "IRQN.h"
#include "MY_Define.h"
#include "All_init.h"
#include "Vision.h"
#include "gps.h"
#include "usart.h"
#include "esp.h"

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef RXMessage;
    uint8_t Data[8];
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RXMessage, Data);

    if (hcan->Instance == CAN1) {
        switch (RXMessage.StdId)
        {
        case (0X205+YAW):
        {
            MotorResolve(&ALL_MOTOR.M6020[YAW], Data);
            MotorRoundResolve(&ALL_MOTOR.M6020[YAW]);
        }
        break;
        case (0x205+PITCH):
        {
            MotorResolve(&ALL_MOTOR.M6020[PITCH], Data);
            MotorRoundResolve(&ALL_MOTOR.M6020[PITCH]);
        }
        break;
        }
    } 
}

// 串口
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        
    } 
    if (huart->Instance == USART2) {
        
    }
    if (huart->Instance == USART3)
    {
        
    }
}
uint8_t rx_byte;
uint8_t data1[4] = {0};
uint16_t len;
// 空闲中断处理函数  TODO 优化成直接调用
void User_IRQHandler(UART_HandleTypeDef *huart)
{
	if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);

        HAL_UART_DMAStop(&huart1);

        uint16_t len = GPS_RX_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);

        if (len > 0 && len < GPS_RX_SIZE) {
            GPS_ParseBuffer(gps_rx_buffer, len);
        }

        // ⭐ 关键：清buffer（避免旧数据残留）
        memset(gps_rx_buffer, 0, GPS_RX_SIZE);

        HAL_UART_Receive_DMA(&huart1, gps_rx_buffer, GPS_RX_SIZE);
    }
	
		// 以下 USART2 和 USART3 保持原样
    if (huart->Instance == USART2)
    {
       
    }
    if (huart->Instance == USART3)
    {
//        if(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE))
//        {
//            __HAL_UART_CLEAR_IDLEFLAG(&huart3); 
//            HAL_UART_DMAStop(&huart3);          
//            DBUS_V_DATA.ONLINE_JUDGE_TIME = 0;
//            RUI_F_DUBS_Resovled(&DBUS_V_UNION, &DBUS_V_DATA);
//            HAL_UART_Receive_DMA(&huart3, (uint8_t *)DBUS_V_UNION.GetData, 19);
//        }
		ESP_UART_IdleCallback();   // 处理空闲中断
    }
}