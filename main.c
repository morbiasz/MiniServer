/* ToDo:
 *
 *	Drobne:
 *	- obs³uga ADC i odczytywanie wartoci
 *	- zapis tej wartosci do jakiejs zmiennej globalnej
 *	- wyswietlenie wartosci w HTML
 *
 *	Du¿e:
 *	- Konfiguracja przez HTML:
 *		/ ustawianie czasu poprzez: 									time HH:MM:SS (uproszczone ale efektywny sposób)
 *		/ ustawianie czestotliwosci odczytywania temperatury: 			setTempRate XX
 *		/ ustawianie adresu bramy (gw_ip):								setGWIP 192.168.1.1
 *		/ ustawianie adresu IP ( rowniez w konsoli dla bezpieczenstwa ) setIP 192.168.2.2
 *
 *	- Konfiguracja przez konsole:
 *		+ ustawianie czestotliwosci odczytywanie temperatury
 *
 *	- Zbieranie danych
 *		- najlepiej na serwerze ( skrypt PHP )
 *		- prezentacja w PHP lub HTML5
 *		- ew. polaczenie z baza danych
 *		- log poleceñ na kartê SD
 *
 *
 *
 *
 */

#include <stdio.h>

#include "src/init_func.h"
#include "usart/usart.h"
#include "src/delay.h"
#include "src/min_printf.h"
#include "rtc/rtc.h"

// Konfiguracja urzadzenia
#include "server_config.h"

// Stos TCP/IP
#include "../SERVER/websrv_help_functions.h"
#include "../src/conversion.h"
#include "../src/ringbuffer.h"
#include "SERVER/tcpip/net.h"
#include "SERVER/serverWWW.h"
#include "enc28j60/enc28j60.h"
#include "SERVER/tcpip/ip_arp_udp_tcp.h"
#include "enc28j60/spi.h"

#if TEMP_READ

	#include "../1wire/temp_read.h"

#endif

// zmienne uzywane przez przerwania:
#if CMD_LINE

	#include "../CMD/cmd.h"

#endif

#if RC5_CONTROL

	#include "../rc5/rc5.h"

#endif

#if SD_CARD

	#include "sd_card/fatfs/ff.h"
	#include "sd_card/fatfs/ff_diskio.h"
	FATFS Fs;

#endif

void static HardwareSetup( void );

inline void Blink(void)
{
		GPIOC->BSRR = GPIO_Pin_9; 	Delay(200);
		GPIOC->BRR = GPIO_Pin_9; 	Delay(200);
		GPIOC->BSRR = GPIO_Pin_9; 	Delay(200);
		GPIOC->BRR = GPIO_Pin_9;
}

int main (void)
{
#if SD_CARD
	DSTATUS driveStatus;
	FATFS fatSystem;
	FIL file;
#endif

	HardwareSetup();

	Delay(1000); //to make sure power supply is quiescient
	InitServer();

	RTC_Config();

#if SD_CARD

	if (f_mount(0, &fatSystem) == FR_OK) {

		driveStatus = disk_initialize(0);

		if (driveStatus & STA_NOINIT || driveStatus & STA_NODISK || driveStatus
				& STA_PROTECT) {

#if UART_DEBUG
			min_printf("Blad inicjalizacji\r\n");
#endif

		} else {

#if UART_DEBUG
			usart_printf("Zamontowano karte SD\r\n");
#endif

			if (f_open(&file, "/temp.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK)
			{

#if UART_DEBUG
				usart_printf("Otwarto plik 'temp.txt'\r\n");
#endif
				Blink();

			}

			f_close(&file);
		}
	} else {
#if UART_DEBUG
		min_printf("Blad montowania\r\n");
#endif

}

#endif

    while(1)
		{
			
			// Obsluga linii komend
#if CMD_LINE
			CommandLineCheck();
#endif

			// Obsluga serwera
			ServerCheckForJob();

#if RC5_CONTROL
			RC5Check();
#endif
			
#if TEMP_READ
			TempCheck();
#endif
			
		}
}


static void HardwareSetup( void )
{
	RCC_Conf();
	NVIC_Config();
	GPIO_Config();
	
	//Standard, ms delay
	Delay_config();
	
	//used for 1Wire
	Delay_us_init();
	usartSetup();
	
#if CMD_LINE
		USART_ITConfig(USART1, USART_IT_RXNE,  ENABLE);
#endif
	
	// used by ENC28j60
	SPI1_Init();
	
	// Konfiguracja pwm dla serwomechanizmu i diody rgb
	pwmSetup();

#if RGB_DIODE
	rgbPwmSetup();
#endif
	
	// uzywane przez sterowanie przez RC5
#if RC5_CONTROL
	RC5Timer_Config();
#endif

}
