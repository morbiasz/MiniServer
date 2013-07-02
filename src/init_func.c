#include "init_func.h"
#include "stm32f10x.h"
#include <stm32f10x_tim.h>
#include <server_config.h>

//Configures output/input directions
void GPIO_Config(void) {

	//STM32 Discovery leds: green( Pin_9) and blue (Pin_8)
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//STM32 Discovery onboard button
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//CS dla karty i enc28j60
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	#if RGB_DIODE
	//Pin dla Channel 3,2,1 PWM TIM4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	#endif
	
	#if RC5_CONTROL
	//Pin for RC5 interrupt
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	#endif
	
	//Also PC10 is used for 1Wire

}

//PLL configuration and clock distribution
void RCC_Conf(void) {
  ErrorStatus HSEStartUpStatus;	

  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------*/    
  /* RCC system reset(for debug purpose) */ 
  RCC_DeInit(); 
 
  /* Enable HSE */ 
  RCC_HSEConfig(RCC_HSE_ON); 
 
  /* Wait till HSE is ready */ 
  HSEStartUpStatus = RCC_WaitForHSEStartUp(); 
 
  if (HSEStartUpStatus == SUCCESS) 
  { 

  	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	FLASH_SetLatency(FLASH_Latency_0);

    /* HCLK = SYSCLK */ 
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  
   
    /* PCLK2 = HCLK */ 
    RCC_PCLK2Config(RCC_HCLK_Div1);  	  							// APB2, max 72Mhz
 
    /* PCLK1 = HCLK */ 
    RCC_PCLK1Config(RCC_HCLK_Div1); 	   							//APB1, max 36Mhz
     
    /* PLLCLK = (8MHz/2) * 6 = 24 MHz */ 
    RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div2); 
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6); 
 
    /* Enable PLL */  
    RCC_PLLCmd(ENABLE); 
 
    /* Wait till PLL is ready */ 
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); 
 
    /* Select PLL as system clock source */ 
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
 
    /* Wait till PLL is used as system clock source */ 
    while (RCC_GetSYSCLKSource() != 0x08); 
 
  } 
  else 
  { 
    while (1) { ; }	 

  }

  /* list of used peripherals */

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	// Delay_us
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	// RC5
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	// SERVO & RGB_DIODE
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
  /*
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	*/
	
	// ENC28j60
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	// Karta SD
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);


}


/* Configures Nested Vectored Interrupt Controller */
void NVIC_Config(void) {

	NVIC_InitTypeDef NVIC_InitStructure;
	
	#if RC5_CONTROL
	EXTI_InitTypeDef EXTI_InitStructure;
	#endif
	
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	
	#if RC5_CONTROL
	//ustwienie dzialania GPIO jako linii EXTI
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource7);

	// --- Konfiguracja przerwania dla RC5  ----- //
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	EXTI_ClearITPendingBit(EXTI_Line5);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; 		
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Timer uzywany przy dekodowaniu RC5	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	#endif

// -----------------------------------------------//

//  ---- Uzywane przez interpreter polecen ------ //

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
// ----------------------------------------------//

// ------------- Zegar RTC ---------------------//

	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;				//przerwanie zegara RTC, co jedna sekundy wywo³ywane
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
// ----------------------------------------------//	
	
}

// Konfiguracja kanalu TIM4 dla potrzeb sterowania serwomechanizmem: f = 50Hz, regulowane wypelnienie
void pwmSetup(void) {

     TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct; 

		 //Set to 50Hz frequency, pulse width = (TIM_Pulse/2000)*2ms
     TIM_TimeBaseStructInit( &TIM_TimeBaseInitStruct ); 
     TIM_TimeBaseInitStruct.TIM_Period = 2000; 
     TIM_TimeBaseInitStruct.TIM_Prescaler = 24;			//240 dla 50Hz
	 TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	 TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
     TIM_TimeBaseInit( TIM4, &TIM_TimeBaseInitStruct ); 
  
     TIM_Cmd( TIM4, ENABLE );

}

// Konfiguracja kanalow TIM4 dla potrzeb sterowania dioda RGB
void rgbPwmSetup(void)
{
		TIM_OCInitTypeDef TIM_OCInitStruct; 
	
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);	

// Kanal 3	
		TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStruct.TIM_Pulse = 1000;
		TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	
		TIM_OC3Init( TIM4, &TIM_OCInitStruct );
		TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);


// Kanal 2	
		TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStruct.TIM_Pulse = 1000;
		TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	
		TIM_OC2Init( TIM4, &TIM_OCInitStruct );
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);


// Kanal 1
		TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStruct.TIM_Pulse = 1000;
		TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	
		TIM_OC1Init( TIM4, &TIM_OCInitStruct );
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

		TIM_ARRPreloadConfig(TIM4, ENABLE);
	
}

caddr_t _sbrk (int size)
{
   extern char __heap_start;
   extern char __heap_end;
   static char *current_heap_end = &__heap_start;
   char *previous_heap_end;

   previous_heap_end = current_heap_end;

   if (current_heap_end + size > &__heap_end)
   {
      errno = ENOMEM;
      return (caddr_t) -1;
   }

   current_heap_end += size;

   return (caddr_t) previous_heap_end;
}




