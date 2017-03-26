#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "ControllerTask.h"
#include "DebugTask.h"

void StartController(void const * argument) {
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 10 * portTICK_PERIOD_MS;

	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		osDelay(1000);
		  DebugMessage test;
		  test.dataCount = '4';
		  test.messageId = 'x';
		  test.timestamp = xTaskGetTickCount();
		  test.data[0] = 1.0;
		  test.data[1] = 3.14;
		  test.data[2] = 6.28;
		  test.data[3] = 0;
		  pushDebugMessage(test);
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_14);
	}
}
