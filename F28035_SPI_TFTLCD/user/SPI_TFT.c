/*
 * SPI_Flash.c
 *
 *  Created on: 2013-12-18
 *      Author: cotto
 */
#include "HK_all_include.h"

#ifdef TFT_SPIA
volatile struct SPI_REGS *PtrSPI= &SpiaRegs;
#else
volatile struct SPI_REGS *PtrSPI= &SpibRegs;
#endif

uint16_t s_TextColor = 0x0000;
uint16_t s_BackColor = 0xFFFF;

Uint16 colurBuf[]={White,Black,Blue,Blue2,Red,Magenta,Green,Cyan,Yellow};

//函数
void Delay_us(int value)////延时函数_us
{
	Uint16 i;
	while(value--)
		for(i=0;i<5;i++);
}

void Delay_ms(int value)////延时函数_ms
{
	while (value--)
		Delay_us(999);


}


void SPI_IOinit()
{
EALLOW;
	EECSMUX = 0;
	EECSDIR = 1;

	LCD_BKMUX = 0;
	LCD_BKDIR = 1;

	LCD_RSTMUX = 0;
	LCD_RSTDIR = 1;

	LCD_CDMUX = 0;
	LCD_CDDIR = 1;

	LCD_CSMUX = 0;
	LCD_CSDIR = 1;


#ifdef TFT_SPIA
	GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;   // Enable pull-up on GPIO16 (SPISIMOA)
	GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;   // Enable pull-up on GPIO17 (SPISOMIA)
	GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;   // Enable pull-up on GPIO18 (SPICLKA)
//	GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;   // Enable pull-up on GPIO19 (SPISTEA)

	GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input GPIO16 (SPISIMOA)
	GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
	GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // Asynch input GPIO18 (SPICLKA)
//	GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)

	GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1; // Configure GPIO16 as SPISIMOA
	GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1; // Configure GPIO17 as SPISOMIA
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1; // Configure GPIO18 as SPICLKA
//	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 1; // Configure GPIO19 as SPISTEA

#else
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;     // Enable pull-up on GPIO12 (SPISIMOB)
    GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;     // Enable pull-up on GPIO13 (SPISOMIB)
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;     // Enable pull-up on GPIO14 (SPICLKB)
//    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;     // Enable pull-up on GPIO15 (SPISTEB)

    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 3;   // Asynch input GPIO12 (SPISIMOB)
    GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 3;   // Asynch input GPIO13 (SPISOMIB)
    GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 3;   // Asynch input GPIO14 (SPICLKB)
//    GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;   // Asynch input GPIO15 (SPISTEB)

    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 3;    // Configure GPIO12 as SPISIMOB
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 3;    // Configure GPIO13 as SPISOMIB
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 3;    // Configure GPIO14 as SPICLKB
//    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 3;    // Configure GPIO15 as SPISTEB
#endif
    CE_High();//若该SPI与TFT公用总线，将FLASH芯片的CS置高，否则FLASH输出会影响总线
EDIS;
}

//CLOCK PHASE = 0:
//CLOCK POLARITY  = 0

void spi_init()
{
	CE_High();//FLASH芯片公用总线，将其使能关闭

	PtrSPI->SPICCR.bit.SPISWRESET = 0; //P25  SW Reset on
	PtrSPI->SPICCR.bit.SPICHAR = 7;    //p26   8bit
	PtrSPI->SPICCR.bit.CLKPOLARITY = 0 ; //P25 When no SPI data is sent, SPICLK is at low
	PtrSPI->SPICTL.bit.CLK_PHASE = 1;   //P25   Data is output on the rising edge of the SPICLK signal;
	                                     //      input data is latched on the falling edge of the SPICLK signal.
	PtrSPI->SPICTL.bit.MASTER_SLAVE  =1;//P26   SPI configured as a master.

	PtrSPI->SPIBRR =3;             // For SPIBRR = 3 to 127;  SPI Baud Rate = LSPCLK/(SPIBRR+1)

	PtrSPI->SPICCR.bit.SPISWRESET = 1; //P25  SW Reset off

	PtrSPI->SPIPRI.bit.FREE = 1;   // Set so breakpoints don't disturb xmission
	PtrSPI->SPIPRI.bit.STEINV = 0; //P34 SPISTE is active low (normal)
}

