#ifndef TEMP
#define TEMP

#include <stm32f10x.h>
#include "1wire.h"
#include <delay.h>
#include <min_printf.h>
#include <usart.h>
#include <server_config.h>

#define MAX_LICZBA_CZUJNIKOW_TEMP 2

// stany automatu czytaj¹cego temperaturê
#define IDLE				0
#define READ_REQUEST 		1
#define WAITING 			2
#define CONVERSION_FINISHED 3

void TempCheck(void);
void ustawStanOdczytywaniaTemperatury(uint8_t stan);
uint8_t odczytajStanOdczytywaniaTemperatury();
void zamknijDzienLogowaniaTemperatury();

#endif
