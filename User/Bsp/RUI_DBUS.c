#include "RUI_DBUS.h"
#include "can.h"
#include "All_init.h"
/************************************************************万能分隔符**************************************************************
 * 	@author:			//  小瑞 COPY FORM 赵澍
 *	@performance:	    //
 *	@parameter:		    //
 *	@time:				//  22-11-21 18:47
 *	@ReadMe:	        //  这个版本有遥控滚轮的控制，但是某些遥控没有办法正确传回滚轮数据
                        //	遥控通道说明图
                        //	^						^
                        //	|						|
                        //	|ch2---->		        |ch0---->
                        //	|						|
                        //	ch3					    ch1
                        //  共用体接收
 ************************************************************万能分隔符**************************************************************/
void RUI_F_DUBS_Resovled( DBUS_UNION_Typdef *Data, DBUS_Typedef *RUI_V_DBUS)
{
    // RUI_V_DBUS->RUI_V_DBUS_ONLINE_JUDGE_TIME = RUI_DF_DBUS_OFFLINE_TIME;

    RUI_V_DBUS->Remote.S1_u8 = Data->DataNeaten.S1;
    RUI_V_DBUS->Remote.S2_u8 = Data->DataNeaten.S2;

    RUI_V_DBUS->Remote.CH0_int16  = Data->DataNeaten.CH0 -1024;
    RUI_V_DBUS->Remote.CH1_int16  = Data->DataNeaten.CH1 -1024;
    RUI_V_DBUS->Remote.CH2_int16  = Data->DataNeaten.CH2 -1024;
    RUI_V_DBUS->Remote.CH3_int16  = Data->DataNeaten.CH3 -1024;
    RUI_V_DBUS->Remote.Dial_int16 = Data->DataNeaten.Direction -1024;

    // if (RUI_V_DBUS_UNION.DataNeaten.CH0 == 0)
    // {
    //     RUI_V_DBUS->Remote.CH0_int16  = 0;
    //     RUI_V_DBUS->Remote.CH1_int16  = 0;
    //     RUI_V_DBUS->Remote.CH2_int16  = 0;
    //     RUI_V_DBUS->Remote.CH3_int16  = 0;
    //     RUI_V_DBUS->Remote.Dial_int16 = 0;
    // }
}


void DBUS_OFFLINE_Check(DBUS_Typedef *RUI_V_DBUS)
{
    RUI_V_DBUS->ONLINE_JUDGE_TIME++;
    if (RUI_V_DBUS->ONLINE_JUDGE_TIME > 100)
    {
        RUI_V_DBUS->ONLINE_JUDGE_TIME = 100;
    }
    // if (RUI_V_DBUS->ONLINE_JUDGE_TIME >= 100)
    // {
    //     // 遥控器离线处理
    //     DJI_Current_Ctrl(&hcan, 0x1ff, 
    //                 0,
    //                 0,
    //                 0,
    //                 0);
    // } else
    // {
    //     DJI_Current_Ctrl(&hcan, 0x1ff, 
    //                 (int16_t)ALL_MOTOR.M6020[YAW].PID_S.Output,
    //                 (int16_t)ALL_MOTOR.M6020[PITCH].PID_S.Output,
    //                 0,
    //                 0);
    // }
}
