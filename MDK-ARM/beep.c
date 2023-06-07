#include "beep.h"


void beep_tone(uint32_t frequency, uint32_t duration_ms) {
  uint32_t period_us = 1000000 / (2 * frequency);  // ��������ڵ�ʱ�䣨��΢��Ϊ��λ��
  uint32_t num_cycles = (duration_ms * 1000) / (2 * period_us);  // ������������

  for (uint32_t i = 0; i < num_cycles; i++) {
    HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);
    delay_us(period_us);
    HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
    delay_us(period_us);
  }
}


// ��������������ʵ��
// ...


