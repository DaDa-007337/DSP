#include "DSP28x_Project.h"
#include "lcd_ili9341.h"



void main(void)
{
    InitSysCtrl();
    InitGpio();
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    LCD_Init();       // 模拟SPI初始化（和STM32一样）
    LCD_Clear(WHITE); // 白底

    // 显示你的信息
    LCD_ShowChinese(40, 60, "林楷晋", RED);
    LCD_ShowString(40, 100, "23124061059", BLUE);

    while(1)
    {

    }
}