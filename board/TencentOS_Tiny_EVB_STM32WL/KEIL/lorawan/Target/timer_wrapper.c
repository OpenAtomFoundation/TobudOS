#include <tos_k.h>
#include "timer.h"
#include "rtc_if.h"


uint8_t TIMER_GetCurrentTime(void)
{
  uint32_t now = RTC_GetTimerValue( );
  return  RTC_Tick2ms(now);
}

uint8_t TIMER_GetElapsedTime(TimerTime_t past)
{
  uint32_t nowInTicks = RTC_GetTimerValue();
  uint32_t pastInTicks = RTC_ms2Tick(past);
  /* intentional wrap around. Works Ok if tick duation below 1ms */
  return RTC_Tick2ms(nowInTicks - pastInTicks);
}