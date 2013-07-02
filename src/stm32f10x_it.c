/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stm32f10x_usart.h>
#include <stm32f10x_rtc.h>
#include <stm32f10x_pwr.h>
#include <stm32f10x_bkp.h>
#include "stm32f10x_it.h"
#include "1wire/temp_read.h"
#include <server_config.h>
#include "../SERVER/serverWWW.h"
#include "delay.h"
#include "usart.h"
#include <ff_diskio.h>

#if CMD_LINE
#include "ringbuffer.h"
extern volatile uint8_t command_request;
#endif

#if RC5_CONTROL

#define IR_Input ((GPIOC->IDR & GPIO_Pin_7)?(1):(0))
volatile uint16_t licznik1 = 0;
volatile uint8_t stan = 0;
extern volatile uint16_t rc5;
extern volatile uint8_t rc5_ok;
#endif

#if TEMP_READ
volatile char temp[3][5];
#endif

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

#if CMD_LINE
void USART1_IRQHandler(void)
{
	 char c; 
	 static int8_t line_length = 0;
	
	 //odbierz dane
	 c = USART1->DR & 0x1FF;

	 // jezeli wcisneto backspace wyzeruj ostatnia pozycje za pomoca spacji
	 if ( c == 0x08 && line_length > 0 )
	 {
		  line_length--;
		 
		 	USART1->DR = ( 0x08 & 0x1FF);
			while (!(USART1->SR & USART_FLAG_TXE));
		 
			USART1->DR = ( 0x20 & 0x1FF);
			while (!(USART1->SR & USART_FLAG_TXE));
		
			USART1->DR = ( 0x08 & 0x1FF);
			while (!(USART1->SR & USART_FLAG_TXE));

			// cofnij karetke
			decrement_end();
		 
			return;
	 }
	 else if ( c == 0x08 && line_length == 0 )		//wykrywanie pustych wierszy
	 {
			return;
	 }
	 else if ( c== '\r' && line_length == 0 )		//wykrywanie konca linii
	 {
			usart_printf("\rMiniServer console#>");
			return;		 
	 }
	 //jezeli wcisnieto enter wystaw flage oczekujacej komendy
	 else if (c == '\r' && line_length > 0)
	 {
			command_request++;
			line_length = 0;
		 	 //odeslij znak tak aby byl widoczny na ekranie
			USART1->DR = ( c & 0x1FF);
			while (!(USART1->SR & USART_FLAG_TXE));
		 	ring_buffer_push(c);
			return;
	 }
	 
	 //jezeli przekroczyl maksymalna dlugosc
	 if ( line_length > 19 )
		 return;
	 
	 //odeslij znak tak aby byl widoczny na ekranie
	 USART1->DR = ( c & 0x1FF);
	 while (!(USART1->SR & USART_FLAG_TXE));		//czekaj na wyslanie
	
	 ring_buffer_push(c);							//jezeli wszystko ok, wrzuc znak do bufora
	 line_length++;

			 
}
#endif

//przerwanie, uzywane do generacji opoznien w ms
void SysTick_Handler(void)
{
	static uint32_t ff_clock = 0;

		ff_clock++;	// co kazda ms inkrementuj
		if ( (ff_clock % 10) == 0)
			disk_timerproc();

	//uzywane przez funkcje Delay( int ) z "delay.c"
	TimingDelay_Decrement();

}

