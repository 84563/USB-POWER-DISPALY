/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "user/oled.h"
#include "user/ina220.h"
#include "user/menu.h"

#include "adc.h"
#include "user/QC_Get.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define voltage ina220_data[0]
#define current ina220_data[1]
#define power ina220_data[2]
#define energy ina220_data[3]
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

u16 ina220_data[4]={0,0,0,0};//电压、电流�?�功率�?�电�???????
double energy_temp=0;//电量
u8 key_enter=0,key_up=0,key_down=0,key_back=0;
u8 adj_up=0,adj_down=0;

u8 time_1s_ok=0;
u8 func_index=0;//页码

u16 mem_addr_base=0x0000;
u16 mem_addr_base_r=0x0000;

u8 oled_on=1;//开启显示
u8 data_sw=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_TIM14_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim14);
  HAL_TIM_Base_Start_IT(&htim3);
  Clear_mem();

  OLED_Init_096();		//初始化OLED
  INA220_Init();		//初始化ina220
  key_back=key_enter=key_up=key_down=0;



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  OLED_Refresh();
//	  if(INA220_CNVR())//When the conversion is complete
	  if(time_1s_ok)
	  {
		  time_1s_ok=0;
		  voltage     = INA220_GetVoltage();
		  current     = INA220_GetCurrent();
		  power       = INA220_GetPower();
		  energy_temp = energy_temp + power*0.00027777;
		  energy      = energy_temp;

		  if(power!=0&&mem_addr_base<0xE00)
		  {
			  HAL_I2C_Mem_Write(&hi2c1,0xA0,mem_addr_base,I2C_MEMADD_SIZE_16BIT,(u8*)ina220_data,4,100);
			  mem_addr_base=mem_addr_base+4;
		  }
	  }

	  /*******************find index****************************/
	  if((key_enter==1)||(key_up==1)||(key_down==1))
	  {
		  if(key_up==1)
		  {
			  func_index=table[func_index].up;    //up
			  key_up=0;
		  }
		  if(key_down==1)
		  {
			  func_index=table[func_index].down;    //down
			  key_down=0;
		  }
		  if(key_enter==1)
		  {
			  OLED_Clear();
			  func_index=table[func_index].enter;    //enter
			  key_enter=0;
		  }

	  }
	  if(key_back==1)
	  {
		  key_back=0;
		  if(oled_on==1)
			  oled_on=0;
		  else
			  oled_on=1;
	  }
	  current_operation_index=table[func_index].current_operation;
	  (*current_operation_index)();//执行当前操作函数

	  HAL_Delay(10);


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1)
  {
  }

  /* HSI configuration and activation */
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 9, LL_RCC_PLLR_DIV_3);
  LL_RCC_PLL_Enable();
  LL_RCC_PLL_EnableDomain_SYS();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

  /* Set AHB prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* Sysclk activation on the main PLL */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Set APB1 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(48000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI4_15_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
  /* TIM3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* TIM14_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM14_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(TIM14_IRQn);
}

/* USER CODE BEGIN 4 */


//按键外部中断回调函数
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);//消抖，关闭中断
	__HAL_TIM_ENABLE(&htim14);//启动定时器，消抖时间到后再开启中断

	if(GPIO_Pin==GPIO_PIN_4)
	{
		key_back=1;
		if(func_index==9)
			data_sw=1;
	}
	if(GPIO_Pin==GPIO_PIN_5)
	{
		key_down=1;
		if(func_index==5||func_index==9||func_index==10||func_index==11)//向下调节
			adj_down=1;
	}
	if(GPIO_Pin==GPIO_PIN_6)
	{
		key_up=1;
		if(func_index==5||func_index==9||func_index==10||func_index==11)//向上调节
			adj_up=1;
	}
	if(GPIO_Pin==GPIO_PIN_7)
	{
		key_enter=1;
		if(func_index==10||func_index==11)//退出QC
			QC_DeInit();
		if(func_index==9)
			mem_addr_base_r=0;
	}
}
//tim中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim3)
	{
		time_1s_ok=1;//1s时间到
	}
	if(htim == &htim14)
	{
		HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);//消抖结束，开启中断
	}

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
