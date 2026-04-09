/*
 * @Descripttion: 
 * @version: 
 * @Author: Eugene
 * @Date: 2024-07-04 16:39:33
 * @LastEditors: Andy
 * @LastEditTime: 2024-07-05 15:48:31
 */
#ifndef __MY_DEFINE
#define __MY_DEFINE

//INIT成功
#define RUI_DF_READY 1
//INIT错误
#define RUI_DF_ERROR 0

#define RUI_DF_NOW  1
#define RUI_DF_LAST 0

//PID双环
#define RUI_DF_PID_DOUBLE 1
//PID单环
#define RUI_DF_PID_SINGLE 0
//正在判断主控位置
#define RUI_DF_MASTER_LOCATION_JUDGEING 0
//主控位置判断完成，在头部
#define RUI_DF_MASTER_LOCATION_HEAD 1
//主控位置判断完成，在底盘
#define RUI_DF_MASTER_LOCATION_BOTTOM 2


#define INIT_ANGLE 2179
#define WIPE_MAX_SPEED 7200
#define SINGLE_ANGLE 36864
/************************************************************万能分隔符**************************************************************
 * 	@author:			//小瑞
 *	@performance:	    //离线检测宏定义
 *	@parameter:		    //
 *	@time:				//23-03-28 20:13
 *	@ReadMe:			//
 ************************************************************万能分隔符**************************************************************/
//设备离线
#define RUI_DF_OFFLINE 0
//设备在线
#define RUI_DF_ONLINE  1 
//遥控器离线检测时间
#define RUI_DF_DBUS_OFFLINE_TIME 30  
//电机离线检测时间
#define RUI_DF_MOTOR_OFFLINE_TIME 15

// 云台电机ID
#define YAW   0
#define PITCH 1


// GPS 接收缓冲区（大小根据最大 NMEA 句子长度，一般 128 足够）
#define GPS_RX_BUF_SIZE  128



#endif
