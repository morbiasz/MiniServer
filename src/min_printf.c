#include "min_printf.h"
#include <stdarg.h>
#include "conversion.h"

void min_printf(char *fmt, ...)
{
	    va_list ap;
	    char *p, *sval;
	    int ival = 0;
	    char temp[5];

	    va_start(ap, fmt);
	    for (p=fmt;*p;p++)
	    {
	        if(*p!='%')
	        {
	        	SendChar(*p);
	            continue;
	        }

	        switch(*++p)
	        {
	            case 'd':
	                ival = va_arg(ap, int);
	                itoa(ival, temp, 10);
	                usart_printf(temp);
	                break;
	            case 's':
	                for (sval= va_arg(ap, char *);*sval;sval++)
	                {
	                	SendChar(*sval);

	                }
	                break;
	            case 'x':
	                ival =va_arg(ap, int);
	                usart_printf("0x");
	                dec2hex(ival);
	                break;
	            default:
	            	SendChar(*p);
	                break;

	        }
	    }
	    va_end(ap);

}
