/*
 * ina220.h
 *
 *  Created on: 2022年2月22日
 *      Author: Feng
 */

#ifndef INC_USER_INA220_H_
#define INC_USER_INA220_H_

#include "main.h"
#include "stdlib.h"

#define 	CFG_REG	 		0x00		//配置寄存器
#define 	SV_REG 			0x01		//分流电压， 此处分流电阻为5mΩ
#define 	BV_REG 			0x02		//总线电压
#define 	PWR_REG 		0x03		//电源功率
#define 	CUR_REG 		0x04		//电流
#define 	CAL_REG 		0x05		//校准，设定满量程范围以及电流和功率测数的

#define   	INA220_ADDR 	0x80	//ina220在i2c总线上的地址

//数据类型
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

u16 INA220_GetVoltage(void);
u16 INA220_GetCurrent(void);
u16 INA220_GetPower(void);
void INA220_Init(void);
u8 INA220_CNVR(void);


#endif /* INC_USER_INA220_H_ */
