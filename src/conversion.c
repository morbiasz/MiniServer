#include "usart.h"
#include <string.h>
#include "conversion.h"
void dec2hex(unsigned int number)
{

int r[10],i=0,j=0;

	while(number>0)
	{
		r[i]=number%16;
		number=number/16;
		i++;
		j++;
	}
	//usart_printf("The unsigned decimal equivalent is:  ");
	for(i=j-1;i>=0;i--)
	{
		switch (r[i])
		{
		case 10:
			SendChar('A');
			break;

		case 11:
			SendChar('B');
			break;

		case 12:
			SendChar('C');
			break;

		case 13:
			SendChar('D');
			break;

		case 14:
			SendChar('E');
			break;

		case 15:
			SendChar('F');
    		break;

		default:
			SendChar(r[i]+48);
			break;
		}//switch

	}//for

}

/* The Itoa code is in the public domain */
char* itoa(int value, char* str, int radix) {
    static char dig[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz";
    int n = 0, neg = 0;
    unsigned int v;
    char* p, *q;
    char c;

    if (radix == 10 && value < 0) {
        value = -value;
        neg = 1;
    }
    v = value;
    do {
        str[n++] = dig[v%radix];
        v /= radix;
    } while (v);
    if (neg)
        str[n++] = '-';
    str[n] = '\0';

    for (p = str, q = p + (n-1); p < q; ++p, --q)
        c = *p, *p = *q, *q = c;
    return str;
}

int xtoi(const char* xs, unsigned int* result)
{
 size_t szlen = strlen(xs);
 int i, xv, fact;

 if (szlen > 0)
 {
  // Converting more than 32bit hexadecimal value?
  if (szlen>8) return 2; // exit

  // Begin conversion here
  *result = 0;
  fact = 1;

  // Run until no more character to convert
  for(i=szlen-1; i>=0 ;i--)
  {
   if (isxdigit(*(xs+i)))
   {
    if (*(xs+i)>=97)
    {
     xv = ( *(xs+i) - 97) + 10;
    }
    else if ( *(xs+i) >= 65)
    {
     xv = (*(xs+i) - 65) + 10;
    }
    else
    {
     xv = *(xs+i) - 48;
    }
    *result += (xv * fact);
    fact *= 16;
   }
   else
   {
    // Conversion was abnormally terminated
    // by non hexadecimal digit, hence
    // returning only the converted with
    // an error value 4 (illegal hex character)
    return 4;
   }
  }
 }

 // Nothing to convert
 return 1;
}

uint16_t myatoi(const char *string)
{
 uint16_t i =0;
	
 while(*string)
 {
 i=(i<<3) + (i<<1) + (*string - '0');
 string++;

 // Dont increment i!

 }
 return(i);
}

