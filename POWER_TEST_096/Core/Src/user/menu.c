/*
 * menu.c
 *
 *  Created on: 2022年3月13日
 *      Author: Feng
 */

#include "user/oled.h"
#include "i2c.h"
#include "user/menu.h"
#include "user/QC_Get.h"


#define voltage ina220_data[0]
#define current ina220_data[1]
#define power ina220_data[2]
#define energy ina220_data[3]

extern u16 ina220_data[4];//电压、电流、功率、电量
extern u8 func_index;//主页
extern u8 adj_down,adj_up;//-、+
extern u8 oled_on;
extern u8 data_sw;
extern u16 mem_addr_base;

void (*current_operation_index)();



u8 brightness=0xCD;//亮度
u8 QC_handshake=0;//QC握手成功标志
u8 QC2_voltage[4]={5,9,12,20};//QC2.0目标电压列表
u8 voltage_num=0;//QC2.0电压序号
u16 QC3_voltage;//QC3.0目标电压
extern u16 mem_addr_base_r;
u8 Target_voltage;



void Clear_mem(void)
{
	u8 temp[128];
	u16 i;
	u16 addr_base=0x0000;
	for(i=0;i<128;i++)
	{
		temp[i]=0x00;
	}
	for(i=0;i<128;i++)
	{
		HAL_I2C_Mem_Write(&hi2c1,0xA0,addr_base,I2C_MEMADD_SIZE_16BIT,temp,128,100);
		HAL_Delay(5);
		addr_base+=128;
	}
}

//清除高位的0
void CLear0(u8 x,u8 y,u16 num)
{
	u8 i;
	u8 j=0;
	u16 temp=10000;
	for(i=0;i<4;i++)
	{
		if(num/temp==0)
		{
			j++;
			temp/=10;
		}
		else
			break;
	}
	OLED_ShowNum(x,y,0,j,12,2);//消除0
}


//绘制画百分比条
void DrawPctLine(u8 x,u8 y,u8 len,u8 mode)
{
	u8 i,temp;
	if(mode!=3)
		if(len==0)
			temp=0;
		else
			temp=len*0.7-1;
	else
	{
		if(len==0)
			temp=0;
		else
			temp=len*0.35-1;
		mode=1;
	}
	if(temp==0&&len!=0)
		temp=1;
	if(temp<35)
	for(i=0;i<temp;i++)
	{
		OLED_DrawLine(x,y-5,x,y,mode);
		x++;
	}
	else
	{
		temp=temp-35;
		for(i=0;i<temp;i++)
		{
			OLED_DrawLine(x,y-5-8,x,y-8,mode);
			x++;
		}
		x=0;
		for(i=0;i<35;i++)
		{
			OLED_DrawLine(x,y-5,x,y,mode);
			x++;
		}

	}
}

//刷新百分比条
void Pct_Refresh(void)
{
	u8 temp;
	DrawPctLine(0,15,100,0);//clear P
	DrawPctLine(0,28,100,0);//clear V
	DrawPctLine(0,45,100,0);//clear I

	temp=power/650;//temp=power/1000/65*100
	if(temp>100)
		temp=100;
	DrawPctLine(0,15,temp,1);
	temp=voltage/200;//temp=voltage/1000/20*100
	if(temp>100)
		temp=100;
	DrawPctLine(0,28,temp,3);
	temp=current/60;//temp=current/1000/6*100
	if(temp>100)
		temp=100;
	DrawPctLine(0,45,temp,1);


//	OLED_DrawLine(0,7,34,7,1);//P
	OLED_DrawLine(0,15,34,15,1);//P
	OLED_DrawLine(0,28,34,12,1);//V
//	OLED_DrawLine(0,37,34,12,1);//I
	OLED_DrawLine(0,45,34,12,1);//I


}



