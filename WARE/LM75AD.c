#include "LM75AD.h"

extern I2C_HandleTypeDef hi2c1;//IIC1句柄

/*
0x00  Temperature Register      
0x01  Configuration register  器件模式 写00普通模式(100ms更新一次温度值) 写01为ShutDown模式
0x02  Hysteresis register
0x03  Over_temperature shutdown register
Temp Register 
MSByte                LSByte
7   6  5  4  3  2  1  0  7  6  5  4 3 2 1 0
D10 D9 D8 D7 D6 D5 D4 D3 D2 D1 D0 X X X X X
D10=0    ℃=+(Temp Data×0.125) 	
D10=1    ℃=-(Temp Data×0.125)
Address Table
MSB          LSB
1 0 0 1 A2 A1 A0
1 0 0 1 0  0  1 0/1       =0x92
*/
 
 
//LM75AD IO初始化
void LM75AD_Init(void)
{
		uint8_t *data;
		uint8_t iic_data=0x00;
		data = &iic_data;
		HAL_I2C_Mem_Write(&hi2c1,LM75AD_ADDR,0x01,I2C_MEMADD_SIZE_8BIT,data,1,100);
}
 
void LM75_Temp(uint8_t *temp)
{
		HAL_I2C_Mem_Read(&hi2c1,LM75AD_ADDR,0x00,I2C_MEMADD_SIZE_8BIT,temp,2,100);
}