uint8_t Send_Byte(uint16_t a)
{
	Uint16 rdata;

	PtrSPI->SPICTL.bit.TALK  =1;                //P23 Enable Transmit path
	PtrSPI->SPITXBUF=(a<<8)&0xff00;
    while(PtrSPI->SPIFFRX.bit.RXFFST >=4 ) { } 	// Wait until data is received
//	while(SpiaRegs.SPISTS.bit.INT_FLAG == 0) {} // Waits until data rx’d
    rdata = PtrSPI->SPIRXBUF;
    return rdata;
}

uint16_t Get_Byte(void)
{
	uint8_t rdata;
	PtrSPI->SPICTL.bit.TALK = 0; // Disable Transmit pat
	PtrSPI->SPITXBUF = DUMMYDATA; // Send dummy to start
	// NOTE: because TALK = 0, data does not tx onto SPISIMOA pin
	//while(SpiaRegs.SPISTS.bit.INT_FLAG !=1) {} // Wait until data rece
	while(PtrSPI->SPIFFRX.bit.RXFFST ==0) { } 	// Wait until data is received
	rdata = PtrSPI->SPIRXBUF; // Master reads data
	return  rdata;
}

void spi_fifo_init()
{
// Initialize SPI FIFO registers
//    SpiaRegs.SPIFFTX.all=0xE040;
	PtrSPI->SPIFFTX.bit.SPIRST =1;  //P31 SPI FIFO can resume transmit or receive.
	PtrSPI->SPIFFTX.bit.SPIFFENA =1;//SPI FIFO enhancements are enabled
	PtrSPI->SPIFFTX.bit.TXFFST = 1; //Re-enable Transmit FIFO operation
	PtrSPI->SPIFFTX.bit.TXFFINTCLR = 1;//Write 1 to clear TXFFINT flag in bit 7.
	PtrSPI->SPIFFTX.bit.TXFFIENA = 0; //不用中断

    //SpiaRegs.SPIFFRX.all=0x2044;
     PtrSPI->SPIFFRX.bit.RXFFOVFCLR = 1;
	 PtrSPI->SPIFFRX.bit.RXFFINTCLR  =1;
	 PtrSPI->SPIFFRX.bit.RXFFST = 1;
	 PtrSPI->SPIFFRX.bit.RXFFIENA = 0; //不用FIFO接收中断
	 PtrSPI->SPIFFRX.bit.RXFFIL = 4;

	 PtrSPI->SPIFFCT.all=0x0;  //P32
}


void HD_reset()
{
	//LCD Reset
	LCD_RST_High();
	Delay_ms(1);
	LCD_RST_Low();
	Delay_ms(10);
	LCD_RST_High();
	Delay_ms(20);
}


void LCD_Writ_Bus(char a)
{   //数据写入函数 8位
	  Uint16 d;
	  d = a;
	  LCD_CS_Low();
	  Send_Byte(d);
//	  LCD_CS_High();//由于采用FIFO模式提高刷屏速度，故不可在这里将CS置高，可以在发送完成中断里将CS置高电平
}



void LCD_WRITE_CMD(char cmd)
{//8 bit
	LCD_CD_Low();
	LCD_Writ_Bus(cmd);
}



void LCD_WRITE_COM_DATA(char com_data)
{//8 bit
	LCD_CD_High();
	LCD_Writ_Bus(com_data);
}


void LCD_WRITE_DATA(int a){//16位数据,分两次，每次送8位
	LCD_CD_High();
	LCD_Writ_Bus(a>>8);
	LCD_Writ_Bus(a);
}