void Show_power(u8 x,u8 y)//显示功率
{
	OLED_ShowChinese(x,y,3,12,1);		//功
	OLED_ShowChinese(x+13,y,4,12,1);	//率
	OLED_ShowChar(x+24,y+3,58,8,1);		//:
	OLED_ShowNum(x+30,y,power,5,12,1);	//功率值
	CLear0(x+30,y,power);
	OLED_ShowChar(x+64,y,109,16,1);		//m
	OLED_ShowChar(x+74,y,87,16,1);		//W
}
void Show_voltage(u8 x,u8 y)//显示电压
{
	OLED_ShowChinese(x,y,0,12,1);		//电
	OLED_ShowChinese(x+13,y,1,12,1);	//压
	OLED_ShowChar(x+24,y+3,58,8,1);		//:
	OLED_ShowNum(x+30,y,voltage,5,12,1);//电压值
	CLear0(x+30,y,voltage);
	OLED_ShowChar(x+64,y,109,16,1);	    //m
	OLED_ShowChar(x+74,y,86,16,1);		//V
}
void Show_current(u8 x,u8 y)//显示电流
{
	OLED_ShowChinese(x,y,0,12,1);		//电
	OLED_ShowChinese(x+13,y,2,12,1);	//流
	OLED_ShowChar(x+24,y+3,58,8,1);		//:
	OLED_ShowNum(x+30,y,current,5,12,1);//电流值
	CLear0(x+30,y,current);
	OLED_ShowChar(x+64,y,109,16,1);		//m
	OLED_ShowChar(x+74,y,65,16,1);		//A
}
void Show_energy(u8 x,u8 y)//显示电量
{
	OLED_ShowChinese(x-26,y,23,12,1);		//充
	OLED_ShowChinese(x-13,y,24,12,1);	//入
	OLED_ShowChinese(x,y,0,12,1);		//电
	OLED_ShowChinese(x+13,y,20,12,1);	//量
	OLED_ShowChar(x+24,y+3,58,8,1);		//:
	OLED_ShowNum(x+30,y,energy,5,12,1);	//电量值
	CLear0(x+30,y,energy);
	OLED_ShowChar(x+64,y,109,16,1);		//m
	OLED_ShowChar(x+74,y,87,16,1);		//W
	OLED_ShowChar(x+84,y-3,104,16,1);	//h
}


//显示固定菜单
void show1_fun1_3(void)
{
	OLED_ShowChinese(20,0,5,12,1);		//设
	OLED_ShowChinese(33,0,6,12,1);		//置

	OLED_ShowChinese(68,0,7,12,1);		//快
	OLED_ShowChinese(81,0,8,12,1);		//充
	OLED_ShowChinese(94,0,11,12,1);		//诱
	OLED_ShowChinese(107,0,12,12,1);	//骗

	OLED_ShowChinese(10,16,15,12,1);	//数
	OLED_ShowChinese(23,16,16,12,1);	//据
	OLED_ShowChinese(36,16,17,12,1);	//计
	OLED_ShowChinese(49,16,18,12,1);	//录

	OLED_ShowChinese(78,16,21,12,1);	//返
	OLED_ShowChinese(91,16,22,12,1);	//回

}
//显示或清除[]
void show_brackets(u8 func_index,u8 mode)
{
	switch(func_index)
	{
	case 1:
		OLED_ShowChar(14,0,91,12,mode);		//[
		OLED_ShowChar(55,0,93,12,mode);		//]
		break;
	case 2:
		OLED_ShowChar(62,0,91,12,mode);		//[
		OLED_ShowChar(119,0,93,12,mode);	//]
		break;
	case 3:
		OLED_ShowChar(4,16,91,12,mode);		//[
		OLED_ShowChar(61,16,93,12,mode);	//]
		break;
	case 4:
		OLED_ShowChar(72,16,91,12,mode);	//[
		OLED_ShowChar(107,16,93,12,mode);	//]
		break;
	default:break;
	}

}

//显示固定菜单
void show1_fun6_8(void)
{
	OLED_ShowChar(20, 0, 81, 12, 1);		//Q
	OLED_ShowChar(27, 0, 67, 12, 1);		//C
	OLED_ShowChar(34, 0, 50, 12, 1);		//2
	OLED_ShowChar(40, 0, 46, 12, 1);		//.
	OLED_ShowChar(44, 0, 48, 12, 1);		//0

	OLED_ShowChar(73, 0, 81, 12, 1);		//Q
	OLED_ShowChar(80, 0, 67, 12, 1);		//C
	OLED_ShowChar(87, 0, 51, 12, 1);		//3
	OLED_ShowChar(93, 0, 46, 12, 1);		//.
	OLED_ShowChar(97, 0, 48, 12, 1);		//0


	OLED_ShowChinese(78, 16, 21, 12, 1);	//返
	OLED_ShowChinese(91, 16, 22, 12, 1);	//回
}

