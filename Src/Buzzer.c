/*
 * Buzzer.c
 *
 *  Created on: 2017. márc. 5.
 *      Author: Dani
 */

#include "Buzzer.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

void RingBuzzer(void const * argument)
{


	for(;;)
	{

		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
		osDelay(1000);
	}
}
