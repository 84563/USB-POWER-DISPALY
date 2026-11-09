/*
 * oled.h
 *
 *  Created on: Feb 19, 2022
 *      Author: Feng
 */

#ifndef INC_USER_OLED_H_
#define INC_USER_OLED_H_
#include "main.h"
#include "stdlib.h"

//设备地址
#define OLED_ADDRESS 0x78

//数据类型
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//OLED控制用函数
void OLED_Refresh(void);
void OLED_WR_Byte(unsigned dat,unsigned cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Init_096(void);
void OLED_Clear(void);
void OLED_On(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
void OLED_ShowString_H(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
void OLED_ShowString_V(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
u32 OLED_Pow(u8 m,u8 n);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowDotNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);

void fill_picture(unsigned char fill_Data);
void Picture();

void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);


#endif /* INC_USER_OLED_H_ */
