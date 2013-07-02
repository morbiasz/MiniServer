/* Martin Thomas 4/2009 */

#include "ff_integer.h"
#include "fattime.h"
#include "../RTC/rtc.h"

DWORD get_fattime (void)
{
	DWORD res;

	RTCDate rtc;

	RTCgetTime( &rtc );
	
	res =  (((DWORD)rtc.year - 1980) << 25)
			| ((DWORD)rtc.month << 21)
			| ((DWORD)rtc.day << 16)
			| (WORD)(rtc.hour << 11)
			| (WORD)(rtc.minute << 5)
			| (WORD)(rtc.second >> 1);

	return res;
}

