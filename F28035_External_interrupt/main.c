#include "DSP28x_Project.h"  // 引用头文件

// 引脚定义
#define LED_PIN     34        // LED引脚（GPB组）
#define KEY_PIN     27        // 按键引脚（GPA组，GPIO12支持外部中断XINT1）
#define LED_ON      1         // 1=拉高点亮（根据LED类型调整）
#define KEY_PRESS   0         // 按键按下为低电平（共地按键）

// 全局变量：用于按键消抖
volatile Uint16 key_int_flag = 0;  // 按键中断标志
volatile Uint32 key_debounce_cnt = 0; // 消抖计数器

// 函数前置声明（解决编译未定义问题）
interrupt void xint1_isr(void);    // 外部中断1服务函数
void LED_Init(void);               // LED初始化
void KEY_Init(void);               // 按键中断初始化
void DelayUs(Uint32 us);           // 微秒级延时（消抖用）

// LED初始化函数
void LED_Init(void)
{
    EALLOW;
    // 配置GPIO34为普通GPIO输出
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;    // 普通GPIO模式
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;     // 输出模式
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 1;     // 禁用上拉
    EDIS;
    
    // LED初始状态
    if(LED_ON == 0){
        GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
    }else{
        GpioDataRegs.GPBSET.bit.GPIO34 = 1;
    }
}

// 按键+外部中断初始化（核心：配置XINT1）
void KEY_Init(void)
{
    EALLOW;
    // 1. 配置GPIO12为普通GPIO输入
    GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 1;    // 普通GPIO模式
    GpioCtrlRegs.GPADIR.bit.GPIO27 = 0;     // 输入模式
    GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;     // 使能内部上拉（按键松开为高电平）
    
    // 2. 映射GPIO12到外部中断XINT1
    GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = KEY_PIN;  // GPIO12关联XINT1
    
    // 3. 配置XINT1中断触发方式（下降沿触发：按键按下）
    XIntruptRegs.XINT1CR.bit.POLARITY = 0;  // 0=下降沿触发，1=上升沿触发，2=双边沿
    
    // 4. 配置PIE中断控制器
    PieVectTable.XINT1 = &xint1_isr;        // 映射XINT1中断服务函数
    EDIS;
    
    // 5. 使能各级中断
    XIntruptRegs.XINT1CR.bit.ENABLE = 1;    // 使能XINT1中断
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;      // 使能PIE组1的4号中断（XINT1）
    IER |= M_INT1;                          // 使能CPU级的INT1中断
    EINT;                                   // 使能全局中断
    ERTM;                                   // 使能实时模式中断
}

// 微秒级延时（消抖用，60MHz主频）
void DelayUs(Uint32 us)
{
    Uint32 i;
    for(; us>0; us--)
    {
        for(i=0; i<60; i++);  // 60个指令周期≈1us（60MHz主频）
    }
}

// 外部中断1服务函数（按键中断处理）
interrupt void xint1_isr(void)
{
    // 1. 清除中断标志（必须）
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;  // 清除PIE组1中断挂起标志
    XIntruptRegs.XINT1CR.bit.ENABLE = 0;     // 暂时关闭中断，防止重复触发
    
    // 2. 按键消抖（延时10ms）
    DelayUs(10000);
    
    // 3. 确认按键确实按下
    if(GpioDataRegs.GPADAT.bit.GPIO27 == KEY_PRESS)
    {
        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;  // 翻转LED电平
    }
    
    // 4. 延时后重新使能中断（防止松手抖动触发）
    DelayUs(10000);
    XIntruptRegs.XINT1CR.bit.ENABLE = 1;     // 重新使能XINT1中断
}

void main(void)
{
    // 系统初始化
    InitSysCtrl();          // 关闭看门狗+配置系统时钟（60MHz）
    DINT;                   // 关闭全局中断
    InitPieCtrl();          // 初始化PIE控制器
    IER = 0x0000;           // 清除CPU中断使能
    IFR = 0x0000;           // 清除CPU中断标志
    InitPieVectTable();     // 初始化PIE中断向量表
    
    // 外设初始化
    LED_Init();             // LED初始化
    KEY_Init();             // 按键+外部中断初始化
    
    // 主循环（空循环，LED翻转由中断触发）
    while(1)
    {
        // 主循环可添加其他任务，不影响按键中断
    }
}
