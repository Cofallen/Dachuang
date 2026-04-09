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
    for(;;)
    {
        ALL_CONTAL.DWT_TIME.Gimbal_dt = DWT_GetDeltaT(&ALL_CONTAL.DWT_TIME.Gimbal_Count);
        ALL_CONTAL.DWT_TIME.Gimbal_time = DWT_GetTimeline_ms();
		
        osDelay(1);
    }
}

void StartMonitorTask(void)
{
    esp_task_entry();
	
    for(;;)
    {
        ALL_CONTAL.DWT_TIME.Monitor_dt = DWT_GetDeltaT(&ALL_CONTAL.DWT_TIME.Monitor_Count);
        
        osDelay(1);
    }
}
