#ifndef CMD_H_INCLUDED
#define CMD_H_INCLUDED
#include <stm32f10x.h>

// Maximum number of commands
#define CMD_COUNT_MAX 5

#define CMD_LENGTH_MAX 		10
#define CMD_DESCR_MAX 		20

#define UNLOCKED_TERMINAL 	1
#define LOCKED_TERMINAL 	0

struct  CMD_entry_table
{
	char  keyword[CMD_LENGTH_MAX] ;
	char  description[CMD_DESCR_MAX];
	void (*wsk)(char *, char);
	char need_argument;
};

// Podstawowe komendy wbudowane
void help(char *, char);
void enable(char * argument, char arg);

int8_t perform_command(char * command, char * arg, char is_argument);
char insert_cmd(char * _keyword, char * _description ,void (*_wsk)(unsigned int));
void get_command_from_ring_buffer(char * line, char * arg, char * is_argument);
unsigned int convert_argument( char * arg);
void lockUnlockTerminal(uint8_t val);

void CommandLineCheck(void);

#endif // CMD_H_INCLUDED
