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

uint8_t mode = 0; // 当前模式，0: 菜单页面, 1: 信息模式, 2: Happy 情绪, 3: Sad 情绪, 4: Surprised, 5: Afraid, 6: Relaxed
uint8_t menu_index = 0; // 当前菜单选项索引
uint8_t last_mode = 0xFF; // 上一次的模式，用于检测模式切换
char current_mood[16] = "Happy"; // 当前情绪状态

uint8_t flag = 0; // WIFI连接状态标志，0: 连接失败, 1: 连接成功

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

    OLED_ShowString(0, 4, "WIFI Connect:", 12, 0);
    if (flag == 0)
    {
        OLED_ShowString(80, 4, "failed", 12, 0);
    }
    else
    {
        OLED_ShowString(80, 4, "success", 12, 0);
    }

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
    OLED_ShowString(0, 2, menu_index == 0 ? "> Information" : "  Information", 12, 0); // 信息模式
    OLED_ShowString(0, 3, menu_index == 1 ? "> Happy" : "  Happy", 12, 0);             // Happy 情绪
    OLED_ShowString(0, 4, menu_index == 2 ? "> Sad Mood" : "  Sad Mood", 12, 0);      // Sad 情绪
    OLED_ShowString(0, 5, menu_index == 3 ? "> Surprised" : "  Surprised", 12, 0);    // Surprised 情绪
    OLED_ShowString(0, 6, menu_index == 4 ? "> Afraid" : "  Afraid", 12, 0);          // Afraid 情绪
    OLED_ShowString(0, 7, menu_index == 5 ? "> Relaxed" : "  Relaxed", 12, 0);        // Relaxed 情绪
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

/// 后面是波形
void OLED_ShowHappyWaveform(void) {
    // Happy 情绪波形逻辑（与之前相同）
    OLED_ShowWaveform();
}

void OLED_ShowSadWaveform(void) {
    OLED_ShowSadMoodWaveform();
}

void OLED_ShowSurprisedWaveform(void) {
    static uint8_t buffer[128] = {0};
    static float phase = 0.0f;
    static uint8_t x_offset = 0;

    float new_y = 32.0f + 20.0f * sinf(phase) + (rand() % 10 - 5); // 高振幅 + 噪声
    if (new_y < 0) new_y = 0;
    if (new_y > 63) new_y = 63;

    buffer[x_offset] = (uint8_t)new_y;

    for (uint8_t i = 0; i < 8; i++) {
        OLED_Set_Pos(x_offset, i);
        OLED_WR_DATA(0x00);
    }

    OLED_Set_Pos(x_offset, buffer[x_offset] / 8);
    OLED_WR_DATA(1 << (buffer[x_offset] % 8));

    x_offset = (x_offset + 1) % 128;
    phase += 0.2f;
    if (phase > 2 * M_PI) {
        phase -= 2 * M_PI;
    }
}

void OLED_ShowAfraidWaveform(void) {
    static uint8_t buffer[128] = {0};
    static float phase = 0.0f;
    static uint8_t x_offset = 0;

    float new_y = 32.0f + 10.0f * sinf(phase * 3.0f) + (rand() % 5 - 2); // 高频正弦波 + 噪声
    if (new_y < 0) new_y = 0;
    if (new_y > 63) new_y = 63;

    buffer[x_offset] = (uint8_t)new_y;

    for (uint8_t i = 0; i < 8; i++) {
        OLED_Set_Pos(x_offset, i);
        OLED_WR_DATA(0x00);
    }

    OLED_Set_Pos(x_offset, buffer[x_offset] / 8);
    OLED_WR_DATA(1 << (buffer[x_offset] % 8));

    x_offset = (x_offset + 1) % 128;
    phase += 0.3f;
    if (phase > 2 * M_PI) {
        phase -= 2 * M_PI;
    }
}

void OLED_ShowRelaxedWaveform(void) {
    static uint8_t buffer[128] = {0};
    static float phase = 0.0f;
    static uint8_t x_offset = 0;

    float new_y = 32.0f + 5.0f * sinf(phase); // 低频正弦波
    if (new_y < 0) new_y = 0;
    if (new_y > 63) new_y = 63;

    buffer[x_offset] = (uint8_t)new_y;

    for (uint8_t i = 0; i < 8; i++) {
        OLED_Set_Pos(x_offset, i);
        OLED_WR_DATA(0x00);
    }

    OLED_Set_Pos(x_offset, buffer[x_offset] / 8);
    OLED_WR_DATA(1 << (buffer[x_offset] % 8));

    x_offset = (x_offset + 1) % 128;
    phase += 0.05f;
    if (phase > 2 * M_PI) {
        phase -= 2 * M_PI;
    }
}

void StartKeyTask(void) {
    for (;;) {
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET) {
            if (mode == 0) {
                menu_index = (menu_index + 1) % 6; // 菜单选项增加到 6
                OLED_ShowMenu();
            } else {
                mode = 0;
                OLED_Clear();
                OLED_ShowMenu();
            }
            while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET) {
                osDelay(10);
            }
        }

        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET) {
            if (mode == 0) {
                mode = menu_index + 1;
                switch (mode) {
                    case 2: snprintf(current_mood, sizeof(current_mood), "Happy"); break;
                    case 3: snprintf(current_mood, sizeof(current_mood), "Sad"); break;
                    case 4: snprintf(current_mood, sizeof(current_mood), "Surprised"); break;
                    case 5: snprintf(current_mood, sizeof(current_mood), "Afraid"); break;
                    case 6: snprintf(current_mood, sizeof(current_mood), "Relaxed"); break;
                }
                OLED_Clear();
            }
            while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET) {
                osDelay(10);
            }
        }

        osDelay(10);
    }
}

void StartDisplayTask(void) {
    for (;;) {
        if (mode != last_mode) {
            OLED_Clear();
            last_mode = mode;
        }

        if (mode == 0) {
            OLED_ShowMenu();
        } else if (mode == 1) {
            OLED_ShowInformation();
        } else if (mode == 2) {
            OLED_ShowHappyWaveform();
        } else if (mode == 3) {
            OLED_ShowSadWaveform();
        } else if (mode == 4) {
            OLED_ShowSurprisedWaveform();
        } else if (mode == 5) {
            OLED_ShowAfraidWaveform();
        } else if (mode == 6) {
            OLED_ShowRelaxedWaveform();
        }

        osDelay(1);
    }
}