
#ifndef DELAY_H_
#define DELAY_H_

#include "stm32f10x.h"

void Delay_config(void);
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

void Delay_us( uint16_t us );
void Delay_us_init(void);

#endif /* DELAY_H_ */
