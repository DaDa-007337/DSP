/*
 * SPI_Flash.h
 *
 *  Created on: 2013-12-18
 *      Author: cotto
 */

#ifndef SPI_TFT_H_
#define SPI_TFT_H_

#define TFT_SPIA

//-----------------根据开发板，选择打开一个参与编译-----------------------
//#define TFT_USE_GPIO_U2     //蓝色标准版采用GPIO6  控制TFT 的RST
#define TFT_USE_GPIO_U2A     //绿色增强版采用GPIO9  控制的RST

#define DUMMYDATA 0xa5


//EEPROM 的CS引脚
#define EECSGPIO GPIO39
#define EECSMUX GpioCtrlRegs.GPBMUX1.bit.EECSGPIO
#define EECSDIR GpioCtrlRegs.GPBDIR.bit.EECSGPIO
#define CE_High()  GpioDataRegs.GPBSET.bit.EECSGPIO =1
#define CE_Low() GpioDataRegs.GPBCLEAR.bit.EECSGPIO =1

#define F032B_SO_READ() GpioDataRegs.GPADAT.bit.GPIO17

//颜色定义


#define White          0xFFFF
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

extern Uint16 colurBuf[];
extern uint16_t s_TextColor ;
extern uint16_t s_BackColor ;


//液晶D/CX引脚，数据/命令控制，接单片机IO引脚
#define LCD_CDGPIO GPIO8
#define LCD_CDMUX GpioCtrlRegs.GPAMUX1.bit.LCD_CDGPIO
#define LCD_CDDIR GpioCtrlRegs.GPADIR.bit.LCD_CDGPIO
#define LCD_CD_High()  GpioDataRegs.GPASET.bit.LCD_CDGPIO =1
#define LCD_CD_Low() GpioDataRegs.GPACLEAR.bit.LCD_CDGPIO =1

//液晶片选CS,
#define LCD_CSGPIO GPIO10
#define LCD_CSMUX GpioCtrlRegs.GPAMUX1.bit.LCD_CSGPIO
#define LCD_CSDIR GpioCtrlRegs.GPADIR.bit.LCD_CSGPIO
#define LCD_CS_High()  GpioDataRegs.GPASET.bit.LCD_CSGPIO =1
#define LCD_CS_Low() GpioDataRegs.GPACLEAR.bit.LCD_CSGPIO =1

//TFT 背光引脚 输出控制PNP三级管点亮TFT背光，低电平点亮
#define LCD_BKGPIO GPIO7
#define LCD_BKMUX GpioCtrlRegs.GPAMUX1.bit.LCD_BKGPIO
#define LCD_BKDIR GpioCtrlRegs.GPADIR.bit.LCD_BKGPIO
#define LCD_BK_High()  GpioDataRegs.GPACLEAR.bit.LCD_BKGPIO =1
#define LCD_BK_Low() GpioDataRegs.GPASET.bit.LCD_BKGPIO =1


//U2蓝色由GPIO6控制RST
#ifdef TFT_USE_GPIO_U2
//液晶RESET引脚
#define LCD_RSTGPIO GPIO6
#define LCD_RSTMUX GpioCtrlRegs.GPAMUX1.bit.LCD_RSTGPIO
#define LCD_RSTDIR GpioCtrlRegs.GPADIR.bit.LCD_RSTGPIO
#define LCD_RST_High()  GpioDataRegs.GPASET.bit.LCD_RSTGPIO =1
#define LCD_RST_Low() GpioDataRegs.GPACLEAR.bit.LCD_RSTGPIO =1

#else  // U2A 绿色版本由GPIO9控制RST
//液晶RESET引脚
#define LCD_RSTGPIO GPIO9
#define LCD_RSTMUX GpioCtrlRegs.GPAMUX1.bit.LCD_RSTGPIO
#define LCD_RSTDIR GpioCtrlRegs.GPADIR.bit.LCD_RSTGPIO
#define LCD_RST_High()  GpioDataRegs.GPASET.bit.LCD_RSTGPIO =1
#define LCD_RST_Low() GpioDataRegs.GPACLEAR.bit.LCD_RSTGPIO =1
#endif
//#define LCD_RSTGPIO AIO10
//#define LCD_RSTMUX GpioCtrlRegs.AIOMUX1.bit.LCD_RSTGPIO
//#define LCD_RSTDIR GpioCtrlRegs.AIODIR.bit.LCD_RSTGPIO
//#define LCD_RST_High()  GpioDataRegs.AIOSET.bit.LCD_RSTGPIO =1
//#define LCD_RST_Low() GpioDataRegs.AIOCLEAR.bit.LCD_RSTGPIO =1
//
//#define LCD_CDGPIO AIO4
//#define LCD_CDMUX GpioCtrlRegs.AIOMUX1.bit.LCD_CDGPIO
//#define LCD_CDDIR GpioCtrlRegs.AIODIR.bit.LCD_CDGPIO
//#define LCD_CD_High()  GpioDataRegs.AIOSET.bit.LCD_CDGPIO  =1
//#define LCD_CD_Low() GpioDataRegs.AIOCLEAR.bit.LCD_CDGPIO =1
//
//#define LCD_CSGPIO AIO6
//#define LCD_CSMUX GpioCtrlRegs.AIOMUX1.bit.LCD_CSGPIO
//#define LCD_CSDIR GpioCtrlRegs.AIODIR.bit.LCD_CSGPIO
//#define LCD_CS_High()  GpioDataRegs.AIOSET.bit.LCD_CSGPIO =1
//#define LCD_CS_Low() GpioDataRegs.AIOCLEAR.bit.LCD_CSGPIO =1



void SPI_IOinit(void);
void spi_fifo_init(void);
void spi_init(void);

uint8_t Send_Byte(uint16_t a);
uint16_t Get_Byte(void);

void Delay_us(int value);
void Delay_ms(int value);////延时函数_ms
void HD_reset(void);
void LCD_Writ_Bus(char a);
void LCD_WRITE_CMD(char cmd);
void LCD_WRITE_COM_DATA(char com_data);
void LCD_WRITE_DATA(int a);
void Address_set(unsigned int x1,unsigned int x2,unsigned int y1,unsigned int y2);
void LCD_TEST_SingleColor(int Discolor);
void LCD_draw_Line(unsigned int y,unsigned int color);
void Initial_ILI9340C(void);
void LCD_Sleep_ILI9340C(void);
void LCD_ExitSleep_ILI9340C(void);


void LCD_draw_Point(unsigned int x,unsigned int y,unsigned int color);
void showHanZi(uint16_t Xpos, uint16_t Ypos,  char const *ptr,char Len);


/*----- High layer function -----*/
void LCD_Init(void);
void LCD_SetTextColor(uint16_t Color);
void LCD_SetBackColor(uint16_t Color);
void LCD_ClearLine(uint8_t Line);
void LCD_Clear(uint16_t Color);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const char *c, uint8_t width);
void LCD_DisplayString(uint16_t Xpos, uint16_t Ypos, char *ptr);
void LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width);
void LCD_WindowModeDisable(void);
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction);
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void LCD_DrawMonoPict(const uint32_t *Pict);
void LCD_WriteBMP(const uint16_t *ptr);
void LCD_SetTransparent(uint8_t _mode);






#endif /* SPI_FLASH_H_ */
