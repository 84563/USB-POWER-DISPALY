/*
 * menu.h
 *
 *  Created on: 2022年3月13日
 *      Author: Feng
 */

#ifndef INC_USER_MENU_H_
#define INC_USER_MENU_H_


extern void (*current_operation_index)();

typedef struct
{
     unsigned char current;
     unsigned char up;//上键
     unsigned char down;//下键
     unsigned char enter;//确认键
     void (*current_operation)();
} key_table;

void fun0(void);
void fun1(void);
void fun2(void);
void fun3(void);
void fun4(void);
void fun5(void);
void fun6(void);
void fun7(void);
void fun8(void);
void fun9(void);
void fun10(void);
void fun11(void);



extern key_table table[30];

void Clear_mem(void);
void DrawPct(u8 x,u8 y,u8 len);
void CLear0(u8 x,u8 y,u16 num);

#endif /* INC_USER_MENU_H_ */
