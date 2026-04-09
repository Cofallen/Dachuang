#include "All_Task.h"
#include "main.h"
#include "cmsis_os.h"
#include "VOFA.h"
#include "All_init.h"
#include "can_bsp.h"
#include "Robot.h"
#include "bsp_dwt.h"
#include "gps.h"
#include "esp.h"
#include "stdio.h"
#include "print.h"

void StartDefaultTask(void)
{
    All_Init();
    for(;;)
    {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
        osDelay(500);
    }
}

void StartGimbalTask(void)
{
    oled_initc();
    for(;;)
    {
        ALL_CONTAL.DWT_TIME.Gimbal_dt = DWT_GetDeltaT(&ALL_CONTAL.DWT_TIME.Gimbal_Count);
        ALL_CONTAL.DWT_TIME.Gimbal_time = DWT_GetTimeline_ms();
		
        osDelay(1);
    }
}

void StartMonitorTask(void)
{
//    esp_task_entry();
	StartKeyTask();
    for(;;)
    {
        ALL_CONTAL.DWT_TIME.Monitor_dt = DWT_GetDeltaT(&ALL_CONTAL.DWT_TIME.Monitor_Count);
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET)
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
        }

        osDelay(1);
    }
}
