#ifndef __LCD_ILI9341_H
#define __LCD_ILI9341_H



#include "DSP28x_Project.h"

#define WHITE   0xFFFF
#define BLACK   0x0000
#define RED     0xF800
#define BLUE    0x001F

#define LCD_W 240
#define LCD_H 320

// 模拟 SPI 引脚
#define SDI(x)   (x? (GpioDataRegs.GPASET.bit.GPIO16=1) : (GpioDataRegs.GPACLEAR.bit.GPIO16=1))
#define SCK(x)   (x? (GpioDataRegs.GPASET.bit.GPIO17=1) : (GpioDataRegs.GPACLEAR.bit.GPIO17=1))
#define CS(x)    (x? (GpioDataRegs.GPASET.bit.GPIO18=1) : (GpioDataRegs.GPACLEAR.bit.GPIO18=1))
#define DC(x)    (x? (GpioDataRegs.GPASET.bit.GPIO19=1) : (GpioDataRegs.GPACLEAR.bit.GPIO19=1))
#define RST(x)   (x? (GpioDataRegs.GPASET.bit.GPIO20=1) : (GpioDataRegs.GPACLEAR.bit.GPIO20=1))

void LCD_Init(void);
void LCD_Clear(Uint16 color);
void LCD_ShowString(Uint16 x, Uint16 y, char *str, Uint16 color);
void LCD_ShowChinese(Uint16 x, Uint16 y, char *str, Uint16 color);

#endif