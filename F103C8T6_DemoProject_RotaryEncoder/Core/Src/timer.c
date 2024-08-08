/**
 * @file		timer.c
 *
 * @date		Aug 31, 2023
 * @author	Mario Niehren
 */

#include "timer.h"

void Timer_set_ThresholdInMs(Timer_structTd* timer, uint32_t threshold)
{
	timer->threshold = threshold;
	timer->now = 0x00;
	timer->old = 0x00;
}

bool Timer_check_TimerElapsed(Timer_structTd* timer)
{
  bool returnValue = false;
  timer->now = HAL_GetTick();
  if((timer->now - timer->old) >= timer->threshold)
  {
    returnValue = true;
    Timer_restart_Timer(timer);
  }
  return returnValue;
}

void Timer_restart_Timer(Timer_structTd* timer)
{
  timer->now = HAL_GetTick();
  timer->old = timer->now;
}
