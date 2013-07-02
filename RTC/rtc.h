/*
 * rtc.h
 *
 *  Created on: 14-11-2012
 *      Author: kuba
 */

#ifndef RTC_H_
#define RTC_H_
#include <inttypes.h>
#include <stm32f10x_rtc.h>
#include <stm32f10x_pwr.h>
#include <stm32f10x_bkp.h>

typedef enum { MON, TUE, WED, THR, FRI, SAT, SUN } WEEK_DAY;
typedef enum { JAN, FEB, MAR, APR, MAY, JUN, JUL, OCT, AUG, SEPT, NOV, DEC } MONTH;

typedef struct {

	WEEK_DAY day;
	MONTH month;

	uint16_t year;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;

}  RTCDate;

void RTCgetTime(RTCDate * time);
void RTC_Config(void);

#endif /* RTC_H_ */