// obsluga kodu RC5
#if RC5_CONTROL
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line7) != RESET)
	{
		//zapis stanu licznika oraz jego wyzerowanie
		licznik1 = TIM_GetCounter(TIM3);
		TIM_SetCounter(TIM3, 0);
 
		//odbioru pierwszego bitu
		if((stan == 0) && (IR_Input == 0))
		{
			//wyzerowanie oraz w³¹czenie licznika
			TIM_SetCounter(TIM3, 0);
			TIM_Cmd(TIM3, ENABLE);
			//ustawienie wartoœci pocz¹tkowych
			stan = 1;
			rc5 = 0;
			rc5_ok = 0;
		}
		else if(stan > 0)
		{
			//aktualizacja stanu w którym znajduje sie dekodowanie rc5
			//wraz ze sprawdzaniem czy czas trwania impulsu jest poprawny
			if((licznik1 > (75)) && (licznik1 < (105)))
				{stan += 2;}
			else if((licznik1 > (35)) && (licznik1 < (55)))
				{stan += 1;}
				
			//w przypadku b³êdnego czasu trwania stanu niskiego
			//lub wysokiego nastêpuje wyzerowanie zmiennych oraz
			//zablokowanie dalszego dekodowania
			else
			{
				//wy³¹czenie licznika
				TIM_Cmd(TIM3, DISABLE);
				TIM_SetCounter(TIM3, 0);
				//b³êdne zakoñczenie odbioru ramki kodu RC5
				stan = 0;
				rc5 = 0;
				rc5_ok = 0;
			}
		}
		else
		{
			//wy³¹czenie licznika
			TIM_Cmd(TIM3, DISABLE);
			TIM_SetCounter(TIM3, 0);
			//b³êdne zakoñczenie odbioru ramki kodu RC5
			stan = 0;
			rc5 = 0;
			rc5_ok = 0;
		}
		
		switch(stan)
		{
			case 27 :
				//ostatnie próbkowanie po zboczu synchronizacji
				if(IR_Input == 0)
					{rc5 |= 0x01;}
				//wy³¹czenie licznika
				TIM_Cmd(TIM3, DISABLE);
				TIM_SetCounter(TIM3, 0);
				//udan zakoñczenie odbioru ramki kodu RC5
				rc5_ok = 1;
				stan = 0;
			break;
			//próbkowanie stanów chwile po zboczu synchronizacji
			case 25 : if(IR_Input == 0){rc5 |= 0x0002;} break;
			case 23 : if(IR_Input == 0){rc5 |= 0x0004;} break;
			case 21 : if(IR_Input == 0){rc5 |= 0x0008;} break;
			case 19 : if(IR_Input == 0){rc5 |= 0x0010;} break;
			case 17 : if(IR_Input == 0){rc5 |= 0x0020;} break;
			case 15 : if(IR_Input == 0){rc5 |= 0x0040;} break;
			case 13 : if(IR_Input == 0){rc5 |= 0x0080;} break;
			case 11 : if(IR_Input == 0){rc5 |= 0x0100;} break;
			case 9 : if(IR_Input == 0){rc5 |= 0x0200;} break;
			case 7 : if(IR_Input == 0){rc5 |= 0x0400;} break;
			case 5 : if(IR_Input == 0){rc5 |= 0x0800;} break;
			case 3 : if(IR_Input == 0){rc5 |= 0x1000;} break;
			case 1 : if(IR_Input == 0){rc5 |= 0x2000;} break;
			default : break;
		}
		//wyzerowanie flagi przerwania
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
	{

		TIM_Cmd(TIM3, DISABLE);
		TIM_SetCounter(TIM3, 0);

		rc5 = 0;
		rc5_ok = 0;
		stan = 0;

		TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

	}
}
#endif

void RTC_IRQHandler(void)
{
	
	  NVIC_ClearPendingIRQ(RTC_IRQn);
	  RTC_ClearITPendingBit(RTC_IT_SEC);

	 	www_clock_tick();

	 	GPIOC->ODR ^= GPIO_Pin_9;
	
	  	//Jezeli minelo 60 sekund, zrob request odczytania temperatury
	    //Nastêpnie steruj automatem odczytu temperatury
		if (RTC_GetCounter() % TEMP_UPDATE_TIME == 0)
		{
			ustawStanOdczytywaniaTemperatury(READ_REQUEST);
		}
		if (odczytajStanOdczytywaniaTemperatury() == WAITING)
		{
			ustawStanOdczytywaniaTemperatury(CONVERSION_FINISHED);
		}
		
		//Po uplywie ustalonego czasu wyslij request aktualizacji IP na zewnetrznym serwerze
		if (RTC_GetCounter() % IP_UPDATE_TIME == 0)
			updateIP();
		
		//Po uplywie ustalonego czasu uaktualnij czas z serwera NTP
		if (RTC_GetCounter() % NTP_UPDATE_TIME == 0)
		{	
			resetNTPClient();
		}

		// Gdy minie dzien pracy:
		if(RTC_GetCounter() == 86399)
		{
			//request zaznaczenia w logu temperatury ¿e min¹³ dzieñ
			zamknijDzienLogowaniaTemperatury();

			/* Wait until last write operation on RTC registers has finished */
			RTC_WaitForLastTask();
			/* Reset counter value */
			RTC_SetCounter(0x0);
			/* Wait until last write operation on RTC registers has finished */
			RTC_WaitForLastTask();

		}	
	
}
