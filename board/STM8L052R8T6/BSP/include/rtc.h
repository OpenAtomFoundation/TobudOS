#ifndef __RTC_H_
#define __RTC_H_

typedef struct   
{
	uint8_t cSecond;
	uint8_t cMinute;
	uint8_t cHour;
	uint8_t cWeek;
	uint8_t cDay;
	uint8_t cMonth;
	uint8_t cYear;
} StruCLOCK;
extern StruCLOCK clock;

void RTC_Setting_Init(void);
void RTC_Get_Time(void);

#endif

