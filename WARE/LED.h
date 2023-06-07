#ifndef __LED_H
#define __LED_H
#include "sys.h"
#include "delay.h"

void LED_State(u8 led ,u8 state);	//控制LED亮灭；led为LED编号：1~4，state为状态：0为灭，1为亮

#endif
