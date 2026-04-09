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
    
    OLED_HorizontalShift(0x26);//全屏水平向右滚动播放
}