//液晶主控初始化
void Initial_ILI9340C(void)
{
        LCD_WRITE_CMD(0xCB);
        LCD_WRITE_COM_DATA(0x39);
        LCD_WRITE_COM_DATA(0x2C);
        LCD_WRITE_COM_DATA(0x00);
        LCD_WRITE_COM_DATA(0x34);
        LCD_WRITE_COM_DATA(0x02);

        LCD_WRITE_CMD(0xCF);
        LCD_WRITE_COM_DATA(0x00);
        LCD_WRITE_COM_DATA(0XC1);
        LCD_WRITE_COM_DATA(0X30);

        LCD_WRITE_CMD(0xE8);
        LCD_WRITE_COM_DATA(0x85);
        LCD_WRITE_COM_DATA(0x00);
        LCD_WRITE_COM_DATA(0x78);

        LCD_WRITE_CMD(0xEA);
        LCD_WRITE_COM_DATA(0x00);
        LCD_WRITE_COM_DATA(0x00);

        LCD_WRITE_CMD(0xED);
        LCD_WRITE_COM_DATA(0x64);
        LCD_WRITE_COM_DATA(0x03);
        LCD_WRITE_COM_DATA(0X12);
        LCD_WRITE_COM_DATA(0X81);

        LCD_WRITE_CMD(0xF7);
        LCD_WRITE_COM_DATA(0x20);

        LCD_WRITE_CMD(0xC0);    //Power control
        LCD_WRITE_COM_DATA(0x23);   //VRH[5:0]

        LCD_WRITE_CMD(0xC1);    //Power control
        LCD_WRITE_COM_DATA(0x10);   //SAP[2:0];BT[3:0]

        LCD_WRITE_CMD(0xC5);    //VCM control
        LCD_WRITE_COM_DATA(0x3e); //对比度调节
        LCD_WRITE_COM_DATA(0x28);

        LCD_WRITE_CMD(0xC7);    //VCM control2
        LCD_WRITE_COM_DATA(0x86);  //--

        LCD_WRITE_CMD(0x36);    // Memory Access Control
        LCD_WRITE_COM_DATA(0x88);//88 C8 48 68竖屏//28 E8 横屏  Address_set(0,239,0,319);

        LCD_WRITE_CMD(0x3A);
        LCD_WRITE_COM_DATA(0x55);

        LCD_WRITE_CMD(0xB1);
        LCD_WRITE_COM_DATA(0x00);
        LCD_WRITE_COM_DATA(0x18);

        LCD_WRITE_CMD(0xB6);    // Display Function Control
        LCD_WRITE_COM_DATA(0x08);
        LCD_WRITE_COM_DATA(0x82);
        LCD_WRITE_COM_DATA(0x27);

        LCD_WRITE_CMD(0xF2);    // 3Gamma Function Disable
        LCD_WRITE_COM_DATA(0x00);

        LCD_WRITE_CMD(0x26);    //Gamma curve selected
        LCD_WRITE_COM_DATA(0x01);

        LCD_WRITE_CMD(0xE0);    //Set Gamma
        LCD_WRITE_COM_DATA(0x0F);
        LCD_WRITE_COM_DATA(0x31);
        LCD_WRITE_COM_DATA(0x2B);
        LCD_WRITE_COM_DATA(0x0C);
        LCD_WRITE_COM_DATA(0x0E);
        LCD_WRITE_COM_DATA(0x08);
        LCD_WRITE_COM_DATA(0x4E);
        LCD_WRITE_COM_DATA(0xF1);
        LCD_WRITE_COM_DATA(0x37);
        LCD_WRITE_COM_DATA(0x07);
        LCD_WRITE_COM_DATA(0x10);
        LCD_WRITE_COM_DATA(0x03);
        LCD_WRITE_COM_DATA(0x0E);
        LCD_WRITE_COM_DATA(0x09);
        LCD_WRITE_COM_DATA(0x00);

        LCD_WRITE_CMD(0XE1);    //Set Gamma
        LCD_WRITE_COM_DATA(0x00);
        LCD_WRITE_COM_DATA(0x0E);
        LCD_WRITE_COM_DATA(0x14);
        LCD_WRITE_COM_DATA(0x03);
        LCD_WRITE_COM_DATA(0x11);
        LCD_WRITE_COM_DATA(0x07);
        LCD_WRITE_COM_DATA(0x31);
        LCD_WRITE_COM_DATA(0xC1);
        LCD_WRITE_COM_DATA(0x48);
        LCD_WRITE_COM_DATA(0x08);
        LCD_WRITE_COM_DATA(0x0F);
        LCD_WRITE_COM_DATA(0x0C);
        LCD_WRITE_COM_DATA(0x31);
        LCD_WRITE_COM_DATA(0x36);
        LCD_WRITE_COM_DATA(0x0F);

        LCD_WRITE_CMD(0x11);    //Exit Sleep
        Delay_ms(120);

        LCD_WRITE_CMD(0x29);    //Display on
        LCD_WRITE_CMD(0x2c);

}

