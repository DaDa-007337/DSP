//###########################################################################
//------------------SPI TFT 2.2寸液晶屏 测试-------------
//开发板硬件连接：（注意：跳线帽是左右短接方式）
//TFT液晶屏插入9P的2.54mm排母
//LCD接线： VCC接3.3V ； LED为背光，接GPIO7，与蜂鸣器共用一个IO口
//         做TFT实验时，需要把控制蜂鸣器的跳线帽取下
//SDO(MISO)不接
// LCD_CD     -> GPIO8
// LCD_CS     -> GPIO10
// LCD_CLK    -> GPIO18
// LCD_MOSI   -> GPIO16
// 在 SPI_TFT.h 文件中选择RST引脚：
// #define TFT_USE_GPIO_U2    //DSP28035蓝色标准版采用GPIO6  控制TFT 的RST
// #define TFT_USE_GPIO_U2A   //DSP28035绿色增强版采用GPIO9  控制的RST
//###########################################################################

#include "DSP28x_Project.h"     // DSP官方标准库头文件
#include "HK_all_include.h"     // 开发板驱动总头文件

// 外设初始化函数声明
void InitLED(void);     // LED初始化
void InitKEY(void);     // 按键初始化
void InitBeep(void);    // 蜂鸣器初始化
void scanKey(void);     // 按键扫描函数

short int Tmp = 0;             // 数码管计数变量
char keyReg = 0;               // 按键计数变量
Uint16 Beepcnt = 0;            // 蜂鸣器持续时间计数

unsigned char temp, i;
Uint16 Xpoint, Ypoint = 0;     // LCD画点坐标
Uint16 colurIndex = 0;         // 颜色索引
Uint16 LCD_cnt = 0;             // LCD显示数字计数

// 数字字符串数组，用于LCD显示0~9
char *shuzi[] = {"0","1","2","3","4","5","6","7","8","9"};

