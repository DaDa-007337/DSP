/****************************************************************************
* Copyright (C), 2009-2010, www.armfly.com
*
* 文件名: ASC16.h
* 内容简述: ASCII字符点阵头文件
*
* 文件历史:
* 版本号  日期       作者    说明
* v0.1    2010-01-04 armfly  创建该文件
*
*/

#ifndef __ASCII16_H_
#define __ASCII16_H_

extern char const Ascii16[];
extern char const Hanzi[][32];
extern char const Hanzi1[][32];
extern char const Hanzi2[][32];
extern char const Hanzi3[][32];

#define HZK16_ADDR 0x803E800		/* 汉字库地址 */

#endif
