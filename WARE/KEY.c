#include "KEY.h"
#include "gpio.h"

u8 KEY_GetPinState(u8 key)
{
	switch(key)
	{
		case 1:
		{
			if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==0)
			{
				delay_ms(1);
				if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==0)
					return 1;
				return 0;	
			}
			return 0;
		
		}
		
		case 2:
		{
			if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==0)
			{
				delay_ms(1);
				if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==0)
					return 1;
				return 0;	
			}
			return 0;
		
		}
		case 3:
		{
			if(HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)==0)
			{
				delay_ms(1);
				if(HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)==0)
					return 1;
				return 0;	
			}
			return 0;
		
		}
		case 4:
		{
			if(HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)==0)
			{
				delay_ms(1);
				if(HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)==0)
					return 1;
				return 0;	
			}
			return 0;
		
		}
		default:
			return 0;
	}
}