//LCD进入休眠状态
void LCD_Sleep_ILI9340C(void)
{
  LCD_WRITE_CMD(0x28);//Display off
  Delay_ms(20);
  LCD_WRITE_CMD(0x10);//Enter Sleep mode
}
//LCD退出休眠状态
void LCD_ExitSleep_ILI9340C(void)
{
  LCD_WRITE_CMD(0x11);//Exit Sleep
  Delay_ms(120);
  LCD_WRITE_CMD(0x29);//Display on
  LCD_WRITE_CMD(0x2c);
}

void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
 	LCD_WRITE_CMD(0x2a); 	//x轴
 	LCD_WRITE_DATA(Xpos);
 	LCD_WRITE_DATA(Xpos);
    LCD_WRITE_CMD(0x2b); 	//y轴
    LCD_WRITE_DATA(Ypos);
 	LCD_WRITE_DATA(Ypos);
 	LCD_WRITE_CMD(0x2c);
}


/*******************************************************************************
*	函数名: LCD_WriteRAM
*	参  数: RGB_Code : 颜色代码
*	返  回: 无
*	功  能: 写显存，显存地址自动增加
*/
void LCD_WriteRAM(uint16_t RGB_Code)
{
// 	LCD_WRITE_CMD(0x2c);
 	LCD_WRITE_DATA(RGB_Code);
}


/*******************************************************************************
*	函数名: LCD_SetTextColor
*	参  数: Color : 文本颜色
*	返  回: 无
*	功  能: 设置文本颜色，保存在全部变量s_TextColor
*/
void LCD_SetTextColor(uint16_t Color)//__IO
{
	s_TextColor = Color;
}

/*******************************************************************************
*	函数名: LCD_SetBackColor
*	参  数: Color : 背景颜色
*	返  回: 无
*	功  能: 设置背景颜色，保存在全部变量 s_BackColor
*/
void LCD_SetBackColor(uint16_t Color)//__IO
{
	s_BackColor = Color;
}


//设置绘点窗口，x=0~239,y=0~319
void Address_set(unsigned int x1,unsigned int x2,unsigned int y1,unsigned int y2)
{
     	LCD_WRITE_CMD(0x2a); 	//x轴
     	LCD_WRITE_DATA(x1);
     	LCD_WRITE_DATA(x2);
	    LCD_WRITE_CMD(0x2b); 	//y轴
	    LCD_WRITE_DATA(y1);
     	LCD_WRITE_DATA(y2);
     	LCD_WRITE_CMD(0x2c);
}


void LCD_TEST_SingleColor(int Discolor){
     	int i,j;
		Address_set(0,239,0,319);

		for (i=0;i<320;i++){
			for (j=0;j<240;j++){
				LCD_WRITE_DATA(Discolor);
				}
		}
}


//画一个点
void LCD_draw_Point(unsigned int x,unsigned int y,unsigned int color)
{
//	Address_set(x,x,y,y);
 	LCD_WRITE_CMD(0x2a); 	//x轴
    LCD_WRITE_DATA(x);
 	LCD_WRITE_DATA(x);
    LCD_WRITE_CMD(0x2b); 	//y轴
    LCD_WRITE_DATA(y);
 	LCD_WRITE_DATA(y);
 	LCD_WRITE_CMD(0x2c);
	LCD_WRITE_DATA(color);
}

//画一条横线
void LCD_draw_Line(unsigned int y,unsigned int color)
{		int i;
		Address_set(0,319,y,y);
		for (i=0;i<319;i++){
			LCD_WRITE_DATA(color);
			}
}

