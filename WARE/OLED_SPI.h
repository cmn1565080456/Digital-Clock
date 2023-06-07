#ifndef __OLED_SPI_H
#define __OLED_SPI_H

#include "sys.h"
#include "delay.h"
#include "gpio.h"
#include "spi.h"

extern RTC_DateTypeDef RTC_DATE;
extern RTC_TimeTypeDef RTC_TIME;

#define OLED_RES_Reset() HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin,GPIO_PIN_RESET)//RES
#define OLED_RES_Set() HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin,GPIO_PIN_SET)
 
#define OLED_DC_Reset() HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,GPIO_PIN_RESET)//DC
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,GPIO_PIN_SET)
 		     
#define OLED_CS_Reset()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin,GPIO_PIN_RESET)//CS
#define OLED_CS_Set()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin,GPIO_PIN_SET)

///////////////////////////////�Լ�дSPI��ʱ����
//#define OLED_D0_Reset()  HAL_GPIO_WritePin(OLED_D0_GPIO_Port,OLED_D0_Pin,GPIO_PIN_RESET)//D0
//#define OLED_D0_Set()  HAL_GPIO_WritePin(OLED_D0_GPIO_Port,OLED_D0_Pin,GPIO_PIN_SET)

//#define OLED_D1_Reset()  HAL_GPIO_WritePin(OLED_D1_GPIO_Port,OLED_D1_Pin,GPIO_PIN_RESET)//D1
//#define OLED_D1_Set()  HAL_GPIO_WritePin(OLED_D1_GPIO_Port,OLED_D1_Pin,GPIO_PIN_SET)

#define OLED_WIDE 128
#define OLED_HIGH 64
 
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����
/////////////////////////////////////////////////////////////////////////////��������
void OLED_Init(void);							//OLED��Ļ��ʼ������
void OLED_WriteByte(u8 data,u8 mode);			//���ݻ�����Ĵ��䣬mode��ѡ�������ݻ�������
void OLED_Refresh(void);						//�����Դ浽��ʾ��
void OLED_Clear(void);							//ȫ������
void OLED_ColorTurn(uint8_t i);					//��ɫ��ʾ
void OLED_DisplayTurn(uint8_t i);				//��ת180��
void OLED_DisPlay_On(void);						//������ʾ
void OLED_DisPlay_Off(void);					//�ر���ʾ
void OLED_nPoint_NW(u8 x,u8 y);					//��ĳ��дΪ0���Դ棩
void OLED_Point_NW(u8 x,u8 y);					//��ĳ��дΪ1���Դ棩
void OLED_nPoint(u8 x,u8 y);					//��ĳ��дΪ0
void OLED_Point(u8 x,u8 y);						//��ĳ��дΪ1

/////////////////////////////////////////////////////////////////////////////��չ����
void OLED_Circle(u8 x,u8 y,u8 r);								//��Բ
u32 OLED_Pow(u8 m,u8 n);										//�ݺ���
void OLED_ShowNum_Sign(u8 x,u8 y, int num,u8 len,u8 size);	//д�����֣�ǰ�治��0��
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1);			//д������
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1);					//��ʾ����
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1);				//��ʾ�ַ���
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1);				//��ʾ����
////////////////////////////////////////////////////////////////////////////GUI����
typedef struct
{
	u8 GUI_state[8];					//��¼GUI����ģʽ
	u8 KEY_state[8];					//��¼����״̬
	u8 KEYs[8];							//��¼������ʶ
	u32 GUI_data[8];					//GUI�ڲ����ݴ洢
	
}GUI;

extern GUI GUI_TIME;
extern GUI GUI_ALARM;
extern GUI GUI_TEMPERATURE;

void GUI_Time(void);						//��ʾʱ�����
void GUI_Alarm(void);						//��ʾ���ӽ���
void GUI_Temperature(void);					//��ʾ�¶Ƚ���





///////////////////////////////////////////////////////////////////////////����������/////////////////
void beep_tone(u32  frequency, u32  duration_ms);//��������������
void beep_music();

/////////////////////////ʱ���/////////////////
//struct tm {
//    int tm_sec;   /* seconds after the minute, 0 to 60
//                     (0 - 60 allows for the occasional leap second) */
//    int tm_min;   /* minutes after the hour, 0 to 59 */
//    int tm_hour;  /* hours since midnight, 0 to 23 */
//    int tm_mday;  /* day of the month, 1 to 31 */
//    int tm_mon;   /* months since January, 0 to 11 */
//    int tm_year;  /* years since 1900 */
//    int tm_wday;  /* days since Sunday, 0 to 6 */
//    int tm_yday;  /* days since January 1, 0 to 365 */
//    int tm_isdst; /* Daylight Savings Time flag */
//    union {       /* ABI-required extra fields, in a variety of types */
//        struct {
//            int __extra_1, __extra_2;
//        };
//        struct {
//            long __extra_1_long, __extra_2_long;
//        };
//        struct {
//            char *__extra_1_cptr, *__extra_2_cptr;
//        };
//        struct {
//            void *__extra_1_vptr, *__extra_2_vptr;
//        };
//    };
//};
#endif
