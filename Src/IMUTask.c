/*
 * IMUTask.c
 *
 *  Created on: 2017. febr. 14.
 *      Author: blazovics
 *
 */

#include <IMUTask.h>
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

/*
 * Task function for managing IMU data reading
 */

void ReadIMU(void const * argument)
{
	/* Infinite loop */
	for(;;)
	{
		osDelay(1000);
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);
	}
}
