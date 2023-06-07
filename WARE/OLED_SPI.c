#include "OLED_SPI.h"
#include "Char_library.h"
#include "rtc.h"
#include "KEY.h"
#include "adc.h"
#include "i2c.h"
#include "LM75AD.h"
GUI GUI_TIME={0,0,0,0};
GUI GUI_ALARM={0,0,0,0};
GUI GUI_TEMPERATURE={0,0,0,0};

u16 temp=0;
u16 temps=0;
RTC_DateTypeDef RTC_DATE;      //获取日期结构体
RTC_TimeTypeDef RTC_TIME;      //获取时间结构体
u8 OLED_RAM[OLED_WIDE][OLED_HIGH];


#define LM75A_ADDRESS 0x90  // LM75A芯片的设备地址
uint8_t tempData[2];  // 存储LM75A温度数据的数组
int16_t temperature;
float temperatureC;
////////////////////////////////////////////////////////////////////基础函数///////////////////////////
void OLED_Init(void)
{
	OLED_RES_Reset();
	HAL_Delay(200);
	OLED_RES_Set();
	
	OLED_WriteByte(0xAE,OLED_CMD);//--turn off OLED panel
	OLED_WriteByte(0x00,OLED_CMD);//---set low column address
	OLED_WriteByte(0x10,OLED_CMD);//---set high column address
	OLED_WriteByte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WriteByte(0x81,OLED_CMD);//--set contrast control register
	OLED_WriteByte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	OLED_WriteByte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WriteByte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WriteByte(0xA6,OLED_CMD);//--set normal display
	OLED_WriteByte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WriteByte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WriteByte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WriteByte(0x00,OLED_CMD);//-not offset
	OLED_WriteByte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WriteByte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WriteByte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WriteByte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WriteByte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WriteByte(0x12,OLED_CMD);
	OLED_WriteByte(0xDB,OLED_CMD);//--set vcomh
	OLED_WriteByte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WriteByte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WriteByte(0x02,OLED_CMD);//
	OLED_WriteByte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WriteByte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WriteByte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WriteByte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WriteByte(0xAF,OLED_CMD);
	OLED_Clear();

}
void OLED_WriteByte(u8 dat,u8 mode)
{
	/*
	dat 是一个无符号8位整数，代表要发送的数据或命令。
	mode 是一个无符号8位整数，决定 dat 是数据还是命令。当 mode为1时，dat被视为数据；当 mode为0时，dat被视为命令。
	*/
	uint8_t * data = &dat;		  
	if(mode)
	  OLED_DC_Set();
	else
	  OLED_DC_Reset();
	OLED_CS_Reset();
	
	HAL_SPI_Transmit(&hspi1,(uint8_t *)data,1,200);
	
	OLED_CS_Set();
	OLED_DC_Set();  	  
}


void OLED_SetPlace(uint8_t x,uint8_t y)
{
	OLED_WriteByte(0xb0+y,OLED_CMD);//设置行起始地址
	OLED_WriteByte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WriteByte((x&0x0f),OLED_CMD);
}
void OLED_Refresh(void)
{
	uint8_t i,n;
	for(i=0;i<8;i++)
	{
	   OLED_WriteByte(0xb0+i,OLED_CMD); //设置行起始地址
	   OLED_WriteByte(0x00,OLED_CMD);   //设置低列起始地址
	   OLED_WriteByte(0x10,OLED_CMD);   //设置高列起始地址
	   for(n=0;n<128;n++)
		 OLED_WriteByte(OLED_RAM[n][i],OLED_DATA);
  }
}
void OLED_Clear(void)
{
	uint8_t i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_RAM[n][i]=0;//清除所有数据
			}
  }
	OLED_Refresh();//更新显示
}
void OLED_ColorTurn(uint8_t i)
{
	if(i==0)
		{
			OLED_WriteByte(0xA6,OLED_CMD);//正常显示
		}
	if(i==1)
		{
			OLED_WriteByte(0xA7,OLED_CMD);//反色显示
		}
}
void OLED_DisplayTurn(uint8_t i)
{
	if(i==0)
		{
			OLED_WriteByte(0xC8,OLED_CMD);//正常显示
			OLED_WriteByte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WriteByte(0xC0,OLED_CMD);//反转显示
			OLED_WriteByte(0xA0,OLED_CMD);
		}
} 
void OLED_DisPlay_On(void)
{
	OLED_WriteByte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WriteByte(0x14,OLED_CMD);//开启电荷泵
	OLED_WriteByte(0xAF,OLED_CMD);//点亮屏幕
}
  
void OLED_DisPlay_Off(void)
{
	OLED_WriteByte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WriteByte(0x10,OLED_CMD);//关闭电荷泵
	OLED_WriteByte(0xAF,OLED_CMD);//关闭屏幕
}
void OLED_nPoint(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	OLED_SetPlace(x,i);
	OLED_RAM[x][i] &=~n;
	OLED_WriteByte(OLED_RAM[x][i],OLED_DATA);
}
void OLED_Point_NW(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	OLED_RAM[x][i]|=n;
}
void OLED_nPoint_NW(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	OLED_RAM[x][i] &=~n;
}
void OLED_Point(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	OLED_RAM[x][i]|=n;
	OLED_SetPlace(x,i);
	OLED_WriteByte(OLED_RAM[x][i],OLED_DATA);
}
////////////////////////////////////////////////////////////////////扩展函数
void OLED_Circle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_Point(x + a, y - b);
        OLED_Point(x - a, y - b);
        OLED_Point(x - a, y + b);
        OLED_Point(x + a, y + b);
 
        OLED_Point(x + b, y + a);
        OLED_Point(x + b, y - a);
        OLED_Point(x - b, y - a);
        OLED_Point(x - b, y + a);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}
