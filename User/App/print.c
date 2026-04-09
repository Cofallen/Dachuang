#include "print.h"
#include "oled.h"
#include "oledfont.h"
#include "math.h"
#include "stdlib.h" // 用于生成随机噪声
#include "gpio.h"  
#include "bsp_dwt.h" // 用于获取单片机运行时间
#include "cmsis_os.h"

#define M_PI 3.1415926f

void oled_initc(void)
{
    OLED_Init();                           //OLED初始
    OLED_Clear();                         //清屏
    //上面的初始化以及清屏的代码在一开始处一定要写
    OLED_ShowString(0,0,"UNICORN_LI",16, 1);    //反相显示8X16字符串
    OLED_ShowString(0,2,"unicorn_li_123",12,0);//正相显示6X8字符串
    
    OLED_ShowCHinese(0,4,0,1); //反相显示汉字“独”
    OLED_ShowCHinese(16,4,1,1);//反相显示汉字“角”
    OLED_ShowCHinese(32,4,2,1);//反相显示汉字“兽”
    OLED_ShowCHinese(0,6,0,0); //正相显示汉字“独”
    OLED_ShowCHinese(16,6,1,0);//正相显示汉字“角”
    OLED_ShowCHinese(32,6,2,0);//正相显示汉字“兽”

    OLED_ShowNum(48,4,6,1,16, 0);//正相显示1位8X16数字“6”
    OLED_ShowNum(48,7,77,2,12, 1);//反相显示2位6X8数字“77”
    OLED_DrawBMP(90,0,122, 4,BMP1,0);//正相显示图片BMP1
    OLED_DrawBMP(90,4,122, 8,BMP1,1);//反相显示图片BMP1
    
    // OLED_HorizontalShift(0x26);//全屏水平向右滚动播放
}

void OLED_ShowWaveform(void) {
    static uint8_t buffer[128] = {0}; // 用于存储当前帧的波形数据
    static float phase = 0.0f;        // 用于控制正弦波的相位
    static uint8_t x_offset = 0;     // 用于滚动更新的偏移量

    // 生成新数据点
    float new_y = 32.0f + 16.0f * sinf(phase) + 
                  8.0f * sinf(phase * 2.0f) +
                  (rand() % 5 - 2); // 小噪声（-2 到 2）

    // 限制 y 的范围在 0 到 63（OLED 高度）
    if (new_y < 0) new_y = 0;
    if (new_y > 63) new_y = 63;

    // 将新数据点存入缓冲区
    buffer[x_offset] = (uint8_t)new_y;

    // 清除当前列的像素（避免闪烁）
    for (uint8_t i = 0; i < 8; i++) {
        OLED_Set_Pos(x_offset, i);
        OLED_WR_DATA(0x00); // 清空当前列
    }

    // 更新当前列的像素
    OLED_Set_Pos(x_offset, buffer[x_offset] / 8);
    OLED_WR_DATA(1 << (buffer[x_offset] % 8));

    // 更新偏移量，实现滚动效果
    x_offset = (x_offset + 1) % 128;

    // 更新相位，制造动态效果
    phase += 0.1f;
    if (phase > 2 * M_PI) {
        phase -= 2 * M_PI;
    }
}

float runtime = 0.0f;

uint8_t mode = 0; // 当前模式，0: 菜单页面, 1: 信息模式, 2: Happy 情绪, 3: Sad 情绪
uint8_t menu_index = 0; // 当前菜单选项索引
uint8_t last_mode = 0xFF; // 上一次的模式，用于检测模式切换
char current_mood[16] = "Happy"; // 当前情绪状态

void OLED_ShowInformation(void) {
    OLED_ShowString(0, 0, "GPS:", 16, 0); // 显示 "GPS:"

    // 假设 GPS 坐标为以下变量（需要根据实际代码替换）
    float latitude = 39.6305;  // 纬度
    float longitude = 118.1806; // 经度
    char lat_str[16], lon_str[16], time_str[16];

    // 格式化纬度和经度为字符串
    snprintf(lat_str, sizeof(lat_str), "Lat:%.4f", latitude);
    snprintf(lon_str, sizeof(lon_str), "Lon:%.4f", longitude);

    // 显示纬度和经度
    OLED_ShowString(0, 2, lat_str, 12, 0); // 显示纬度
    OLED_ShowString(0, 3, lon_str, 12, 0); // 显示经度

    // 显示情绪状态（与上一次选择的情绪相关）
    OLED_ShowString(0, 6, "Mood:", 12, 0);
    OLED_ShowString(36, 6, current_mood, 12, 0); // 显示当前情绪

    // 显示当前城市
    OLED_ShowString(0, 7, "City: Tangshan", 12, 0); // 显示 "唐山"

    // 获取单片机运行时间
    runtime = DWT_GetTimeline_s();
    snprintf(time_str, sizeof(time_str), "Time:%.2fs", runtime);

    // 显示运行时间
    OLED_ShowString(0, 5, time_str, 12, 0); // 显示运行时间
}




