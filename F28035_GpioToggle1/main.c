#include "DSP28x_Project.h"  // 引用头文件，包含所有寄存器/函数定义
#define LED_PIN    34        // LED引脚，根据数据手册可知：GPIO0~31→GPA组，GPIO32~63→GPB组
#define LED_ON     1         // 1=拉高点亮,共阴LED：0=拉低点亮,共阳LED;
#define DELAY_CNT  500000    // 闪烁频率, 60MHz主频下：50万次≈500ms，100万次≈1秒

void delay(void)
{
    unsigned long i;             // 定义无符号长整型计数器（范围0~4294967295）
    for(i=0; i<DELAY_CNT; i++);  // 空循环延时：从0计数到DELAY_CNT，循环结束即完成延时
}
void LED_Init(void)
{
    EALLOW;                                   // 解锁受保护寄存器 
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;      // 配置GPIO34为"普通GPIO模式"
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;       // 配置GPIO34为"输出模式" 
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 1;       // 禁用GPIO34的内部上拉电阻
    EDIS;                                     // 锁定受保护寄存器
    // 设置LED初始电平
    if(LED_ON == 0){
        GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;}
    else{
        GpioDataRegs.GPBSET.bit.GPIO34 = 1;}
}
void main(void)
{
    InitSysCtrl();  // 初始化：关闭看门狗+配置系统时钟（60MHz）+初始化外设时钟
    LED_Init();     // 调用LED GPIO初始化函数：
    while(1)
    {
        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;//翻转GPIO34电平：高→低 或 低→高
        delay();
    }
}
