/*

	Jest to modul umozliwiajacy dodanie do aplikacji funkcjonalnosci prostego terminala ( CLI )
	Wywolanie kazdej komendy ogranicza sie do podania jej nazwy oraz parametru
	
	W obecnej (0.2) wersji mozna korzystac tylko z jednego argumentu, jednakze latwo to zmienic.

	Do zrobienia:
		Obsluga tabulacji, skrotow typu CRTL+Z


Autor:	Jakub Zbydniewski

Data ostatniej modyfikacji: 15.12.2012

*/

#include <string.h>

#include "cmd.h"
#include "conversion.h"
#include "min_printf.h"
#include "CMD_commands.h"
#include "usart.h"
#include <stdlib.h>
#include <init_func.h>
#include "ringbuffer.h"
#include "server_config.h"
#include <stdio.h>
volatile uint8_t command_request = 0;

#if TERMINAL_USE_PASSWORD
static const char password[] = "kuba";
#endif

#if COMMAND_LOG

#include "../sd_card/fatfs/ff.h"
#include "../RTC/rtc.h"

#endif

char * ENDLINE = "\r\n";
static uint8_t terminal_state = 0;

const struct CMD_entry_table CMD_commands[] =
{
	//nazwa komendy, opis komendy,     , nazwa funkcji, czy potrzeba argument ? 0 = nie, tak = 1
    {"help",   "Wyswietla komendy", 	help,				0},
	{"time",   "Wyswietla czas RTC", 	funkcja_time,		0},
	{"ls", 	   "Listing folderu",		funkcja_ls, 		0},
	{"info",   "Running info", 			funkcja_info, 		0},
	{"enable", "tryb super user", 		enable, 			1},
	{"IpReq", "refresh IP", 			funkcja_updateIP, 	0},
	{"NTPReq", "refresh NTP",		funkcja_NTPrequest, 0}

};

void help(char * argument, char arg)
{
    uint8_t i = 0;


    if ( arg == 0 )
    {
        min_printf("\rLista dostepnych komend:\r\n");

        // jezeli bez argumentu, to wypisz wszystkie komendy
    	for (i =0; i< sizeof(CMD_commands)/ sizeof(CMD_commands[0]); i++)
			{
				min_printf("\rKomenda: %s\rOpis: %s\r\n", CMD_commands[i].keyword, CMD_commands[i].description);
				min_printf("Argument?: ");
				CMD_commands[i].need_argument	 ? min_printf("tak\r\n") : min_printf("nie\r\n");
			}
    }
    else
    {

    	// jezeli jest argument, to wypisz help dla konkretnej komendy
    	while ( strcmp(argument, CMD_commands[i].keyword)!=0 && i < sizeof(CMD_commands)/ sizeof(CMD_commands[0]))
    	        i++;

    	if ( i == sizeof(CMD_commands)/ sizeof(CMD_commands[0]))
    	{
    	       min_printf("Brak takiej komendy\r\n");
    	       min_printf(ENDLINE);
    	}
    	else
    	{
    		min_printf("%s",CMD_commands[i].description);
    		min_printf(ENDLINE);
    	}

    }

}

#if TERMINAL_USE_PASSWORD
// Funkcja która realizuje dostêp do trybu uprzywilejowanego
void enable(char * argument, char arg) {

	if (terminal_state == UNLOCKED_TERMINAL) {
		return;
	}
	if ( strcmp(argument, password)==0 ) {

		min_printf("Przyznano dostep");
		min_printf(ENDLINE);
		terminal_state = UNLOCKED_TERMINAL;
	}
	else {

		min_printf("Blad autoryzacji");
		min_printf(ENDLINE);
	}

}


//funkcja do uzycia przez zewnetrzne obiekty ( np. zegar RTC resetujacy stan zalogowania co jakis czas )
void lockUnlockTerminal(uint8_t val) {

	if ( val == LOCKED_TERMINAL) {
		terminal_state = LOCKED_TERMINAL;
	}
	else
		terminal_state = UNLOCKED_TERMINAL;
}
#endif

