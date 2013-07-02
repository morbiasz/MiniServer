#include "rc5.h"

volatile uint16_t rc5 = 0;
volatile uint16_t rc5_ok = 0;

void RC5Check(void)
{
		static uint8_t rc5_toogle = 2;
		static uint8_t rc5_toogle_last = 2;
		uint16_t rc5_code = 0;
	
		//sprawdzenie flagi odbioru danych
		if(rc5_ok != 0)
		{
			//wykonanie kopii zdekodowanego kodu RC5
			rc5_code = rc5;

			//sprawdzenie bitu toggle
			if((rc5_code & 0x0800) == 0)
				{rc5_toogle = 0;}
			else
				{rc5_toogle = 1;}

			//wykonanie kodu jeli wcisnieto "nowy" klawisz
			if(rc5_toogle != rc5_toogle_last)
			{
				//aktualizacja poprzedniej flagi toogle
				rc5_toogle_last = rc5_toogle;
				
				switch (rc5_code & 0x003F)
				{
					case RC5_Code_1 :
						GPIOC->ODR ^=GPIO_Pin_9;

						#if RC5_UART_DEBUG
							min_printf("RC5: 1\r\n")
						#endif

						break;
					
					case RC5_Code_2 :
						GPIOC->ODR ^=GPIO_Pin_8;

						#if RC5_UART_DEBUG
							min_printf("RC5: 2\r\n")
						#endif

						break;
				
					default:
						break;
				
				}
			
			}	
		}
		
}

void RC5Timer_Config(void)
{
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		TIM_OCInitTypeDef TIM_OCInitStructure;

		TIM_TimeBaseStructure.TIM_Period = 65535;
		TIM_TimeBaseStructure.TIM_Prescaler = 467;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 150;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

		TIM_OC1Init(TIM3, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
		TIM_ARRPreloadConfig(TIM3, ENABLE);

		TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

		TIM_Cmd(TIM3, ENABLE);

}
