/*
 * QC_Get.c
 *
 *  Created on: Mar 14, 2022
 *      Author: Feng
 */
#include "main.h"
#include "adc.h"

#define DP0_0V  QC_DP_vol(0)
#define DP0_6V  QC_DP_vol(6)
#define DP3_3V  QC_DP_vol(33)
#define DM0_0V  QC_DM_vol(0)
#define DM0_6V  QC_DM_vol(6)
#define DM3_3V  QC_DM_vol(33)


extern u8 QC_handshake;
extern u8 voltage_num;
extern u8 adj_up,adj_down;//电压+、电压-
extern u8 func_index;
//u8 qc_check;

//D+初始化
void QC_DP_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);

	/*Configure GPIO pins : PB0 PB3 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}
//D-初始化
void QC_DM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	if(func_index==10)//QC2.0
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	}
	else//QC3.0
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	}

	/*Configure GPIO pins : PB0 PB3 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

//调整DP引脚的电压
void QC_DP_vol(u8 vol)
{
	if(vol==0)//0V
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	}
	if(vol==6)//0.6V
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
	}
	if(vol==33)//3.3V
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
	}
}
//调整DM引脚的电压
void QC_DM_vol(u8 vol)
{
	if(vol==0)//0V
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
	}
	if(vol==6)//0.6V
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	}
	if(vol==33)//3.3V
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
	}
}



//检测充电头是否支持QC协议
u8 QC_Check(void)
{
	u8 i;
	u16 DP_voltage;
	MX_ADC1_Init();
	HAL_ADCEx_Calibration_Start(&hadc1);
	QC_DP_Init();
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1,100);
	DP_voltage=HAL_ADC_GetValue(&hadc1);
	if(DP_voltage<700||DP_voltage>850)
		return 0;
	for(i=0;i<15;i++)
	{
		HAL_Delay(100);
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 100);
		DP_voltage = HAL_ADC_GetValue(&hadc1);
		if(DP_voltage<20)
		{
			HAL_ADC_DeInit(&hadc1);
			QC_DM_Init();
			return 1;
		}
	}
	return 0;
}
//QC2.0电压调整
void QC2_adj(u8 vol)
{
	if(vol==5)
	{
		DP0_6V;
		DM0_0V;
	}
	if(vol==9)
	{
		DP3_3V;
		DM0_6V;
	}
	if(vol==12)
	{
		DP0_6V;
		DM0_6V;
	}
	if(vol==20)
	{
		DP3_3V;
		DM3_3V;
	}

}

//QC3.0电压调整。1，升高电压；0，降低电压
void QC3_adj(u8 direct)
{

	if(direct)
	{
		DP3_3V;
		HAL_Delay(250);
		DP0_6V;
	}
	if(!direct)
	{
		DM0_6V;
		HAL_Delay(250);
		DM3_3V;
	}

}



//解除QC
void QC_DeInit(void)
{
	DP0_0V;
	DM0_0V;
	QC_handshake=0;
	voltage_num=0;
	adj_up=adj_down=0;
	HAL_ADC_DeInit(&hadc1);
	HAL_GPIO_DeInit(GPIOA,GPIO_PIN_0);
	HAL_GPIO_DeInit(GPIOA,GPIO_PIN_1);
	HAL_GPIO_DeInit(GPIOB,GPIO_PIN_0);
	HAL_GPIO_DeInit(GPIOB,GPIO_PIN_3);
}



