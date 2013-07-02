#include "min_printf.h"
#include "CMD_commands.h"
#include "delay.h"
#include <init_func.h>
#include <server_config.h>
#include <stm32f10x_rtc.h>
#include <stdlib.h>
#include <string.h>
#include "../SERVER/serverWWW.h"
#include "../RTC/rtc.h"
#include "../sd_card/fatfs/ff_diskio.h"
#include "../sd_card/fatfs/ff.h"
#include <stdio.h>
extern char * ENDLINE; 	// z pliku cmd.c

void funkcja_time(char * argument, char arg) {

	min_printf("Aktualny czas: %d:%d", RTC_GetCounter()/3600, (RTC_GetCounter() % 3600) / 60);
	min_printf(ENDLINE);
}

void funkcja_ls(char * argument, char arg) {

	uint8_t i = 0;
	const uint8_t liczba_plikow = 10;
	min_printf("Zawartosc biezacego katalogu:\r\n");
	DIR dir;
    FRESULT res;
    FILINFO fno;

	for(;i<10;i++) {
		SendChar('-');
	}

	for(i=0; i < liczba_plikow; i++) {
		min_printf("PLIK%d   ", i);

		if ( (i%4) == 0 ) {
			min_printf(ENDLINE);
		}

	}

	for (;;) {
		res = f_readdir(&dir, &fno);                   	// Read a directory item

		if (res != FR_OK || fno.fname[0] == 0)
			break;  									// Break on error or end of dir

		if (fno.fname[0] == '.')
			continue;             						//Ignore dot entry
/*
	    if (fno.fattrib & AM_DIR) {                    	// It is a directory

	    	//sprintf(&path[i], "/%s", fn);
	    	res = scan_files(path);
	    	if (res != FR_OK) break;
	        	path[i] = 0;
	        }
	    	else {                                       // It is a file.
	        	//min_printf("%s/%s\n", path, fn);
	        }
*/
	}

	min_printf("\r\n");

}

void funkcja_info(char * argument, char arg) {

	uint8_t i = 0;
	RTCDate data;

	RTCgetTime(&data);

	for(i=0;i<20;i++) {
		SendChar('-');
	}

	min_printf(ENDLINE);

	min_printf("Informacje o systemie:\r\n");

	for(i=0;i<20;i++) {
		SendChar('-');
	}

	min_printf(ENDLINE);
	min_printf("Aktualny czas: %d:%d\r\n", data.hour, data.minute);
	min_printf("Stan wyjsc I/O:\r\n");
	min_printf("\tGPIOA: %x\r\n", GPIOA->ODR);
	min_printf("\tGPIOB: %x\r\n", GPIOB->ODR);
	min_printf("\tGPIOC: %x\r\n", GPIOC->ODR);
	min_printf("Stan karty SD: ");

	if (  !socket_is_empty() )
		min_printf("obecna");
	else
		min_printf("brak");

	min_printf(ENDLINE);

	for(i=0;i<20;i++) {
		SendChar('-');
	}

	min_printf(ENDLINE);



}

//funkcja wymusza wys³anie requestu NTP
void funkcja_NTPrequest(char * argument, char arg) {

	resetNTPClient();

}

void funkcja_updateIP(char * argument, char arg) {

	updateIP();

}

void funkcja_configWWW(char * argument, char arg) {


	if ( !strcmp(argument, "enable")) {


		enableWWWConfig();

	}
	else if ( !strcmp(argument, "disable")) {

		disableWWWConfig();

	}
	else {

		min_printf("Niepoprawny parametr!\r\n");
		min_printf(ENDLINE);


	}

}

void funkcja_setIP(char * argument, char arg) {

	uint8_t newIP[] = { 192, 168, 1, 5 };
	changeIP(newIP);

}

void funkcja_setTempRate(char * argument, char arg) {


}

void funkcja_setTime(char * argument, char arg) {

	asm("nop");
	return;
}

