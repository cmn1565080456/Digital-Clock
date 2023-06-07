/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LED.h"
#include "KEY.h"
#include "OLED_SPI.h"
#include "LM75AD.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int main()
{
	SystemClock_Config();
	MX_GPIO_Init();
	MX_ADC1_Init();
	MX_SPI1_Init();
	MX_USART1_UART_Init();
	MX_RTC_Init();
	MX_I2C1_Init();
	
	OLED_Init();
	LM75AD_Init();
	delay_init(100);
	u8 beep_state[2]={0};	//蜂鸣器状态
	u8 choise=0;		//GUI选择
	u8 key_state[2]={0};//3，4按键状态
	u16 times=0;		//公用迭代数
	u16 time=0;


	RTC_DateTypeDef GetData;    //获取日期结构体
	RTC_TimeTypeDef GetTime;    //获取时间结构体
	
//开始设置开机动画
	GUI_Init();

	while(1)
	{
		
		
		switch (choise)
		{
			case 0:GUI_Time();break;//时间
			case 1:GUI_Alarm();break;//闹钟
			case 2:GUI_Temperature();break;//ADC采集V，温度
//			case 3:GUI_Real();break;//实时时间
			default:break;
		}
		

////////////////////////////////////////设置不同状态对应的状态变量choise以切换GUI///////////////////////////////////////////////////////////		
		if((GUI_TIME.GUI_state[1]==0)&&(GUI_ALARM.GUI_state[1]==0)&&(GUI_TEMPERATURE.GUI_state[1]==0))
		{
			//////////////////////按键3	
			if((KEY_GetPinState(3)==1)&&(key_state[0]==0))
			{
				key_state[0]=1;
			}
			else if((KEY_GetPinState(3)==0)&&(key_state[0]==1))
			{
				//////////////////////按键输出
				if(choise==0)
					choise=2;
				else
					choise--;
				//////////////////////	
				GUI_TIME.GUI_state[0]=0;
				GUI_ALARM.GUI_state[0]=0;	
				GUI_TEMPERATURE.GUI_state[0]=0;				
				key_state[0]=0;
			}
				//////////////////////按键4	
			if((KEY_GetPinState(4)==1)&&(key_state[1]==0))
			{
				key_state[1]=1;
			}
			else if((KEY_GetPinState(4)==0)&&(key_state[1]==1))
			{
				//////////////////////按键输出
				if(choise==2)
					choise=0;
				else
					choise++;				
				//////////////////////
				GUI_TIME.GUI_state[0]=0;
				GUI_ALARM.GUI_state[0]=0;	
				GUI_TEMPERATURE.GUI_state[0]=0;					
				key_state[1]=0;
			}
			
		}
		
/////////////////////////////////////////////蜂鸣器////////////////////////////////////////////
		
		if((RTC_TIME.Hours==GUI_ALARM.GUI_data[0])&&(RTC_TIME.Minutes==GUI_ALARM.GUI_data[1])&&(RTC_TIME.Seconds==GUI_ALARM.GUI_data[2]))
			if((GUI_ALARM.GUI_data[0]!=0)||(GUI_ALARM.GUI_data[1]!=0)||(GUI_ALARM.GUI_data[2]!=0))
				beep_state[0]=1;
			
		if(beep_state[1]==0)
			if(beep_state[0]==1)
			{

//				while(time<6)        //设置beep每隔0.5s响一次，连续响六次
//				{
//						times=0;
//						while(times<10)
//						{
//							LED_State(times+1,1);
//							delay_ms(25);
//							times++;
//							HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_SET);
//						}
//						times=0;
//						while(times<10)
//						{
//							LED_State(times+1,0);
//							delay_ms(25);
//							times++;
//							HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_RESET);
//						}
//						time++;
//				}
//				time=0;
//				beep_state[0]=0;
//				beep_state[1]=1;
					
					while(time<1)
					{
						beep_music();
						time++;
					}
				time=0;
				beep_state[0]=0;
				beep_state[1]=1;
			}
		
		
	
//按下按键时灯亮提示按键按下
		if(KEY_GetPinState(1))
			LED_State(1,1);
		else
			LED_State(1,0);
		if(KEY_GetPinState(2))
			LED_State(2,1);	
		else
			LED_State(2,0);
		if(KEY_GetPinState(3))
			LED_State(3,1);	
		else
			LED_State(3,0);
		if(KEY_GetPinState(4))
			LED_State(4,1);
		else
			LED_State(4,0);

	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
