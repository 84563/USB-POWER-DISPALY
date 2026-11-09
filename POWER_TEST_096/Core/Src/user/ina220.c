/*
 * ina220.c
 *
 *  Created on: 2022年2月22日
 *      Author: Feng
 */

#include "user/ina220.h"
//#include "main.h"

extern I2C_HandleTypeDef hi2c1;


//读取电压 4mV/bit
u16 INA220_GetVoltage(void)
{
	u16 temp=0;
	u8 buffer[2] = {0,0};

	HAL_I2C_Mem_Read(&hi2c1,INA220_ADDR,BV_REG,I2C_MEMADD_SIZE_8BIT,&buffer[0],2,100);
	temp = buffer[0]<<8|buffer[1];//读取到的两个8位二进制数合成一个16位二进制数
	temp >>= 3;					//右移三位对齐数据
	temp  *= 4;					//计算出毫伏电压

	return temp;
}

//读取电流 1mA/bit
u16 INA220_GetCurrent(void)
{
	u16 temp=0;
	u8 buffer[2] = {0,0};

	HAL_I2C_Mem_Read(&hi2c1,INA220_ADDR,CUR_REG,I2C_MEMADD_SIZE_8BIT,&buffer[0],2,100);
	temp = buffer[0]<<8|buffer[1];//读取到的两个8位二进制数合成一个16位二进制数
	//temp的值乘1即为毫安电流
	return temp;
}

//读取功率 20mW/bit
u16 INA220_GetPower(void)
{
	u16 temp=0;
	u8 buffer[2] = {0,0};

	HAL_I2C_Mem_Read(&hi2c1,INA220_ADDR,PWR_REG,I2C_MEMADD_SIZE_8BIT,&buffer[0],2,100);
	temp = buffer[0]<<8|buffer[1];//读取到的两个8位二进制数合成一个16位二进制数
	temp*= 20;  //计算毫瓦功率
	return temp;
}

void INA220_Init(void)
{
	u8 CFG[2] = {0x27,0xff}; //配置寄存器0x21D7
	u8 CAL[2] = {0x20,0x00}; //校准寄存器0x5000，1ma/bit

	HAL_I2C_Mem_Write(&hi2c1,INA220_ADDR,CFG_REG,I2C_MEMADD_SIZE_8BIT,&CFG[0],2,100);//设置配置寄存器
	HAL_I2C_Mem_Write(&hi2c1,INA220_ADDR,CAL_REG,I2C_MEMADD_SIZE_8BIT,&CAL[0],2,100);//设置校准寄存器
}

u8 INA220_CNVR(void)
{
	u8 buffer[2] = {0,0};

	HAL_I2C_Mem_Read(&hi2c1,INA220_ADDR,BV_REG,I2C_MEMADD_SIZE_8BIT,&buffer[0],2,100);
	buffer[1] >>= 1;
	return buffer[1]&1;

}



