/*
 * PWMTask.c
 *
 *  Created on: 2017. febr. 14.
 *      Author: blazovics
 */

#include "PWMTask.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal.h"

/*
 * Task function for managing PWM outputs
 */
void SetPWMOutput(void const * argument)
{
	/* Infinite loop */
	for(;;)
	{
		osDelay(1000);
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
	}
}


