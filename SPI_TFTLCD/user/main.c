//###########################################################################
#include "DSP28x_Project.h"
#include "HK_all_include.h"

void InitLED(void);
void InitKEY(void);
void InitBeep(void);
void scanKey(void);

// 必须加这个，否则编译报错！
Uint16 Beepcnt = 0;

void main(void) 
{
    InitSysCtrl();
    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    InitKEY();
    InitLED();
    //InitBeep();

    //BEEPOFF();
    D400ON();
    D401ON();
    D402LOW();
    Timer0_init();

    SCI_Init();
    open_uart_debug();

    softResetIIC_BUS();
    InitI2C_Gpio();
    I2CA_Init();

    SPI_IOinit();
    spi_fifo_init();
    spi_init();

    LCD_BK_High();
    HD_reset();
    Initial_ILI9340C();

    // 纯红色背景
    LCD_TEST_SingleColor(Red);
    LCD_SetBackColor(Red);
    LCD_SetTextColor(White);

    // ===================== 显示三组姓名 + 学号 =====================
    // 第一组：林楷晋  23124061059
    showHanZi(30, 60, &Hanzi3[6][0], 1);    // 林
    showHanZi(30+16, 60, &Hanzi3[7][0], 1); // 楷
    showHanZi(30+32, 60, &Hanzi3[8][0], 1); // 晋
    LCD_DisplayString(30+64, 60, "  23124061059");

    // 第二组：罗俊贤  23124061057
    showHanZi(30, 100, &Hanzi3[0][0], 1);    // 罗
    showHanZi(30+16, 100, &Hanzi3[1][0], 1); // 俊
    showHanZi(30+32, 100, &Hanzi3[2][0], 1); // 贤
    LCD_DisplayString(30+64, 100, "  23124061057");

    // 第三组：肖钰涛  23124061026
    showHanZi(30, 140, &Hanzi3[3][0], 1);    // 肖
    showHanZi(30+16, 140, &Hanzi3[4][0], 1); // 钰
    showHanZi(30+32, 140, &Hanzi3[5][0], 1); // 涛
    LCD_DisplayString(30+64, 140, "  23124061026");
    // ===============================================================

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    EINT;
    ERTM;

    while(1) 
    {
    }
}

void InitLED()
{
    EALLOW;
    D400MUX = 0; D400DIR = 1;
    D401MUX = 0; D401DIR = 1;
    D402MUX = 0; D402DIR = 1;
    RGB_R_MUX=0; RGB_R_DIR = 1;
    RGB_G_MUX=0; RGB_G_DIR = 1;
    RGB_B_MUX=0; RGB_B_DIR = 1;
    EDIS;
}

void InitKEY()
{
    EALLOW;
    S100MUX = 0;
    S100DIR = 0;
    S100PUD = 1;
    EDIS;
}

void InitBeep()
{
    BEEPOFF();
    EALLOW;
    BEEPMUX = 0;
    BEEPDIR = 1;
    EDIS;
}

void scanKey(void)
{
}