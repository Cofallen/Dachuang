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

    StartDisplayTask();
    for(;;)
    { 	
        osDelay(1);
    }
}

void StartMonitorTask(void)
{
//    esp_task_entry();
	StartKeyTask();
    for(;;)
    {
        osDelay(1);
    }
}