/*******************************************************************************
*	函数名: LCD_DrawChar
*	参  数:
*		Xpos : X坐标;
*		Ypos: Y坐标；
*		c : 指向字符点阵的指针
*		width : 点阵的宽度，字符是8，汉字是16
*	返  回: 无
*	功  能: 在LCD上显示一个字符(16x24)
*/
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const char *c, uint8_t width)
{
	uint32_t index = 0, i = 0;
	uint8_t Yaddress;

	Yaddress = Ypos;

//	LCD_SetCursor(Xpos, Ypos);
	Address_set(Xpos,Xpos+width-1,Ypos,Ypos+15);


//	if (s_Transparent == 0)
	{
		for (index = 0; index < 16; index++)	/* 字符高度 */
		{
//			LCD_WriteRAM_Prepare();
			for (i = 0; i < width; i++)	/* 字符宽度 */
			{
				if ((c[index * (width / 8) + i / 8] & (0x80 >> (i % 8))) == 0x00)
				{
					LCD_WriteRAM(s_BackColor);
				}
				else
				{
					LCD_WriteRAM(s_TextColor);
				}
			}
			Yaddress++;
//			LCD_SetCursor(Xpos, Yaddress);
		}
	}
//	else	/* armfly 添加：实现文字叠加在图片上的功能 */
//	{
//		for (index = 0; index < 16; index++)	/* 字符高度 */
//		{
//			uint16_t x = Xpos;
//
//			for (i = 0; i < width; i++)	/* 字符宽度 */
//			{
//				if ((c[index * (width / 8) + i / 8] & (0x80 >> (i % 8))) != 0x00)
//				{
//					LCD_SetCursor(x, Yaddress);
//					LCD_WriteReg(0x202,s_TextColor);
//				}
//				x++;
//			}
//			Yaddress++;
//		}
//	}
}



/*******************************************************************************
*	函数名: LCD_DisplayString
*	参  数:
*		Xpos : X坐标 0 - 399
*		Ypos : Y坐标 0 - 239
*		ptr  : 字符串指针；
*	返  回: 无
*	功  能: 在LCD指定文本行显示一个字符串
*/
void LCD_DisplayString(uint16_t Xpos, uint16_t Ypos, char *ptr)
{
	uint8_t i = 0;
	uint8_t code1;
	uint8_t code2;
	uint16_t CanDisLen = Xpos;
	uint32_t address;


	while ((*ptr != 0) && (i < (50-(CanDisLen>>3))))
	{
		code1 = *ptr;	/* ascii代码 或者汉字代码的高字节 */
		if (code1 < 0x80)
		{
			LCD_DrawChar(Xpos, Ypos, &Ascii16[code1 * 16], 8);	/* 16 表示1个字符字模的字节数 */
			Xpos += 8;		/* 列地址+8 */
		}
		else	/* 汉字内码 */
		{
			code2 = *++ptr;
			if (code2 == 0)
			{
				break;
			}
			/* 计算16点阵汉字点阵地址
				ADDRESS = [(code1-0xa1) * 94 + (code2-0xa1)] * 32
				;
			*/
//			address = ((code1-0xa1) * 94 + (code2-0xa1)) * 32 + HZK16_ADDR;
//			address = Hanzi[0][0];

			LCD_DrawChar(Xpos, Ypos,  &Hanzi[0][0], 16);
			Xpos += 16;		/* 列地址+16 */
			i++;

		}
		ptr++;			/* 指向下一个字符 */
		i++;
	}
}

//#################################################
//显示中文函数
//取模方式：阳码逐行顺向
//取模，混合含有西文或者字符的话，需要全角输入，这样做西文或者字符为32byte取模对齐
// *ptr 为数组的首地址
// Len为显示内容的个数
//-----------------------------------------------
void showHanZi(uint16_t Xpos, uint16_t Ypos, char const  *ptr,char Len)
{
	uint16_t  i = 0;
	for(i=0;i<Len;i++)
	{
		LCD_DrawChar(Xpos, Ypos,  ptr+i*32, 16);
		Xpos += 16;		/* 列地址+16 */
	}
}