void fun0(void)
{
	Pct_Refresh();//刷新百分比条
	Show_energy(28,50);
	Show_current(40,34);
	Show_voltage(40,18);
	Show_power(40,2);
	if(!oled_on)
		OLED_Clear();
}
void fun1(void)
{
	show1_fun1_3();
	show_brackets(2,2);
	show_brackets(4,2);
	show_brackets(1,1);
}
void fun2(void)
{
	show1_fun1_3();
	show_brackets(1,2);
	show_brackets(3,2);
	show_brackets(2,1);
}
void fun3(void)
{
	show1_fun1_3();
	show_brackets(2,2);
	show_brackets(4,2);
	show_brackets(3,1);
}
void fun4(void)
{
	show1_fun1_3();
	show_brackets(3,2);
	show_brackets(1,2);
	show_brackets(4,1);
}
void fun5(void)//亮度调整
{
	OLED_On();
	OLED_ShowChinese(36,16,32,12,1);		//亮
	OLED_ShowChinese(49,16,33,12,1);		//度
	OLED_ShowChar(60,19,58,8,1);			//:
	OLED_ShowNum(66,16,brightness,3,12,1);	//电量值
	if(adj_up==1 && brightness<255)
	{
		brightness+=10;
		adj_up=0;
		OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
		OLED_WR_Byte(brightness,OLED_CMD);// Set SEG Output Current Brightness
	}
	if(adj_down==1 && brightness>5)
	{
		brightness-=10;
		adj_down=0;
		OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
		OLED_WR_Byte(brightness,OLED_CMD);// Set SEG Output Current Brightness
	}
}
void fun6(void)
{
	show1_fun6_8();
	show_brackets(2,2);
	show_brackets(4,2);
	show_brackets(1,1);

}
void fun7(void)
{
	show1_fun6_8();
	show_brackets(1,2);
	show_brackets(4,2);
	show_brackets(2,1);

}
void fun8(void)
{
	show1_fun6_8();
	show_brackets(2,2);
	show_brackets(1,2);
	show_brackets(4,1);

}
void fun9(void)
{
	u16 temp[2]={0,0};
	u8 x,y;
	u16 offset=0;
//	u16 set[3]={236,125,1875};
	static u8 fun9_mode=0;
	switch(fun9_mode)
	{
	case 0: {
		OLED_ShowString_H(0, 55, (u8*) "5V", 8, 1);
		OLED_ShowString_H(0, 0, (u8*) "20V", 8, 1);
		OLED_ShowString_H(0, 26, (u8*) "V", 8, 1);
		for(x=8;x<128;x++)
		{
			HAL_I2C_Mem_Read(&hi2c1,0xA1,mem_addr_base_r+offset,I2C_MEMADD_SIZE_16BIT,(u8*)temp,4,100);
			y=temp[fun9_mode]/236;
			y=64-y+21;
			OLED_DrawPoint(x, y, 1);
			offset+=4;
//			HAL_Delay(2);
		}
		offset=0;
		break;
	}
	case 1: {
		OLED_ShowString_H(0, 55, (u8*) "0A", 8, 1);
		OLED_ShowString_H(0, 0, (u8*) "6A", 8, 1);
		OLED_ShowString_H(0, 26, (u8*) "A", 8, 1);
		for(x=8;x<128;x++)
		{
			HAL_I2C_Mem_Read(&hi2c1,0xA1,mem_addr_base_r+offset,I2C_MEMADD_SIZE_16BIT,(u8*)temp,4,100);

		/*	if(temp[fun9_mode]==0xffff)
				y=0;
			else*/
				y=temp[fun9_mode]/125;
				y=64-y;
			OLED_DrawPoint(x, y, 1);
			offset+=4;
		}
		offset=0;
		break;
	}
	case 2: {
		OLED_ShowString_H(0, 55, (u8*) "0W", 8, 1);
		OLED_ShowString_H(0, 0, (u8*) "20W", 8, 1);
		OLED_ShowString_H(0, 26, (u8*) "P", 8, 1);
		for(x=8;x<128;x++)
		{
			HAL_I2C_Mem_Read(&hi2c1,0xA1,mem_addr_base_r+offset,I2C_MEMADD_SIZE_16BIT,(u8*)temp,4,100);
			y=temp[0]*temp[1]/1875;
			y=64-y;
			OLED_DrawPoint(x, y, 1);
			offset+=4;
		}
		offset=0;
		break;
	}
	default:
		break;
	}

/*	for(x=8;x<128;x++)
	{
		HAL_I2C_Mem_Read(&hi2c1,0xA1,mem_addr_base_r+offset,I2C_MEMADD_SIZE_16BIT,(u8*)temp,4,100);
		y=temp[fun9_mode]/set[fun9_mode];
		if(fun9_mode==0)
			y=64-y+21;
		else
			y=64-y;
		OLED_DrawPoint(x, y, 1);
		offset+=4;
	}
	offset=0;
*/
	if(data_sw==1)
	{
		data_sw=0;
		fun9_mode++;
		if(fun9_mode>2)
			fun9_mode=0;
		OLED_Clear();
	}
	if(adj_up==1)
	{
		adj_up=0;
		mem_addr_base_r+=120;
		if(mem_addr_base_r>0x3FFF)
			mem_addr_base_r=0x3FFF;
		OLED_Clear();
	}
	if(adj_down==1)
	{
		adj_down=0;
		mem_addr_base_r-=120;
		if(mem_addr_base_r>0x3FFF)
			mem_addr_base_r=0x0000;
		OLED_Clear();
	}
}
void fun10(void)//QC2.0诱骗
{
	if(!QC_handshake)
	{
		QC_handshake=1;

		OLED_ShowChinese(20, 0, 25, 12, 1);		//握
		OLED_ShowChinese(33, 0, 26, 12, 1);		//手
		OLED_ShowChinese(46, 0, 27, 12, 1);		//中
		OLED_ShowChar(57, 0, 46, 12, 1);		//.
		OLED_ShowChar(61, 0, 46, 12, 1);		//.
		OLED_Refresh();
		if (!QC_Check())
		{
			voltage_num=0;
			QC_handshake=0;
		}
		else
			OLED_Clear();
	}
	else
	{
		OLED_ShowChar(0, 0, 81, 12, 1);		//Q
		OLED_ShowChar(7, 0, 67, 12, 1);		//C
		OLED_ShowChar(14, 0, 50, 12, 1);	//2
		OLED_ShowChar(20, 0, 46, 12, 1);	//.
		OLED_ShowChar(24, 0, 48, 12, 1);	//0
		OLED_ShowChinese(31,0,30,12,1);		//目
		OLED_ShowChinese(44,0,31,12,1);		//标
		OLED_ShowChinese(57,0,0,12,1);		//电
		OLED_ShowChinese(70,0,1,12,1);		//压
		OLED_ShowChar(81,3,58,8,1);		    //:
		OLED_ShowNum(87,0,QC2_voltage[voltage_num],2,12,1);//电压值
		if(QC2_voltage[voltage_num]/10==0)
			OLED_ShowNum(87,0,0,1,12,2);		  //消除首位的0
		OLED_ShowChar(103,0,86,12,1);		//V

/*		OLED_ShowChinese(0,16,0,12,1);		//电
		OLED_ShowChinese(13,16,1,12,1);		//压
		OLED_ShowChar(24,19,58,8,1);		//:
		OLED_ShowNum(30,16,voltage,5,12,1);	//电压值
		if(voltage/10000==0)
			OLED_ShowNum(30,16,0,1,12,2);	//消除首位的0

		OLED_ShowChinese(0,31,0,12,1);	    //电
		OLED_ShowChinese(13,31,2,12,1);	    //流
		OLED_ShowChar(24,34,58,8,1);		//:
		OLED_ShowNum(30,31,current,5,12,1);	//电流值
		if(current/10000==0)
			OLED_ShowNum(30,31,0,1,12,2);	//消除首位的0

		OLED_ShowChinese(0,48,3,12,1);		//功
		OLED_ShowChinese(13,48,4,12,1);		//率
		OLED_ShowChar(24,51,58,8,1);		//:
		OLED_ShowNum(30,48,power,5,12,1);	//功率值
		if(power/10000==0)
			OLED_ShowNum(30,48,0,1,12,2);
*/
		Show_voltage(0, 16);
		Show_current(0, 31);
		Show_power(0, 48);

		if(adj_up==1)
		{
			adj_up=0;
			voltage_num++;
			if(voltage_num>3)
				voltage_num=3;
		}
		if(adj_down==1)
		{
			adj_down=0;
			voltage_num--;
			if(voltage_num>3)
				voltage_num=0;
		}
		QC2_adj(QC2_voltage[voltage_num]);//调整电压

	}
}
void fun11(void)//QC3.0诱骗
{
	if(!QC_handshake)
	{
		QC_handshake=1;

		OLED_ShowChinese(20, 0, 25, 12, 1);		//握
		OLED_ShowChinese(33, 0, 26, 12, 1);		//手
		OLED_ShowChinese(46, 0, 27, 12, 1);		//中
		OLED_ShowChar(57, 0, 46, 12, 1);		//.
		OLED_ShowChar(61, 0, 46, 12, 1);		//.
		OLED_ShowChar(65, 0, 46, 12, 1);		//.
		OLED_Refresh();
		if (!QC_Check())
		{
			QC_handshake=0;
		}
		else
		{
			OLED_Clear();
			Target_voltage=50;
		}
	}
	else
	{

		OLED_ShowChar(0, 0, 81, 12, 1);		//Q
		OLED_ShowChar(7, 0, 67, 12, 1);		//C
		OLED_ShowChar(14, 0, 51, 12, 1);	//3
		OLED_ShowChar(20, 0, 46, 12, 1);	//.
		OLED_ShowChar(24, 0, 48, 12, 1);	//0
		OLED_ShowChinese(31,0,30,12,1);		//目
		OLED_ShowChinese(44,0,31,12,1);		//标
		OLED_ShowChinese(57,0,0,12,1);		//电
		OLED_ShowChinese(70,0,1,12,1);		//压
		OLED_ShowChar(81,3,58,8,1);		    //:
		OLED_ShowNum(87,0,Target_voltage/10,2,12,1);//电压值1
		if(Target_voltage/100==0)
			OLED_ShowNum(87,0,0,1,12,2);		  //消除首位的0
		OLED_ShowChar(99, 0, 46, 12, 1);		  //.
		OLED_ShowNum(103,0,Target_voltage,1,12,1);//电压值2
		OLED_ShowChar(111,0,86,12,1);		//V

/*		OLED_ShowChinese(0,16,0,12,1);		//电
		OLED_ShowChinese(13,16,1,12,1);		//压
		OLED_ShowChar(24,19,58,8,1);		//:
		OLED_ShowNum(30,16,voltage,5,12,1);	//电压值
		if(voltage/10000==0)
			OLED_ShowNum(30,16,0,1,12,2);	//消除首位的0

		OLED_ShowChinese(0,31,0,12,1);	    //电
		OLED_ShowChinese(13,31,2,12,1);	    //流
		OLED_ShowChar(24,34,58,8,1);		//:
		OLED_ShowNum(30,31,current,5,12,1);	//电流值
		if(current/10000==0)
			OLED_ShowNum(30,31,0,1,12,2);	//消除首位的0

		OLED_ShowChinese(0,48,3,12,1);		//功
		OLED_ShowChinese(13,48,4,12,1);		//率
		OLED_ShowChar(24,51,58,8,1);		//:
		OLED_ShowNum(30,48,power,5,12,1);	//功率值
		if(power/10000==0)
			OLED_ShowNum(30,48,0,1,12,2);*/

		Show_voltage(0, 16);
		Show_current(0, 31);
		Show_power(0, 48);

		if(adj_up==1)
		{
			adj_up=0;
			QC3_adj(1);
			Target_voltage+=2;
			if(Target_voltage>200)
				Target_voltage=200;
		}
		if(adj_down==1)
		{
			adj_down=0;
			if(voltage>4500)
				QC3_adj(0);
			Target_voltage-=2;
			if(Target_voltage<45)
				Target_voltage=44;
		}
	}

}


