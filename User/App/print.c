#include "print.h"
#include "oled.h"
#include "oledfont.h"

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

// 显示波形
void OLED_ShowWaveform(void) {
    OLED_Clear();
    for (uint8_t x = 0; x < 128; x++) {
        uint8_t y = (uint8_t)(32 + 16 * sin(x * 0.1)); // 简单正弦波
        OLED_Set_Pos(x, y / 8); // 设置位置
        OLED_WR_DATA(1 << (y % 8)); // 点亮对应像素
    }
}

// 显示数字状态
void OLED_ShowNumbers(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "Number Mode", 16, 0);
    OLED_ShowNum(0, 2, 12345, 5, 16, 0); // 显示数字 12345
}

#include "gpio.h" // 确保包含 GPIO 配置头文件

void StartKeyTask(void) {
    for (;;) {
        // 检测按键 1 是否按下
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET) { // 按键按下
            OLED_ShowWaveform(); // 显示波形
            while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET) {
                osDelay(10); // 防抖
            }
        }

        // 检测按键 2 是否按下
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET) { // 按键按下
            OLED_ShowNumbers(); // 显示数字状态
            while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET) {
                osDelay(10); // 防抖
            }
        }

        osDelay(10); // 任务延时
    }
}