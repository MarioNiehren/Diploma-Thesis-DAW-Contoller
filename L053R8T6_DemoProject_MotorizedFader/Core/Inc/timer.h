/**
 * @defgroup		Timer
 * @brief				This Module offers non blocking timer functions for general use.
 * @addtogroup	Timer
 * @{
 *
 * @file			timer.h
 *
 * @date			Aug 31, 2023
 * @author			Mario Niehren
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32L0xx_hal.h"

/**
 * @name		Structure of a Timer
 * @brief		The user has to declare an object of this structure for each timer.
 */
typedef struct {
	unsigned long int now;		/**<HAL_GetTick will be stored here */
	unsigned long int old;		/**<HAL_GetTick from previous cycle will be stored here */
	uint16_t threshold;				/**<Threshold of the timer in MS is stored here */
}timer_setup_structTd;

/**
 * @name		User Functions
 * @brief		Functions to use the timer in any user code
 *
 * How to use the timer:
 * 	1. Declare an object of timer_setup_structTd type
 * 	2. Set the threshold time (The rest of the structure will be set to 0 here)
 * 	3. Check it the timer is elapsed. The Timer will automatically restart if the function returns true.
 *
 * 	@note		This timer is based on the HAL_GetTick() function. It this function is not setup to milliseconds,
 * 					the threshold will have an other unit.
 * 					HAL-Standard is in milliseconds. Only modify this if a smaller interval than milliseconds is needed
 * 					as it my require a deeper dive to the HAL-Drivers.
 *
 * @{
 */

/**
 * @brief		Set the threshold time here
 *
 * @param 	timer: Pointer to the Timer Object of the user
 * @param		threshold: Threshold time in MS
 * @return  none
 */
void timer_set_ThresholdMS(timer_setup_structTd* timer, uint32_t threshold);

/**
 * @brief		Check the timer state. Returns true and restarts timer if timer elapsed.
 *
 * @param 	timer: Pointer to the Timer Object of the user
 * @return  Boolean True if timer elapsed, false if not
 */
bool timer_check_TimerElapsed(timer_setup_structTd* timer);
/** @} *//* end of name "User Functions" */

/** @} *//* end of defgroup "Timer" */
#endif /* INC_TIMER_H_ */
