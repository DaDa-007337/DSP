/*
 * HK_include_all.h
 *
 *  Created on: 2013-10-22
 *      Author: cotto
 */

#ifndef HK_INCLUDE_ALL_H_
#define HK_INCLUDE_ALL_H_

#define SYSCLK 60000000L

#include "HK28035Board.h"
#include "timer0_base.h"
#include "printf_uart.h"
#include "test485.h"
#include "TM1650_IIC.h"
#include "SPI_TFT.h"
#include "fonts.h"

//D400 低电平亮;（也是PWM7B的引脚）
#define D400GPIO GPIO41
#define D400MUX GpioCtrlRegs.GPBMUX1.bit.D400GPIO
#define D400DIR GpioCtrlRegs.GPBDIR.bit.D400GPIO
#define D400TOGGLE()  GpioDataRegs.GPBTOGGLE.bit.D400GPIO = 1
#define D400ON()  GpioDataRegs.GPBCLEAR.bit.D400GPIO =1
#define D400LOW() GpioDataRegs.GPBSET.bit.D400GPIO =1

//D401 低电平亮;
#define D401GPIO GPIO34
#define D401MUX GpioCtrlRegs.GPBMUX1.bit.D401GPIO
#define D401DIR GpioCtrlRegs.GPBDIR.bit.D401GPIO
#define D401TOGGLE()  GpioDataRegs.GPBTOGGLE.bit.D401GPIO = 1
#define D401ON()  GpioDataRegs.GPBCLEAR.bit.D401GPIO =1
#define D401LOW() GpioDataRegs.GPBSET.bit.D401GPIO =1

//D402 低电平亮;
#define D402GPIO GPIO43
#define D402MUX GpioCtrlRegs.GPBMUX1.bit.D402GPIO
#define D402DIR GpioCtrlRegs.GPBDIR.bit.D402GPIO
#define D402TOGGLE()  GpioDataRegs.GPBTOGGLE.bit.D402GPIO = 1
#define D402ON()  GpioDataRegs.GPBCLEAR.bit.D402GPIO =1
#define D402LOW() GpioDataRegs.GPBSET.bit.D402GPIO =1


//独立按键S100
#define S100GPIO GPIO27
#define S100MUX GpioCtrlRegs.GPAMUX2.bit.S100GPIO
#define S100DIR GpioCtrlRegs.GPADIR.bit.S100GPIO
#define S100PUD GpioCtrlRegs.GPAPUD.bit.S100GPIO
#define rS100DAT()  GpioDataRegs.GPADAT.bit.S100GPIO


//------------5050 RGB  高亮LED，高电平亮
//GPIO7=RED  GPIO9=GRE GPIO8=BLUE--------------------
#define D5050_R_GPIO GPIO7
#define RGB_R_MUX GpioCtrlRegs.GPAMUX1.bit.D5050_R_GPIO
#define RGB_R_DIR GpioCtrlRegs.GPADIR.bit.D5050_R_GPIO
#define RGB_R_TOGGLE()  GpioDataRegs.GPATOGGLE.bit.D5050_R_GPIO = 1
#define RGB_R_ON()  GpioDataRegs.GPASET.bit.D5050_R_GPIO =1
#define RGB_R_LOW() GpioDataRegs.GPACLEAR.bit.D5050_R_GPIO =1

//D5 5050 全彩LED的绿色，高电平亮;（也是PWM5B的引脚）
#define D5050_G_GPIO GPIO9
#define RGB_G_MUX GpioCtrlRegs.GPAMUX1.bit.D5050_G_GPIO
#define RGB_G_DIR GpioCtrlRegs.GPADIR.bit.D5050_G_GPIO
#define RGB_G_TOGGLE()  GpioDataRegs.GPATOGGLE.bit.D5050_G_GPIO = 1
#define RGB_G_ON()  GpioDataRegs.GPASET.bit.D5050_G_GPIO =1
#define RGB_G_LOW() GpioDataRegs.GPACLEAR.bit.D5050_G_GPIO =1

//D5 5050 全彩LED的蓝色，高电平亮;（也是PWM5A的引脚）
#define D5050_B_GPIO GPIO8
#define RGB_B_MUX GpioCtrlRegs.GPAMUX1.bit.D5050_B_GPIO
#define RGB_B_DIR GpioCtrlRegs.GPADIR.bit.D5050_B_GPIO
#define RGB_B_TOGGLE()  GpioDataRegs.GPATOGGLE.bit.D5050_B_GPIO = 1
#define RGB_B_ON()  GpioDataRegs.GPASET.bit.D5050_B_GPIO =1
#define RGB_B_LOW() GpioDataRegs.GPACLEAR.bit.D5050_B_GPIO =1

//蜂鸣器接口
#define BEEPGPIO GPIO6
#define BEEPMUX GpioCtrlRegs.GPAMUX1.bit.BEEPGPIO
#define BEEPDIR GpioCtrlRegs.GPADIR.bit.BEEPGPIO
#define BEEPPUD GpioCtrlRegs.GPAPUD.bit.BEEPGPIO
#define BEEPON()  GpioDataRegs.GPASET.bit.BEEPGPIO =1
#define BEEPOFF() GpioDataRegs.GPACLEAR.bit.BEEPGPIO =1
//----------------BEEP蜂鸣器--------------------------


#endif /* HK_INCLUDE_ALL_H_ */
