#include "delay.h"
#include <stm32f10x_tim.h>



static __IO uint32_t TimingDelay;

volatile uint32_t timerCounter = 0UL;
volatile uint32_t arptimer = 0UL;

#define SYS_CLK 24000000    /* 24 MHz */
#define DELAY_TIM_FREQUENCY 1000000 /* = 1MHZ -> timer runs in microseconds */

void Delay_config(void) {

	if (SysTick_Config(24000))	//ms
	  {
	    while (1);
	  }

}

void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

void Delay_us_init() {

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_ClockDivision	= 0;
	TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period	= UINT16_MAX;
	TIM_TimeBaseStructure.TIM_Prescaler	= (SYS_CLK / DELAY_TIM_FREQUENCY) - 1;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_Cmd(TIM2, ENABLE);
}

/* wait busy loop, microseconds */
void Delay_us( uint16_t us )
{
	TIM2->CNT = 0;
	/* use 16 bit count wrap around */
	while((uint16_t)(TIM2->CNT) <= us);
}