key_table table[30]=
{
	{0,0,0,1,(*fun0)}, //第0层,显示主界面

	{1,4,2,5,(*fun1)}, //第一层，显示【亮度设置】、快充诱骗、  数据记录、  返回
	{2,1,3,6,(*fun2)}, //第一层，显示  亮度设置、【快充诱骗】、数据记录、  返回
	{3,2,4,9,(*fun3)}, //第一层，显示  亮度设置、  快充诱骗、【数据记录】、返回
	{4,3,1,0,(*fun4)}, //第一层，显示  亮度设置、  快充诱骗、  数据记录、【返回】

	{5,5,5,1,(*fun5)}, //第二层，亮度设置

	{6,8,7,10,(*fun6)}, //第二层，快充诱骗层下显示【QC 2.0】、 QC3.0 、  返回
	{7,6,8,11,(*fun7)}, //第二层，快充诱骗层下显示  QC 2.0 、【QC3.0】、 返回
	{8,7,6,2,(*fun8)},  //第二层，快充诱骗层下显示  QC 2.0 、  QC3.0 、【返回】

	{9,9,9,3,(*fun9)}, //第二层，数据记录

	{10,10,10,6,(*fun10)},   //第三层，快充诱骗QC 2.0
	{11,11,11,7,(*fun11)},//第三层，快充诱骗QC 3.0


};