void main(void) 
{
    // Step 1. 系统控制初始化：时钟、看门狗、外设时钟使能
    InitSysCtrl();

    // Step 2. 关闭总中断，初始化PIE中断控制器
    DINT;                       // 关闭CPU全局中断
    InitPieCtrl();              // PIE控制寄存器默认初始化

    // 关闭CPU中断并清空中断标志
    IER = 0x0000;
    IFR = 0x0000;

    InitPieVectTable();         // 初始化PIE中断向量表

    // 基本外设初始化
    InitKEY();                  // 独立按键GPIO初始化
    InitLED();                  // LED指示灯GPIO初始化
    InitBeep();                 // 蜂鸣器GPIO初始化

    BEEPOFF();                  // 开机默认关闭蜂鸣器
    D400ON();                   // 点亮D400 LED
    D401ON();                   // 点亮D401 LED
    D402LOW();                  // D402 LED置低
    Timer0_init();              // 定时器0初始化（用于毫秒定时）

    SCI_Init();                 // 串口初始化
    open_uart_debug();          // 打开调试串口

    //--------------------- TM1650数码管驱动(IIC)初始化 -----------------------
    softResetIIC_BUS();         // 软件复位IIC总线
    InitI2C_Gpio();             // IIC对应GPIO初始化
    I2CA_Init();                // 硬件IIC初始化，100KHz

    // TM1650显示设置：亮度+显示开关
    LigntVal = 0x11;            // 亮度设置，BIT0=1开启显示
    TM1650_Send(CMD_SEG, LigntVal);

    // 数码管初始显示0
    Tmp = 0;
    TM1650_Send(DIG4, SEG7Table[Tmp/1000]);
    TM1650_Send(DIG3, SEG7Table[Tmp/100%10]);
    TM1650_Send(DIG2, SEG7Table[(Tmp/10)%10]);
    TM1650_Send(DIG1, SEG7Table[Tmp%10]);

    // 串口打印提示信息
    printf("\r\nM新动力DSP28035开发板 SPI LCD试验");

    //------------------------- SPI & LCD 硬件初始化 --------------------------
    SPI_IOinit();               // SPI对应GPIO配置
    spi_fifo_init();            // SPI FIFO初始化
    spi_init();                 // SPI通信参数初始化

    LCD_BK_High();              // 打开LCD背光

    HD_reset();                 // LCD硬件复位
    Initial_ILI9340C();         // ILI9340C驱动IC初始化

    // 全屏显示红色
    LCD_TEST_SingleColor(Red);
    LCD_SetBackColor(Red);      // 设置文字背景色
    LCD_SetTextColor(White);    // 设置文字前景色

    // LCD显示字符串
    LCD_DisplayString(0, 0, "TI TMS320F28035-->");

    // LCD显示汉字
    showHanZi(0, 48,  &Hanzi[0][0], 8);
    showHanZi(0, 80,  &Hanzi2[0][0], 11);
    showHanZi(0, 108, &Hanzi1[0][0], 15);
    showHanZi(0, 130, &Hanzi3[0][0], 7);

    // 动态画点起始Y坐标
    Ypoint = 180;

    // 使能PIE & 全局中断
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    EINT;                       // 使能全局中断INTM
    ERTM;                       // 使能实时中断

    //======================== 主循环 ==============================
    while(1) 
    {
        // 1ms标志位，定时器0中断设置
        if(timer0Base.Mark_Para.Status_Bits.OnemsdFlag == 1)
        {
            timer0Base.Mark_Para.Status_Bits.OnemsdFlag = 0;

            // 每200ms执行一次
            if(timer0Base.msCounter >= 200)
            {
                timer0Base.msCounter = 0;
                D401TOGGLE();               // LED状态翻转

                // LCD显示数字0~9循环
                LCD_cnt = LCD_cnt == 9 ? 0 : LCD_cnt + 1;
                LCD_DisplayString(160, 0, shuzi[LCD_cnt]);

                Tmp += 1;                   // 数码管计数+1
                if(Tmp > 999) Tmp = 0;

                // 更新TM1650数码管显示
                TM1650_Send(DIG3, SEG7Table[Tmp/100]);
                TM1650_Send(DIG2, SEG7Table[(Tmp/10)%10]);
                TM1650_Send(DIG1, SEG7Table[Tmp%10]);

                scanKey();                  // 扫描按键
            }
        }
        // 逐点扫描画点，实现渐变彩色条
        if(Xpoint == 0)
        {
            Ypoint++;
            if(Ypoint >= 320)              // 到屏幕底部后重置Y并换色
            {
                Ypoint = 180;
                // 颜色索引0~9循环
                colurIndex = colurIndex == 9 ? 0 : colurIndex + 1;
            }
        }
        // X坐标自增，到240后重置为0
        Xpoint = Xpoint >= 240 ? 0 : Xpoint + 1;

        // 在当前坐标画一个点，使用当前颜色
        LCD_draw_Point(Xpoint, Ypoint, colurBuf[colurIndex]);
    }
}
//#################################################
// 函数名：InitLED
// 功能：初始化开发板上LED对应的GPIO为输出模式
//#################################################
void InitLED()
{
    EALLOW;                       // 开启受保护寄存器写使能
    // 普通LED配置为GPIO功能+输出模式
    D400MUX = 0;
    D400DIR = 1;
    D401MUX = 0;
    D401DIR = 1;
    D402MUX = 0;
    D402DIR = 1;
    // RGB灯珠GPIO配置
    RGB_R_MUX = 0;
    RGB_R_DIR = 1;
    RGB_G_MUX = 0;
    RGB_G_DIR = 1;
    RGB_B_MUX = 0;
    RGB_B_DIR = 1;
    EDIS;                         // 关闭受保护寄存器写使能
}
//#################################################
// 函数名：InitKEY
// 功能：初始化独立按键S100对应GPIO为输入模式
//#################################################
void InitKEY()
{
    EALLOW;
    S100MUX = 0;                  // 设置为GPIO功能
    S100DIR = 0;                  // 设置为输入模式
    S100PUD = 1;                  // 关闭内部上拉（外部已有上拉）
    EDIS;
}
//#################################################
// 函数名：InitBeep
// 功能：初始化蜂鸣器GPIO为输出模式，默认关闭
//#################################################
void InitBeep()
{
    BEEPOFF();                    // 默认关闭蜂鸣器
    EALLOW;

    BEEPMUX = 0;                  // 设置为GPIO功能
    BEEPDIR = 1;                  // 设置为输出模式

    EDIS;
}
/************************************************************************
 * 函数名：scanKey
 * 功能：扫描S100~S104按键，执行对应动作：
 *       S100  清零计数+长蜂鸣
 *       S101  数字+1
 *       S102  数字-1
 *       S103  清零
 *       S104  数码管亮度+1
 ***********************************************************************/
void scanKey(void)
{
    // S100 按键按下检测（低电平有效）
    if(rS100DAT() == 0)
    {
        Tmp = 0;                  // 清零计数
        Beepcnt = 300;            // 蜂鸣器响300ms
        printf("\r\nS100");
    }
    // 读取TM1650按键码（S101~S104）
    TM1650_Read(CMD_KEY, &keyVal);
    // S101 按键处理
    if(keyVal == 0x44)
    {
        keyReg++;
        keyReg &= 0x0f;
        TM1650_Send(DIG4, SEG7Table[keyReg]);
        Beepcnt = 100;
        printf("\r\nS101");
    }
    // S102 按键处理
    if(keyVal == 0x4c)
    {
        keyReg--;
        keyReg &= 0x0f;
        TM1650_Send(DIG4, SEG7Table[keyReg]);
        Beepcnt = 100;
        printf("\r\nS102");
    }
    // S103 按键处理
    if(keyVal == 0x54)
    {
        keyReg = 0;
        TM1650_Send(DIG4, SEG7Table[keyReg]);
        printf("\r\nS103");
        Beepcnt = 100;
    }
    // S104 按键处理：增加数码管亮度
    if(keyVal == 0x5C)
    {
        LigntVal = (LigntVal + 0x10) & 0x7F;
        TM1650_Send(CMD_SEG, LigntVal);
        printf("\r\nS104");
        Beepcnt = 100;
    }
}
