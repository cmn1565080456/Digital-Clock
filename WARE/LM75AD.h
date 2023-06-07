#ifndef __LM75AD_H
#define __LM75AD_H	 
#include "main.h"
#include "sys.h"
#define LM75AD_ADDR 0x90   //Ä¬ÈÏµØÖ·0x90
 
void LM75AD_Init(void);
void LM75_Temp(uint8_t *temp);
		 				    
#endif