//function which performs function assigned to specific command
int8_t perform_command(char * command, char * arg, char is_argument)
{
    unsigned char i = 0;

#if TERMINAL_USE_PASSWORD
    if ( terminal_state == LOCKED_TERMINAL  && (strcmp(command, "enable")!=0) )
    {
    	min_printf("Musisz sie zalogowac przez enable");
    	min_printf(ENDLINE);
    	return -2;
    }
#endif

    //szukamy komendy w tablicy komend
    while ( strcmp(command, CMD_commands[i].keyword)!=0 && i < sizeof(CMD_commands)/ sizeof(CMD_commands[0]))
        i++;

    if ( i == sizeof(CMD_commands)/ sizeof(CMD_commands[0]))
    {
        if (strcmp(command, ""))
            return -1;

        return -2;
    }

    //wykonanie funkcji przypisanej do komendy
    CMD_commands[i].wsk(arg, is_argument);

    return 0;
}

unsigned int convert_argument( char * arg)
{
    unsigned int number;

    // in hex
    if ( arg[0] == '0' && arg[1] == 'x' )
    {
        xtoi(arg, &number);
    }
    else
        number = myatoi(arg);


    return number;

}

// Pobiera komende z bufora kolowego uzupelnianego przez przerwanie od USART'u RXNE 
void get_command_from_ring_buffer(char * line, char * arg, char * is_argument)
{
    char c = 0;
    int i = 0;

        memset(line, 0, CMD_LENGTH_MAX);
        memset(arg, 0, CMD_LENGTH_MAX);
	
        // Pobieranie polecenia
        while( (c = ring_buffer_pop()) != '\r' && i < CMD_LENGTH_MAX )
        {
					
            // if space try again
            if ( c == ' ' && i == 0 )
                continue;
            else if ( c == ' ')
                break;

            *(line+i) = c;
            i++;

        }

        if ( c == '\r' )
        {
            *is_argument = 0;
            return;
        }
        else
        {

            *is_argument = 1;
        }


    i = 0;

    // Pobieranie argumentu
    while( (c = ring_buffer_pop()) != '\r')
    {
        // if space try again
        if ( c == ' ')
            continue;

		//Musi przeleciec cala pobrana linie az do znaku '\r'
		if (i < CMD_LENGTH_MAX)
			*(arg+i) = c;
				
        i++;

    }

}

void CommandLineCheck(void)
{
			char komenda[10];
			char argument[10];
			char arg = 0; 
			FIL file;
			UINT bytesWritten;
			char sd_ok = 0;
			RTCDate data;
			RTCgetTime(&data);
			char buf[40];
	
			// obsluga interpretera polecen
			if (command_request)
			{
				#if COMMAND_LOG
				if( f_open(&file, "/cmd.log", FA_READ | FA_WRITE | FA_OPEN_ALWAYS) == FR_OK ) {

					sd_ok = 1;
					f_lseek(&file, f_size(&file));

				}
				else
				{
					min_printf("CMD: Blad karty SD");
				}
				#endif

				//pobiera komendê z bufora ko³owego razem z argmentem i informacj¹ czy potrzeba argument
				get_command_from_ring_buffer(komenda, argument, &arg);

				#if COMMAND_LOG
					if ( sd_ok == 1) {
						GPIOC->BSRR = GPIO_Pin_8;
						sprintf(buf, "Data: %d.%d.%d Godzina:%d:%d ", data.day, data.month, data.year, data.hour, data.minute);
						f_write(&file, buf, strlen(buf), &bytesWritten);
						f_write(&file, "MiniServer console#>",20, &bytesWritten);

						sprintf(buf, "%s %s\r\n\r\n", komenda, argument);
						f_write(&file, buf, strlen(buf), &bytesWritten);
						GPIOC->BRR = GPIO_Pin_8;

					}
				#endif

				//w niezmienionej formie wykonujemy komendê
				if (perform_command(komenda, argument, arg) == -1) {
					min_printf("Command not found\r");
					#if COMMAND_LOG
					f_write(&file, "Command not found\r\n",19, &bytesWritten);
					#endif
				}

				f_close(&file);

				usart_printf("MiniServer console#>");
				command_request--;
				
			}
	
}
