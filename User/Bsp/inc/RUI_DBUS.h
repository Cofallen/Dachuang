/*
____/\\\\\\\\\_____        __/\\\________/\\\_        __/\\\\\\\\\\\_
 __/\\\///////\\\___        _\/\\\_______\/\\\_        _\/////\\\///__
  _\/\\\_____\/\\\___        _\/\\\_______\/\\\_        _____\/\\\_____
   _\/\\\\\\\\\\\/____        _\/\\\_______\/\\\_        _____\/\\\_____
    _\/\\\//////\\\____        _\/\\\_______\/\\\_        _____\/\\\_____
     _\/\\\____\//\\\___        _\/\\\_______\/\\\_        _____\/\\\_____
      _\/\\\_____\//\\\__        _\//\\\______/\\\__        _____\/\\\_____
       _\/\\\______\//\\\_        __\///\\\\\\\\\/___        __/\\\\\\\\\\\_
        _\///________\///__        ____\/////////_____        _\///////////__
*/
#ifndef RUI_V_DBUS_H
#define RUI_V_DBUS_H

#include "stdint.h"
// #include "RUI_MATH.h"
#include "MY_Define.h"
#include "string.h"
#include "gpio.h"
#include "usart.h"
// #include "Motors.h"

typedef struct
{
    uint16_t ONLINE_JUDGE_TIME; // 在线检测时间
    //遥控
    struct
    {
        int16_t CH0_int16;
        int16_t CH1_int16;
        int16_t CH2_int16;
        int16_t CH3_int16;
        int16_t Dial_int16;
        uint8_t S1_u8;
        uint8_t S2_u8;
        int8_t Error_int8;
    } Remote;

}DBUS_Typedef;


typedef union  // 使用共用体整合数据
{
    struct __packed
    {
// 遥控数据
        uint64_t CH0 : 11;
        uint64_t CH1 : 11;
        uint64_t CH2 : 11;
        uint64_t CH3 : 11;
        uint64_t S1 : 2;
        uint64_t S2 : 2;
        // 鼠标数据
        int64_t Mouse_X : 16;
        int64_t Mouse_Y : 16;
        int64_t Mouse_Z : 16;
        int64_t Mouse_R : 8;
        int64_t Mouse_L : 8;
        // 键盘数据
        uint64_t KeyBoard_W : 1;
        uint64_t KeyBoard_S : 1;
        uint64_t KeyBoard_A : 1;
        uint64_t KeyBoard_D : 1;
        uint64_t KeyBoard_Shift : 1;
        uint64_t KeyBoard_Ctrl : 1;
        uint64_t KeyBoard_Q : 1;
        uint64_t KeyBoard_E : 1;
        uint64_t KeyBoard_R : 1;
        uint64_t KeyBoard_F : 1;
        uint64_t KeyBoard_G : 1;
        uint64_t KeyBoard_Z : 1;
        uint64_t KeyBoard_X : 1;
        uint64_t KeyBoard_C : 1;
        uint64_t KeyBoard_V : 1;
        uint64_t KeyBoard_B : 1;
        // 遥控滑轮
        uint64_t Direction : 11;
        uint64_t : 0;
    } DataNeaten;
    // 接收到的数组
    uint8_t  GetData[19];
}DBUS_UNION_Typdef;

//遥控接收
void RUI_F_DUBS_Resovled(DBUS_UNION_Typdef *Data, DBUS_Typedef *RUI_V_DBUS);
void DBUS_OFFLINE_Check(DBUS_Typedef *RUI_V_DBUS);

#endif
