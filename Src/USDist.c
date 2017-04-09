/*
 * USDist.c
 *
 *  Created on: 2017. márc. 12.
 *      Author: Dani
 */

#include "USDist.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

extern UART_HandleTypeDef huart7;
extern SemaphoreHandle_t usdistSemaphore;
extern osSemaphoreId DistSyncBinarySemHandle;

uint32_t DistanceGlobal;

uint8_t RecData[5] = {1, 0, 0, 0, 0};

void GetDistance(void const * argument)
{
	volatile uint16_t DistInch = 0;

	HAL_NVIC_DisableIRQ(UART7_IRQn);

	xSemaphoreTake(DistSyncBinarySemHandle , 0);

	for(;;)
		{

			__HAL_UART_CLEAR_FEFLAG(&huart7);
			__HAL_UART_CLEAR_OREFLAG(&huart7);
			HAL_NVIC_EnableIRQ(UART7_IRQn);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_UART_Receive_IT(&huart7, &RecData, 5);

			if(xSemaphoreTake(DistSyncBinarySemHandle , portMAX_DELAY) == pdTRUE)
			{

				DistInch = (uint16_t) (RecData[2]-48) * 100 + (uint16_t) (RecData[3]-48) * 10 +(uint16_t) (RecData[4]-48);
				if(xSemaphoreTake(usdistSemaphore , 0) == pdTRUE)
				{
					DistanceGlobal = DistInch * 254 + 200;
					xSemaphoreGive( usdistSemaphore );
				}
			}
			osDelay(1000);
			HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
		}

}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_NVIC_DisableIRQ(UART7_IRQn);
	xSemaphoreGiveFromISR(DistSyncBinarySemHandle, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

volatile int my_uart_error = 0;
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	my_uart_error++;
}