#include "gpio.h" // 确保包含 GPIO 配置头文件



void OLED_ShowMenu(void) {
    // OLED_Clear(); // 清屏
    OLED_ShowString(0, 0, "Menu:", 16, 0); // 显示菜单标题

    // 显示菜单选项
    OLED_ShowString(0, 2, menu_index == 0 ? "> information" : "  information", 12, 0); // 信息模式
    OLED_ShowString(0, 3, menu_index == 1 ? "> Happy Mood" : "  Happy Mood", 12, 0);   // Happy 情绪
    OLED_ShowString(0, 4, menu_index == 2 ? "> Sad Mood" : "  Sad Mood", 12, 0); // 悲伤情绪
}

void OLED_ShowSadMoodWaveform(void) {
    static uint8_t buffer[128] = {0}; // 用于存储当前帧的波形数据
    static float phase = 0.0f;        // 用于控制正弦波的相位
    static uint8_t x_offset = 0;     // 用于滚动更新的偏移量

    // 生成新数据点（悲伤情绪波形：低频正弦波 + 噪声）
    float new_y = 32.0f + 8.0f * sinf(phase) +
                  4.0f * sinf(phase * 0.5f) +
                  (rand() % 3 - 1); // 小噪声（-1 到 1）

    // 限制 y 的范围在 0 到 63（OLED 高度）
    if (new_y < 0) new_y = 0;
    if (new_y > 63) new_y = 63;

    // 将新数据点存入缓冲区
    buffer[x_offset] = (uint8_t)new_y;

    // 清除当前列的像素（避免闪烁）
    for (uint8_t i = 0; i < 8; i++) {
        OLED_Set_Pos(x_offset, i);
        OLED_WR_DATA(0x00); // 清空当前列
    }

    // 更新当前列的像素
    OLED_Set_Pos(x_offset, buffer[x_offset] / 8);
    OLED_WR_DATA(1 << (buffer[x_offset] % 8));

    // 更新偏移量，实现滚动效果
    x_offset = (x_offset + 1) % 128;

    // 更新相位，制造动态效果
    phase += 0.05f; // 悲伤情绪波形频率较低
    if (phase > 2 * M_PI) {
        phase -= 2 * M_PI;
    }
}

void StartKeyTask(void) {
    for (;;) {
        // 检测按键 1 是否按下（返回/切换菜单）
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET) { // 按键按下
            if (mode == 0) { // 如果在菜单页面
                menu_index = (menu_index + 1) % 3; // 切换到下一个菜单选项
                OLED_ShowMenu(); // 更新菜单显示
            } else {
                mode = 0; // 返回菜单页面
                OLED_Clear(); // 清屏
                OLED_ShowMenu(); // 显示菜单
            }
            while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET) {
                osDelay(10); // 防抖
            }
        }

        // 检测按键 2 是否按下（确认/进入模式）
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET) { // 按键按下
            if (mode == 0) { // 如果在菜单页面
                mode = menu_index + 1; // 根据菜单选项进入对应模式
                if (mode == 3) { // 如果进入悲伤情绪模式
                    snprintf(current_mood, sizeof(current_mood), "Sad"); // 更新情绪状态
                } else if (mode == 2) { // 如果进入 Happy 情绪模式
                    snprintf(current_mood, sizeof(current_mood), "Happy"); // 更新情绪状态
                }
                OLED_Clear(); // 清屏
            }
            while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET) {
                osDelay(10); // 防抖
            }
        }

        osDelay(10); // 控制检测频率
    }
}

void StartDisplayTask(void) {
    for (;;) {
        // 检测模式是否切换
        if (mode != last_mode) {
            OLED_Clear(); // 仅在模式切换时清屏
            last_mode = mode; // 更新上一次的模式
        }

        // 根据当前模式刷新显示内容
        if (mode == 0) {
            OLED_ShowMenu(); // 显示菜单页面
        } else if (mode == 1) {
            OLED_ShowInformation(); // 显示信息页面
        } else if (mode == 2) {
            OLED_ShowWaveform(); // 显示 Happy 情绪波形
        } else if (mode == 3) {
            OLED_ShowSadMoodWaveform(); // 显示 Sad 情绪波形
        }

        osDelay(1); // 控制刷新频率
    }
}