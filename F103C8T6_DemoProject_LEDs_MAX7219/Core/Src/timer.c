/**
 * @file		timer.c
 *
 * @date		Aug 31, 2023
 * @author	Mario Niehren
 */

#include "timer.h"

void timer_set_ThresholdMS(timer_setup_structTd* timer, uint32_t threshold)
{
	timer->threshold = threshold;
	timer->now = 0x00;
	timer->old = 0x00;
}

void reset_Timer(timer_setup_structTd* timer);

bool timer_check_TimerElapsed(timer_setup_structTd* timer)
{
  bool returnValue = false;
  timer->now = HAL_GetTick();
  if((timer->now - timer->old) >= timer->threshold)
  {
    returnValue = true;
    reset_Timer(timer);
  }
  return returnValue;
}

void reset_Timer(timer_setup_structTd* timer)
{
  timer->now = HAL_GetTick();
  timer->old = timer->now;
}
