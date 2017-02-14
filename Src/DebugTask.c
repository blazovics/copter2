/*
 * DebugTask.c
 *
 *  Created on: 2017. febr. 14.
 *      Author: blazovics
 */

#include "DebugTask.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

void WriteDebug(void const * argument)
{
	/* Infinite loop */
	for(;;)
	{
		osDelay(1000);
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
	}
}