void OLED_ShowNum_Sign(u8 x,u8 y, int num,u8 len,u8 size) 
{
	short int num1=0;
	if(num&0x8000)
	{
		OLED_ShowChar(x,y,45,size);
		num1=-num;
	}
	else
	{
		OLED_ShowChar(x,y,32,size);
		num1=num;
	}
	OLED_ShowNum(x+size/2,y,num1,len-1,size);
}
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1)
{
	u8 t,temp;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2)*t,y,'0',size1);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2)*t,y,temp+'0',size1);
			}
  }
}
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1)
{
	u16 i,m,size2,chr1;
	u8 temp;									//i,m对RAM操作的循环，temp存储字符，size所需字节数，chr1字符位置
	u16 page=0,a,b;								//page计需要的页数，a、b为显示操作的循环
	u16 y0=y;									//y0为RAM操作所需折返时的中间值
	u16 x0=x;									//x0为显示操作表示x坐标的中间值
	size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++)
	{
		switch(size1)
		{
			case 12:temp=asc2_1206[chr1][i];break; //调用1206字体
			case 16:temp=asc2_1608[chr1][i];break;	 //调用1608字体
			case 24:temp=asc2_2412[chr1][i];break;	 //调用2412字体
			case 32:temp=asc2_3216[chr1][i];break;	//调用3216字体
			default:return;
		}
		for(m=0;m<8;m++)           //写入数据
		{
			if(temp&0x80)
				OLED_Point_NW(x,y);
			else 
				OLED_nPoint_NW(x,y);
			temp<<=1;
			y++;
			if((y-y0)==size1)
			{
				y=y0;
				x++;
				break;
			}
		}
	}
	if ((y%8)+(size1%8)==8) 
		page=size1/8+((size1%8)?1:0);
	else 
		page=size1/8+((size1%8)?1:0)+1;
	for(a=0;a<page;a++)
		for(b=0;b<size1/2;b++)
		{
			OLED_SetPlace(x0+b,a+y/8);
			OLED_WriteByte(OLED_RAM[x0+b][a+y/8],OLED_DATA);
		}
}
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		OLED_ShowChar(x,y,*chr,size1);
		x+=size1/2;
		if(x>128-size1)  //换行
		{
			x=0;
			y+=2;
		}
		chr++;
  }
}
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1)
{
	u16 i,m,n=0,chr1,page;
	u16 x0=x,y0=y;
	u16 x1=x,y1=y,a,b;
	u16 size3=size1/8;
	u8 temp;
	while(size3--)
	{
		chr1=num*size1/8+n;
		n++;
			for(i=0;i<size1;i++)
			{
				switch(size1)
				{
					case 12:
					case 16:temp=chinese16[chr1][i];break; //调用16*16
					case 24:temp=chinese24[chr1][i];break; //调用24*24
					case 32:temp=chinese32[chr1][i];break; //调用32*32
					case 64:temp=chinese64[chr1][i];break; //调用64*64
					
					case 110:temp=init1[chr1][i];break;  //自定义字符
					case 120:temp=init2[chr1][i];break;
					case 130:temp=init3[chr1][i];break;
					case 140:temp=init3[chr1][i];break;
					default:return;
				}		
				for(m=0;m<8;m++)
				{
					if(temp&0x01)OLED_Point_NW(x,y);
					else OLED_nPoint_NW(x,y);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==size1)
				{x=x0;y0=y0+8;}
				y=y0;
			 }
	}
	if ((y1%8)+(size1%8)==8) 
	page=size1/8+((size1%8)?1:0);
	else 
	page=size1/8+((size1%8)?1:0)+1;
	for(a=0;a<page;a++)
		for(b=0;b<size1;b++)
		{
			OLED_SetPlace(x1+b,a+y1/8);
			OLED_WriteByte(OLED_RAM[x0+b][a+y1/8],OLED_DATA);
		}
}
////////////////////////////////////////////////////////////////////GUI函数
void GUI_Time(void)
{
	if(GUI_TIME.GUI_state[0]==0)//进入初始化
	{
		OLED_Clear();
		OLED_ShowString(10,16,"TIME:",16);
		OLED_ShowString(42,43,"h",12);
		OLED_ShowString(80,43,"m",12);
		OLED_ShowString(118,43,"s",12);
		OLED_ShowString(10,0,"Date:",16);
		//OLED_ShowString(82,0,"`C",16);
		GUI_TIME.GUI_state[0]=1;
	}
	
	HAL_RTC_GetTime(&hrtc,&RTC_TIME,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&RTC_DATE,RTC_FORMAT_BIN);
	
/////////////////////////////////////显示部分	
//日期显示
OLED_ShowNum_Sign(46,0,RTC_DATE.Year+2000,5,16);
OLED_ShowNum_Sign(85,0,RTC_DATE.Month,2,16);
OLED_ShowNum_Sign(100,0,RTC_DATE.WeekDay,2,16);
OLED_ShowString(85,16,"7th",16);
	

	
//温度显示
//		HAL_I2C_Master_Transmit(&hi2c1, LM75A_ADDRESS , 0x00, 1, HAL_MAX_DELAY);  // 发送读取温度的命令
//		HAL_I2C_Master_Receive(&hi2c1, LM75A_ADDRESS, tempData, 2, HAL_MAX_DELAY);  // 接收温度数据
//		temperature = (tempData[0] << 8) | tempData[1];  // 将两个字节的温度数据合并为一个16位有符号整数
//		temperature = temperature >> 5;  // 温度数据占用12位，将高5位清零

//		if (temperature & 0x0400)  // 最高位为1表示负温度
//		{
//			temperature |= 0xF800;  // 将高11位扩展为16位有符号数
//		}
//		else
//		temperatureC = temperature * 0.125;
//		OLED_ShowNum_Sign(50,0,temperatureC,3,16);

	if(GUI_TIME.GUI_state[1]==0)//正常显示
	{
		OLED_ShowNum(10,31,RTC_TIME.Hours+GUI_TIME.GUI_data[0],2,32);
		OLED_ShowNum(48,31,RTC_TIME.Minutes+GUI_TIME.GUI_data[1],2,32);
		OLED_ShowNum(86,31,RTC_TIME.Seconds+GUI_TIME.GUI_data[2],2,32);
	}
	else if(GUI_TIME.GUI_state[1]==1)//时间设置
	{
		if(GUI_TIME.KEYs[0]==0)		//hour 十位
		{
			OLED_ShowNum(48,31,RTC_TIME.Minutes+GUI_TIME.GUI_data[1],2,32);
			OLED_ShowNum(86,31,RTC_TIME.Seconds+GUI_TIME.GUI_data[2],2,32);
			if(GUI_TIME.GUI_data[7]<20)
			{
				OLED_ShowString(10,31," ",32);
				GUI_TIME.GUI_data[7]++;
			}
			else if(GUI_TIME.GUI_data[7]<40)
			{
				OLED_ShowNum(10,31,RTC_TIME.Hours+GUI_TIME.GUI_data[0],2,32);
				GUI_TIME.GUI_data[7]++;
			}
			else
				GUI_TIME.GUI_data[7]=0;
		}
		else if(GUI_TIME.KEYs[0]==1)		//hour 个位
		{
			OLED_ShowNum(48,31,RTC_TIME.Minutes+GUI_TIME.GUI_data[1],2,32);
			OLED_ShowNum(86,31,RTC_TIME.Seconds+GUI_TIME.GUI_data[2],2,32);
			if(GUI_TIME.GUI_data[7]<20)
			{
				OLED_ShowString(26,31," ",32);
				GUI_TIME.GUI_data[7]++;
			}
			else if(GUI_TIME.GUI_data[7]<40)
			{
				OLED_ShowNum(10,31,RTC_TIME.Hours+GUI_TIME.GUI_data[0],2,32);
				GUI_TIME.GUI_data[7]++;
			}
			else
				GUI_TIME.GUI_data[7]=0;
		}
		else if(GUI_TIME.KEYs[0]==2)//minutes 十位
		{
			OLED_ShowNum(10,31,RTC_TIME.Hours+GUI_TIME.GUI_data[0],2,32);
			OLED_ShowNum(86,31,RTC_TIME.Seconds+GUI_TIME.GUI_data[2],2,32);
			if(GUI_TIME.GUI_data[7]<20)
			{
				OLED_ShowString(48,31," ",32);
				GUI_TIME.GUI_data[7]++;
			}
			else if(GUI_TIME.GUI_data[7]<40)
			{
				OLED_ShowNum(48,31,RTC_TIME.Minutes+GUI_TIME.GUI_data[1],2,32);
				GUI_TIME.GUI_data[7]++;
			}
			else
				GUI_TIME.GUI_data[7]=0;
		}
		else if(GUI_TIME.KEYs[0]==3)//minutes 个位
		{
			OLED_ShowNum(10,31,RTC_TIME.Hours+GUI_TIME.GUI_data[0],2,32);
			OLED_ShowNum(86,31,RTC_TIME.Seconds+GUI_TIME.GUI_data[2],2,32);
			if(GUI_TIME.GUI_data[7]<20)
			{
				OLED_ShowString(64,31," ",32);
				GUI_TIME.GUI_data[7]++;
			}
			else if(GUI_TIME.GUI_data[7]<40)
			{
				OLED_ShowNum(48,31,RTC_TIME.Minutes+GUI_TIME.GUI_data[1],2,32);
				GUI_TIME.GUI_data[7]++;
			}
			else
				GUI_TIME.GUI_data[7]=0;
		}
		else if(GUI_TIME.KEYs[0]==4)//second	十位
		{
			OLED_ShowNum(10,31,RTC_TIME.Hours+GUI_TIME.GUI_data[0],2,32);
			OLED_ShowNum(48,31,RTC_TIME.Minutes+GUI_TIME.GUI_data[1],2,32);
			
			if(GUI_TIME.GUI_data[7]<20)
			{
				OLED_ShowString(86,31," ",32);
				GUI_TIME.GUI_data[7]++;
			}
			else if(GUI_TIME.GUI_data[7]<40)
			{
				OLED_ShowNum(86,31,RTC_TIME.Seconds+GUI_TIME.GUI_data[2],2,32);
				GUI_TIME.GUI_data[7]++;
			}
			else
				GUI_TIME.GUI_data[7]=0;
		}
		else if(GUI_TIME.KEYs[0]==5)//second	个位
		{
			OLED_ShowNum(10,31,RTC_TIME.Hours+GUI_TIME.GUI_data[0],2,32);
			OLED_ShowNum(48,31,RTC_TIME.Minutes+GUI_TIME.GUI_data[1],2,32);
			
			if(GUI_TIME.GUI_data[7]<20)
			{
				OLED_ShowString(102,31," ",32);
				GUI_TIME.GUI_data[7]++;
			}
			else if(GUI_TIME.GUI_data[7]<40)
			{
				OLED_ShowNum(86,31,RTC_TIME.Seconds+GUI_TIME.GUI_data[2],2,32);
				GUI_TIME.GUI_data[7]++;
			}
			else
				GUI_TIME.GUI_data[7]=0;
		}
		else
		{
		}
	}

/////////////////////////////////////按键处理部分GUI_data[6]为延时计数，KEY_state0~3对应按键1~4标识，KEY_state[4]是按键1延时标识
//////////////////////按键1
	if((KEY_GetPinState(1)==1)&&(GUI_TIME.KEY_state[0]==0)&&(GUI_TIME.KEY_state[4]==0)) //按键按下
	{
		GUI_TIME.KEY_state[0]=1;
	}
	else if((KEY_GetPinState(1)==0)&&(GUI_TIME.KEY_state[0]==1)&&(GUI_TIME.KEY_state[4]==0)&&GUI_TIME.GUI_state[1]==1)//按键放开
	{
		//////////////////////按键输出
		if(GUI_TIME.GUI_state[1]==1)
		{
			switch(GUI_TIME.KEYs[0])
			{
				case 0:
				{
					if(RTC_TIME.Hours>=10)
						RTC_TIME.Hours-=10;
					else
						RTC_TIME.Hours+=10;
					break;
				}
				case 1:
				{
					if(RTC_TIME.Hours>=1)
						RTC_TIME.Hours-=1;
					else
						RTC_TIME.Hours+=23;
					break;			
				
				}
				case 2:
				{
					if(RTC_TIME.Minutes>=10)
						RTC_TIME.Minutes-=10;
					else
						RTC_TIME.Minutes+=50;
					break;				
				}
				case 3:
				{
					if(RTC_TIME.Minutes>=1)
						RTC_TIME.Minutes-=1;
					else
						RTC_TIME.Minutes+=59;
					break;				
				}
				case 4:
				{
					if(RTC_TIME.Seconds>=10)
						RTC_TIME.Seconds-=10;
					else
						RTC_TIME.Seconds+=50;
					break;				
				}
				case 5:
				{
					if(RTC_TIME.Seconds>=1)
						RTC_TIME.Seconds-=1;
					else
						RTC_TIME.Seconds+=59;
					break;				
				}
				default:break;
			}
			HAL_RTC_SetTime(&hrtc,&RTC_TIME,RTC_FORMAT_BIN);
		}
		//////////////////////
		GUI_TIME.KEY_state[0]=0;
		GUI_TIME.GUI_data[6]=0;
	}
	else if((KEY_GetPinState(1)==1)&&(GUI_TIME.KEY_state[0]==1)&&(GUI_TIME.KEY_state[4]==0))//按键一直按下
	{
		GUI_TIME.GUI_data[6]++;
			if(GUI_TIME.GUI_data[6]>60)
			{
				//////////////////////长按按键输出
				if(GUI_TIME.GUI_state[1]==0)
					GUI_TIME.GUI_state[1]=1;
				else
					GUI_TIME.GUI_state[1]=0;
				//////////////////////
				GUI_TIME.GUI_data[6]=0;
				GUI_TIME.KEY_state[4]=1;
			}
	}
	else if((KEY_GetPinState(1)==0)&&(GUI_TIME.KEY_state[0]==1)&&(GUI_TIME.KEY_state[4]==1))//得到长按标识后放开
	{
		GUI_TIME.GUI_data[6]=0;
		GUI_TIME.KEY_state[4]=0;
		GUI_TIME.KEY_state[0]=0;		
	}
	if(GUI_TIME.GUI_state[1]==1)
	{
		//////////////////////按键2	
		if((KEY_GetPinState(2)==1)&&(GUI_TIME.KEY_state[1]==0))
		{
			GUI_TIME.KEY_state[1]=1;
		}
		else if((KEY_GetPinState(2)==0)&&(GUI_TIME.KEY_state[1]==1))
		{
			//////////////////////按键输出
			if(GUI_TIME.GUI_state[1]==1)
			{
				switch(GUI_TIME.KEYs[0])
				{
					case 0:
					{
						if(RTC_TIME.Hours<14)
							RTC_TIME.Hours+=10;
						else
							RTC_TIME.Hours-=10;
						break;
					}
					case 1:
					{
						if(RTC_TIME.Hours<23)
							RTC_TIME.Hours+=1;
						else
							RTC_TIME.Hours-=23;
						break;			
					}
					case 2:
					{
						if(RTC_TIME.Minutes<49)
							RTC_TIME.Minutes+=10;
						else
							RTC_TIME.Minutes-=50;
						break;				
					}
					case 3:
					{
						if(RTC_TIME.Minutes<59)
							RTC_TIME.Minutes+=1;
						else
							RTC_TIME.Minutes-=59;
						break;				
					}
					case 4:
					{
						if(RTC_TIME.Seconds<49)
							RTC_TIME.Seconds+=10;
						else
							RTC_TIME.Seconds-=50;
						break;				
					}
					case 5:
					{
						if(RTC_TIME.Seconds<59)
							RTC_TIME.Seconds+=1;
						else
							RTC_TIME.Seconds-=59;
						break;				
					}
					default:break;
				}
				HAL_RTC_SetTime(&hrtc,&RTC_TIME,RTC_FORMAT_BIN);
			}
			//////////////////////	
			GUI_TIME.KEY_state[1]=0;
		}
	//////////////////////按键3	
		if((KEY_GetPinState(3)==1)&&(GUI_TIME.KEY_state[2]==0))
		{
			GUI_TIME.KEY_state[2]=1;
		}
		else if((KEY_GetPinState(3)==0)&&(GUI_TIME.KEY_state[2]==1))
		{
			//////////////////////按键输出
			if(GUI_TIME.KEYs[0]==0)
				GUI_TIME.KEYs[0]=5;
			else
				GUI_TIME.KEYs[0]--;
			//////////////////////			
			GUI_TIME.KEY_state[2]=0;
		}
	//////////////////////按键4	
		if((KEY_GetPinState(4)==1)&&(GUI_TIME.KEY_state[3]==0))
		{
			GUI_TIME.KEY_state[3]=1;
		}
		else if((KEY_GetPinState(4)==0)&&(GUI_TIME.KEY_state[3]==1))
		{
			//////////////////////按键输出
			if(GUI_TIME.KEYs[0]==5)
				GUI_TIME.KEYs[0]=0;
			else
				GUI_TIME.KEYs[0]++;
			//////////////////////	
			GUI_TIME.KEY_state[3]=0;
		}
	}
}
void GUI_Alarm(void)
{
	if(GUI_ALARM.GUI_state[0]==0)//进入初始化
	{
		OLED_Clear();
		OLED_ShowString(0,0,"CLOCK",24);
		OLED_ShowChinese(66,5,7,16);
		OLED_ShowChinese(82,5,8,16);
		OLED_ShowChinese(98,5,9,16);
		OLED_ShowString(42,43,"-",12);

		OLED_ShowString(80,43,"-",12);

		OLED_ShowNum(10,31,GUI_ALARM.GUI_data[0],2,32);
		OLED_ShowNum(48,31,GUI_ALARM.GUI_data[1],2,32);
		OLED_ShowNum(86,31,GUI_ALARM.GUI_data[2],2,32);
		GUI_ALARM.GUI_state[0]=1;
	}
/////////////////////////////////////显示部分//////////////////////////////	
	if(GUI_ALARM.GUI_state[1]==0)
	{
		OLED_ShowNum(10,31,GUI_ALARM.GUI_data[0],2,32);
		OLED_ShowNum(48,31,GUI_ALARM.GUI_data[1],2,32);
		OLED_ShowNum(86,31,GUI_ALARM.GUI_data[2],2,32);	
	
	}
	else if(GUI_ALARM.GUI_state[1]==1)//时间设置
	{
		if(GUI_ALARM.KEYs[0]==0)		//hour 十位
		{
			OLED_ShowNum(48,31,GUI_ALARM.GUI_data[1],2,32);
			OLED_ShowNum(86,31,GUI_ALARM.GUI_data[2],2,32);
			if(GUI_ALARM.GUI_data[7]<20)
			{
				OLED_ShowString(10,31," ",32);
				GUI_ALARM.GUI_data[7]++;
			}
			else if(GUI_ALARM.GUI_data[7]<40)
			{
				OLED_ShowNum(10,31,GUI_ALARM.GUI_data[0],2,32);
				GUI_ALARM.GUI_data[7]++;
			}
			else
				GUI_ALARM.GUI_data[7]=0;
		}
		else if(GUI_ALARM.KEYs[0]==1)		//hour 个位
		{
			OLED_ShowNum(48,31,GUI_ALARM.GUI_data[1],2,32);
			OLED_ShowNum(86,31,GUI_ALARM.GUI_data[2],2,32);
			if(GUI_ALARM.GUI_data[7]<20)
			{
				OLED_ShowString(26,31," ",32);
				GUI_ALARM.GUI_data[7]++;
			}
			else if(GUI_ALARM.GUI_data[7]<40)
			{
				OLED_ShowNum(10,31,GUI_ALARM.GUI_data[0],2,32);
				GUI_ALARM.GUI_data[7]++;
			}
			else
				GUI_ALARM.GUI_data[7]=0;
		}
		else if(GUI_ALARM.KEYs[0]==2)//minutes 十位
		{
			OLED_ShowNum(10,31,GUI_ALARM.GUI_data[0],2,32);
			OLED_ShowNum(86,31,GUI_ALARM.GUI_data[2],2,32);
			if(GUI_ALARM.GUI_data[7]<20)
			{
				OLED_ShowString(48,31," ",32);
				GUI_ALARM.GUI_data[7]++;
			}
			else if(GUI_ALARM.GUI_data[7]<40)
			{
				OLED_ShowNum(48,31,GUI_ALARM.GUI_data[1],2,32);
				GUI_ALARM.GUI_data[7]++;
			}
			else
				GUI_ALARM.GUI_data[7]=0;
		}
		else if(GUI_ALARM.KEYs[0]==3)//minutes 个位
		{
			OLED_ShowNum(10,31,GUI_ALARM.GUI_data[0],2,32);
			OLED_ShowNum(86,31,GUI_ALARM.GUI_data[2],2,32);
			if(GUI_ALARM.GUI_data[7]<20)
			{
				OLED_ShowString(64,31," ",32);
				GUI_ALARM.GUI_data[7]++;
			}
			else if(GUI_ALARM.GUI_data[7]<40)
			{
				OLED_ShowNum(48,31,GUI_ALARM.GUI_data[1],2,32);
				GUI_ALARM.GUI_data[7]++;
			}
			else
				GUI_ALARM.GUI_data[7]=0;
		}
		else if(GUI_ALARM.KEYs[0]==4)//second	十位
		{
			OLED_ShowNum(10,31,GUI_ALARM.GUI_data[0],2,32);
			OLED_ShowNum(48,31,GUI_ALARM.GUI_data[1],2,32);
			
			if(GUI_ALARM.GUI_data[7]<20)
			{
				OLED_ShowString(86,31," ",32);
				GUI_ALARM.GUI_data[7]++;
			}
			else if(GUI_ALARM.GUI_data[7]<40)
			{
				OLED_ShowNum(86,31,GUI_ALARM.GUI_data[2],2,32);
				GUI_ALARM.GUI_data[7]++;
			}
			else
				GUI_ALARM.GUI_data[7]=0;
		}
		else if(GUI_ALARM.KEYs[0]==5)//second	个位
		{
			OLED_ShowNum(10,31,GUI_ALARM.GUI_data[0],2,32);
			OLED_ShowNum(48,31,GUI_ALARM.GUI_data[1],2,32);
			
			if(GUI_ALARM.GUI_data[7]<20)
			{
				OLED_ShowString(102,31," ",32);
				GUI_ALARM.GUI_data[7]++;
			}
			else if(GUI_ALARM.GUI_data[7]<40)
			{
				OLED_ShowNum(86,31,GUI_ALARM.GUI_data[2],2,32);
				GUI_ALARM.GUI_data[7]++;
			}
			else
				GUI_ALARM.GUI_data[7]=0;
		}
		else
		{
		}
	}

/////////////////////////////////////按键处理部分GUI_data[6]为延时计数，KEY_state0~3对应按键1~4标识，KEY_state[4]是按键1延时标识
//////////////////////按键1
	if((KEY_GetPinState(1)==1)&&(GUI_ALARM.KEY_state[0]==0)&&(GUI_ALARM.KEY_state[4]==0)) //按键按下
	{
		GUI_ALARM.KEY_state[0]=1;
	}
	else if((KEY_GetPinState(1)==0)&&(GUI_ALARM.KEY_state[0]==1)&&(GUI_ALARM.KEY_state[4]==0)&&GUI_ALARM.GUI_state[1]==1)//按键放开
	{
		//////////////////////按键输出
		if(GUI_ALARM.GUI_state[1]==1)
		{
			switch(GUI_ALARM.KEYs[0])
			{
				case 0:
				{
					if(GUI_ALARM.GUI_data[0]>=10)
						GUI_ALARM.GUI_data[0]-=10;
					else
						GUI_ALARM.GUI_data[0]+=10;
					break;
				}
				case 1:
				{
					if(GUI_ALARM.GUI_data[0]>=1)
						GUI_ALARM.GUI_data[0]-=1;
					else
						GUI_ALARM.GUI_data[0]+=23;
					break;			
				
				}
				case 2:
				{
					if(GUI_ALARM.GUI_data[1]>=10)
						GUI_ALARM.GUI_data[1]-=10;
					else
						GUI_ALARM.GUI_data[1]+=50;
					break;				
				}
				case 3:
				{
					if(GUI_ALARM.GUI_data[1]>=1)
						GUI_ALARM.GUI_data[1]-=1;
					else
						GUI_ALARM.GUI_data[1]+=59;
					break;				
				}
				case 4:
				{
					if(GUI_ALARM.GUI_data[2]>=10)
						GUI_ALARM.GUI_data[2]-=10;
					else
						GUI_ALARM.GUI_data[2]+=50;
					break;				
				}
				case 5:
				{
					if(GUI_ALARM.GUI_data[2]>=1)
						GUI_ALARM.GUI_data[2]-=1;
					else
						GUI_ALARM.GUI_data[2]+=59;
					break;				
				}
				default:break;
			}
		}
		//////////////////////
		GUI_ALARM.KEY_state[0]=0;
		GUI_ALARM.GUI_data[6]=0;
	}
	else if((KEY_GetPinState(1)==1)&&(GUI_ALARM.KEY_state[0]==1)&&(GUI_ALARM.KEY_state[4]==0))//按键一直按下
	{
		GUI_ALARM.GUI_data[6]++;
			if(GUI_ALARM.GUI_data[6]>60)
			{
				//////////////////////长按按键输出
				if(GUI_ALARM.GUI_state[1]==0)
					GUI_ALARM.GUI_state[1]=1;
				else
					GUI_ALARM.GUI_state[1]=0;
				//////////////////////
				GUI_ALARM.GUI_data[6]=0;
				GUI_ALARM.KEY_state[4]=1;
			}
	}
	else if((KEY_GetPinState(1)==0)&&(GUI_ALARM.KEY_state[0]==1)&&(GUI_ALARM.KEY_state[4]==1))//得到长按标识后放开
	{
		GUI_ALARM.GUI_data[6]=0;
		GUI_ALARM.KEY_state[4]=0;
		GUI_ALARM.KEY_state[0]=0;		
	}
	if(GUI_ALARM.GUI_state[1]==1)
	{		
	//////////////////////按键2	
		if((KEY_GetPinState(2)==1)&&(GUI_ALARM.KEY_state[1]==0))
		{
			GUI_ALARM.KEY_state[1]=1;
		}
		else if((KEY_GetPinState(2)==0)&&(GUI_ALARM.KEY_state[1]==1))
		{
			//////////////////////按键输出
			if(GUI_ALARM.GUI_state[1]==1)
			{
				switch(GUI_ALARM.KEYs[0])
				{
					case 0:
					{
						if(GUI_ALARM.GUI_data[0]<14)
							GUI_ALARM.GUI_data[0]+=10;
						else
							GUI_ALARM.GUI_data[0]-=10;
						break;
					}
					case 1:
					{
						if(GUI_ALARM.GUI_data[0]<23)
							GUI_ALARM.GUI_data[0]+=1;
						else
							GUI_ALARM.GUI_data[0]-=23;
						break;			
					}
					case 2:
					{
						if(GUI_ALARM.GUI_data[1]<49)
							GUI_ALARM.GUI_data[1]+=10;
						else
							GUI_ALARM.GUI_data[1]-=50;
						break;				
					}
					case 3:
					{
						if(GUI_ALARM.GUI_data[1]<59)
							GUI_ALARM.GUI_data[1]+=1;
						else
							GUI_ALARM.GUI_data[1]-=59;
						break;				
					}
					case 4:
					{
						if(GUI_ALARM.GUI_data[2]<49)
							GUI_ALARM.GUI_data[2]+=10;
						else
							GUI_ALARM.GUI_data[2]-=50;
						break;				
					}
					case 5:
					{
						if(GUI_ALARM.GUI_data[2]<59)
							GUI_ALARM.GUI_data[2]+=1;
						else
							GUI_ALARM.GUI_data[2]-=59;
						break;				
					}
					default:break;
				}
			}
			//////////////////////	
			GUI_ALARM.KEY_state[1]=0;
		}
	//////////////////////按键3	
		if((KEY_GetPinState(3)==1)&&(GUI_ALARM.KEY_state[2]==0))
		{
			GUI_ALARM.KEY_state[2]=1;
		}
		else if((KEY_GetPinState(3)==0)&&(GUI_ALARM.KEY_state[2]==1))
		{
			//////////////////////按键输出
			if(GUI_ALARM.KEYs[0]==0)
				GUI_ALARM.KEYs[0]=5;
			else
				GUI_ALARM.KEYs[0]--;
			//////////////////////			
			GUI_ALARM.KEY_state[2]=0;
		}
	//////////////////////按键4	
		if((KEY_GetPinState(4)==1)&&(GUI_ALARM.KEY_state[3]==0))
		{
			GUI_ALARM.KEY_state[3]=1;
		}
		else if((KEY_GetPinState(4)==0)&&(GUI_ALARM.KEY_state[3]==1))
		{
			//////////////////////按键输出
			if(GUI_ALARM.KEYs[0]==5)
				GUI_ALARM.KEYs[0]=0;
			else
				GUI_ALARM.KEYs[0]++;
			//////////////////////	
			GUI_ALARM.KEY_state[3]=0;
		}
	}
}
void GUI_Temperature(void)
{
	if(GUI_TEMPERATURE.GUI_state[0]==0)
	{
		OLED_Clear();
		//Name
		OLED_ShowChinese(0,0,3,16);
		OLED_ShowChinese(16,0,4,16);
		OLED_ShowString(35,0,":",16);
		OLED_ShowChinese(42,0,0,16);
		OLED_ShowChinese(58,0,1,16);
		OLED_ShowChinese(74,0,2,16);
		//Num
		OLED_ShowChinese(0,16,5,16);
		OLED_ShowChinese(16,16,6,16);
		OLED_ShowString(35,16,":",16);
		OLED_ShowNum(42,18,20200509,8,12);
		OLED_ShowNum(90,18,3005,5,12);
		OLED_ShowString(0,30,"Volt:",16);
		OLED_ShowString(0,46,"Temp:",16);
		OLED_ShowString(72,46,"`C",16);
		OLED_ShowString(72,30,"mv",16);
		GUI_TEMPERATURE.GUI_state[0]=1;			
	}
	if(GUI_TEMPERATURE.GUI_data[7]>10000)
	{
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,10);
		GUI_TEMPERATURE.GUI_data[0]=HAL_ADC_GetValue(&hadc1);

		HAL_I2C_Master_Transmit(&hi2c1, LM75A_ADDRESS , 0x00, 1, HAL_MAX_DELAY);  // 发送读取温度的命令
		HAL_I2C_Master_Receive(&hi2c1, LM75A_ADDRESS, tempData, 2, HAL_MAX_DELAY);  // 接收温度数据
		temperature = (tempData[0] << 8) | tempData[1];  // 将两个字节的温度数据合并为一个16位有符号整数
		temperature = temperature >> 5;  // 温度数据占用12位，将高5位清零

		if (temperature & 0x0400)  // 最高位为1表示负温度
		{
			temperature |= 0xF800;  // 将高11位扩展为16位有符号数
			/*
			在LM75A温度传感器中，温度值使用12位表示，其中最高位是符号位，表示正负温度。如果最高位为1，则表示负温度；如果最高位为0，则表示正温度。
			在代码中，如果温度数据的最高位为1（表示负温度），则需要将高11位扩展为16位有符号数，以保持负温度的正确表示。具体而言，通过将高11位（位7到位0）的值全部设置为1，将扩展后的高11位与原始的温度数据进行按位或操作，以获得一个16位的有符号整数，其中高11位是扩展的负温度值，低5位是原始温度数据的低5位。
			这样做的目的是确保负温度值在转换后保持正确的负号和数值范围。
			*/
		}
		else
		temperatureC = temperature * 0.125;
		
		OLED_ShowNum(40,30,GUI_TEMPERATURE.GUI_data[0]*3300/4095,4,16);
		OLED_ShowNum_Sign(40,46,temperatureC,4,16);
		GUI_TEMPERATURE.GUI_data[7]=0;
	}
		else
		GUI_TEMPERATURE.GUI_data[7]++;
}

