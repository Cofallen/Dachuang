#include "print.h"
#include "oled.h"
#include "oledfont.h"
#include "math.h"
#include "stdlib.h" // 用于生成随机噪声
#include "gpio.h"  

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

void OLED_ShowNumbers(void) {
    // OLED_Clear(); // 清屏
    OLED_ShowString(0, 0, "GPS:", 16, 0); // 显示 "GPS:"

    // 假设 GPS 坐标为以下变量（需要根据实际代码替换）
    float latitude = 39.6305;  // 纬度
    float longitude = 118.1806; // 经度
    char lat_str[16], lon_str[16];

    // 格式化纬度和经度为字符串
    snprintf(lat_str, sizeof(lat_str), "Lat:%.4f", latitude);
    snprintf(lon_str, sizeof(lon_str), "Lon:%.4f", longitude);

    // 显示纬度和经度
    OLED_ShowString(0, 2, lat_str, 12, 0); // 显示纬度
    OLED_ShowString(0, 4, lon_str, 12, 0); // 显示经度

    // 显示情绪状态
    OLED_ShowString(0, 6, "Mood: Happy", 12, 0); // 假设情绪状态为 "Happy"

    // 显示当前城市
    OLED_ShowString(0, 7, "City: Tangshan", 12, 0); // 显示 "唐山"
}

#include "gpio.h" // 确保包含 GPIO 配置头文件

void StartKeyTask(void) {
    static uint8_t mode = 0xFF; // 当前模式，初始化为无效值
    uint8_t new_mode = 0;       // 新模式

    for (;;) {
        // 检测按键 1 是否按下
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET) { // 按键按下
            new_mode = 0; // 切换到波形模式
            while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET) {
                osDelay(10); // 防抖
            }
        }

        // 检测按键 2 是否按下
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET) { // 按键按下
            new_mode = 1; // 切换到数字模式
            while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET) {
                osDelay(10); // 防抖
            }
        }

        // 如果模式发生变化，清屏并切换模式
        if (new_mode != mode) {
            mode = new_mode;
            OLED_Clear(); // 清屏

            if (mode == 0) {
                OLED_ShowWaveform(); // 显示动态波形
            } else if (mode == 1) {
                OLED_ShowNumbers(); // 显示数字状态
            }
        }

        // 根据当前模式刷新内容
        if (mode == 0) {
            OLED_ShowWaveform(); // 持续更新波形
        }

        osDelay(1); // 控制刷新频率
    }
}