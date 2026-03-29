
#include "DSP28x_Project.h"  // 引用头文件，包含所有寄存器/函数定义

#define LED_PIN    34        // LED引脚，GPIO34属于GPB组
#define LED_ON     1         // 1=拉高点亮
#define TIMER_FREQ 1000      // 定时器中断频率(Hz)，1000Hz=1ms中断一次
#define BLINK_FREQ 1         // LED闪烁频率(Hz)，1Hz=1秒闪烁一次

// 全局变量：用于中断计数
volatile Uint16 timer_count = 0;
// 声明中断服务函数
interrupt void cpu_timer0_isr(void);
// LED初始化函数
void LED_Init(void)
{
    EALLOW;                                   // 解锁受保护寄存器 
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;      // 配置GPIO34为普通GPIO模式
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;       // 配置GPIO34为输出模式 
    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 1;       // 禁用GPIO34的内部上拉电阻
    EDIS;                                     // 锁定受保护寄存器
    
    // 设置LED初始电平
    if(LED_ON == 0){
        GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
    }else{
        GpioDataRegs.GPBSET.bit.GPIO34 = 1;
    }
}
// 定时器0初始化函数（配置中断）
void Timer0_Init(void)
{
    // 1. 禁用定时器0，进入配置模式
    CpuTimer0Regs.TCR.bit.TSS = 1;            // 停止定时器0
    CpuTimer0Regs.TCR.bit.TRB = 1;            // 重载周期寄存器
    // 2. 配置定时器周期（60MHz主频）
    // 周期计算公式：Period = (主频 / 中断频率) - 1
    CpuTimer0Regs.PRD.all = (Uint32)(60000000.0 / TIMER_FREQ) - 1;
    // 3. 配置定时器控制寄存器
    CpuTimer0Regs.TCR.bit.TIE = 1;            // 使能定时器0中断
    CpuTimer0Regs.TCR.bit.FREE = 1;           // 调试时定时器继续运行
    CpuTimer0Regs.TCR.bit.SOFT = 0;
    // 4. 配置PIE中断控制器
    EALLOW;
    PieVectTable.TINT0 = &cpu_timer0_isr;     // 映射定时器0中断服务函数
    EDIS;
    
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;        // 使能PIE组1的7号中断（TINT0）
    IER |= M_INT1;                            // 使能CPU级的INT1中断
    EINT;                                     // 使能全局中断
    ERTM;                                     // 使能实时模式中断
    // 5. 启动定时器0
    CpuTimer0Regs.TCR.bit.TSS = 0;            // 启动定时器0
}
// 定时器0中断服务函数（中断发生时执行）
interrupt void cpu_timer0_isr(void)
{
    // 1. 清除中断标志（必须！否则会一直进入中断）
    CpuTimer0Regs.TCR.bit.TIF = 1;            // 清除定时器0中断标志
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // 清除PIE组1的中断挂起标志
    // 2. 计数到指定值后翻转LED
    timer_count++;
    if(timer_count >= (TIMER_FREQ / BLINK_FREQ))
    {
        timer_count = 0;                      // 重置计数器
        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;// 翻转LED电平
    }
}
void main(void)
{
    // 1. 系统初始化
    InitSysCtrl();          // 关闭看门狗+配置系统时钟（60MHz）+初始化外设时钟
    DINT;                   // 关闭全局中断
    InitPieCtrl();          // 初始化PIE控制器
    IER = 0x0000;           // 清除CPU中断使能寄存器
    IFR = 0x0000;           // 清除CPU中断标志寄存器
    InitPieVectTable();     // 初始化PIE中断向量表
    
    // 2. 外设初始化
    LED_Init();             // LED GPIO初始化
    Timer0_Init();          // 定时器0初始化（含中断配置）
    
    // 3. 主循环
    while(1)
    {
    }
}