void GUI_Init(void)
{
//	int a = 0;
//	int n = 0;
	OLED_Clear();

//		for(n=0;n<1;n++)
//		{
//				for(a=0;a<25;a++)
//				{
//					OLED_ShowString(a,10,"Starting",16);
//					delay_ms(50);
//					OLED_Clear();
//				}
//				for(a=25;a<50;a++)
//				{
//					OLED_ShowString(50-a,10,"Starting",16);
//					delay_ms(50);
//					OLED_Clear();
//				}
//				a=0;
//		}
	
	OLED_Circle(45,24,24);
	delay_ms(500);	
	OLED_Circle(30,40,24);
	delay_ms(500);
	OLED_Circle(60,40,24);
	delay_ms(500);
	OLED_Clear();

}	

///////////////////////////beep tone//////////////////////////////
void beep_tone(u32 frequency, u32 duration_ms) 
{
  uint32_t period_us = 1000000 / (2 * frequency);  // 计算半周期的时间（以微秒为单位）
  uint32_t num_cycles = (duration_ms * 1000) / (2 * period_us);  // 计算总周期数

  for (uint32_t i = 0; i < num_cycles; i++) {
    HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);
    delay_us(period_us);
    HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
    delay_us(period_us);
  }
}
void beep_music(void)
{
							beep_tone(255, 500);  // C
							delay_ms(50);
							beep_tone(255, 500);  // C
							delay_ms(50);
							beep_tone(392, 500);     // G
							delay_ms(50);
							beep_tone(392, 500);     // G
							delay_ms(50);
							beep_tone(440, 500);     // A
							delay_ms(50);
							beep_tone(440, 500);     // A
							delay_ms(50);
							beep_tone(392, 500);     // G
							delay_ms(500);

							beep_tone(349, 500);  // F
							delay_ms(50);
							beep_tone(349, 500);  // F
							delay_ms(50);
							beep_tone(329, 500);  // E
							delay_ms(50);
							beep_tone(329, 500);  // E
							delay_ms(50);
							beep_tone(293, 500);  // D
							delay_ms(50);
							beep_tone(293, 500);  // D
							delay_ms(50);
							beep_tone(261, 500);  // C
							delay_ms(500);

							beep_tone(392, 500);     // G
							delay_ms(50);
							beep_tone(392, 500);     // G
							delay_ms(50);
							beep_tone(349, 500);  // F
							delay_ms(50);
							beep_tone(349, 500);  // F
							delay_ms(50);
							beep_tone(329, 500);  // E
							delay_ms(50);
							beep_tone(329, 500);  // E
							delay_ms(50);
							beep_tone(293, 500);  // D
							delay_ms(500);

							beep_tone(392, 500);     // G
							delay_ms(50);
							beep_tone(392, 500);     // G
							delay_ms(50);
							beep_tone(349, 500);  // F
							delay_ms(50);
							beep_tone(349, 500);  // F
							delay_ms(50);
							beep_tone(329, 500);  // E
							delay_ms(50);
							beep_tone(329, 500);  // E
							delay_ms(50);
							beep_tone(293, 500);  // D
							delay_ms(500);

							beep_tone(255, 500);  // C
							delay_ms(50);
							beep_tone(255, 500);  // C
							delay_ms(50);
							beep_tone(392, 500);     // G
							delay_ms(50);
							beep_tone(392, 500);     // G
							delay_ms(50);
							beep_tone(440, 500);     // A
							delay_ms(50);
							beep_tone(440, 500);     // A
							delay_ms(50);
							beep_tone(392, 500);     // G
							delay_ms(500);

							beep_tone(349, 500);  // F
							delay_ms(50);
							beep_tone(349, 500);  // F
							delay_ms(50);
							beep_tone(329, 500);  // E
							delay_ms(50);
							beep_tone(329, 500);  // E
							delay_ms(50);
							beep_tone(293, 500);  // D
							delay_ms(50);
							beep_tone(293, 500);  // D
							delay_ms(50);
							beep_tone(261, 500);  // C
							delay_ms(500);
}
/////////////////////////时间戳///////////////////////////


