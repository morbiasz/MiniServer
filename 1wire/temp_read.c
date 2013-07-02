#include "temp_read.h"
#include <conversion.h>
#include <string.h>
#include "../sd_card/fatfs/ff.h"
#include "../RTC/rtc.h"
#include <stdio.h>

extern unsigned int gSensorIDs[MAXSENSORS][8];

// przechowuje stan maszyny stanów odczytywania temperatury
static uint8_t volatile stan_odczytywania_temp = 0;
static volatile uint8_t next_day;

static uint8_t liczba_czujnikow_temperatury;

char temperatura[MAX_LICZBA_CZUJNIKOW_TEMP][8];

void ustawStanOdczytywaniaTemperatury(uint8_t stan) {
	stan_odczytywania_temp = stan;
}

void zamknijDzienLogowaniaTemperatury() {
	next_day = 1;
}

uint8_t odczytajStanOdczytywaniaTemperatury() {
	return stan_odczytywania_temp;
}

void TempCheck(void)
{
		int8_t i = 0;
		int8_t temp_calk;
		int8_t temp_ulam;
		unsigned char ret = 1;

		#if SD_CARD
		UINT bytesWritten;
		char stringBuf[15];
		FIL file;
		RTCDate date;
		#endif

		if ( stan_odczytywania_temp == READ_REQUEST) 						// wywolano zadanie odczytania temperatury
		{
				liczba_czujnikow_temperatury = search_sensors();			//szukamy sensorow, wpisze ich id do tablicy gSensorID

				if ( liczba_czujnikow_temperatury > MAX_LICZBA_CZUJNIKOW_TEMP )
				{
					liczba_czujnikow_temperatury = MAX_LICZBA_CZUJNIKOW_TEMP;

#if TEMP_UART_DEBUG
					usart_printf("Za duzo czujnikow!\r\n");
#endif

				}

				ret = ow_reset();
				ow_write_byte(0xCC);										//skip ROM
				ow_write_byte(0x44);										//zacznij konwersje - zrobi ja kazdy czujnik
				stan_odczytywania_temp = WAITING;
		}
		if ( stan_odczytywania_temp == CONVERSION_FINISHED )				// minal czas konwersji
		{	
			for (i = 0;i <  liczba_czujnikow_temperatury; i++)
			{

				//komenda odczytu dla pierwszego (indeks 0) czujnika
				ow_command( 0xBE,&gSensorIDs[i][0] );
				
				// Sprawdzamy czy nie wystapil blad CRC
				if (read_temp(&temp_calk, &temp_ulam) == 1)		
				{
					
					#if TEMP_UART_DEBUG
					usart_printf("Blad crc\r\n");
					#endif
					
					if (i == liczba_czujnikow_temperatury-1)
						stan_odczytywania_temp = IDLE;
					continue;
				}
				
#if TEMP_UART_DEBUG
				min_printf("%d.%d\r\n", temp_calk, temp_ulam);
#endif
				
				// ----------------- ZAPIS TEMPERATURY DO BUFORA ------------------------- //
				memset(temperatura[i], 0, 8);
				sprintf(temperatura[i], "\r\n%d.%d", temp_calk, temp_ulam);

				// ----------------- ZAPIS TEMPERATURY NA KARTE SD ----------------------- //
				// ----------------------------------------------------------------------- //

#if SD_CARD

				//otwieramy do zapisu i odczytu, utworzy nowy plik jesli go nie ma
				if( f_open(&file, "/temp.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK )
				{
					// przechodzimy na koniec
					f_lseek(&file, f_size(&file));

					// Oddzielanie dni
					if (next_day == 1)
					{
						f_write(&file, "----------\r\n",12, &bytesWritten);
						f_sync(&file);
						f_lseek(&file, f_size(&file));
						next_day = 0;
					}

					//odczytaj date z RTC
					RTCgetTime(&date);

					sprintf(stringBuf, "%d:%d [%d] : %d.%d\r\n", date.hour, date.minute, i, temp_calk, temp_ulam);
					f_write(&file, stringBuf, strlen(stringBuf), &bytesWritten);

					f_close(&file);

				}
				else {
#if TEMP_UART_DEBUG
					min_printf("Blad zapisu temperatury\r\n");
#endif
				}

#endif
				// ---------------- KONIEC ZAPISU NA KARTE SD --------------------------- //
				
			}

			// temperatura odczytana, wyzeruj stan
			stan_odczytywania_temp = IDLE;
		}
}
