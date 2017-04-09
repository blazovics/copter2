/*
 * USDist.c
 *
 *  Created on: 2017. márc. 12.
 *      Author: Dani
 */

#include "USDist.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

#include "DebugTask.h"

/* External variables START */
extern UART_HandleTypeDef huart7;
extern SemaphoreHandle_t usdistSemaphore;
extern osSemaphoreId DistSyncBinarySemHandle;
/* External variables END */

/* Global variables START */
uint32_t DistanceGlobal;
/* Global variables END */

static uint8_t RecData[5] = {1, 0, 0, 0, 0};
static char sendText[8] = "Vetel: ";
static char noText[9] = "S.O.S!\n\r";
static DebugMessage ultraSoundMessage;

void GetDistance(void const * argument)
{
	volatile uint16_t DistInch = 0;
	char ultraSoundText[6] = "0";

	HAL_NVIC_DisableIRQ(UART7_IRQn);

	xSemaphoreTake(DistSyncBinarySemHandle , 0);

	for(;;)
		{

			__HAL_UART_CLEAR_FEFLAG(&huart7);
			__HAL_UART_CLEAR_OREFLAG(&huart7);
			HAL_NVIC_EnableIRQ(UART7_IRQn);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_UART_Receive_IT(&huart7, &RecData, 5);

			if(xSemaphoreTake(DistSyncBinarySemHandle , pdMS_TO_TICKS(200)) == pdTRUE)
			{

				DistInch = (uint16_t) (RecData[2]-48) * 100 + (uint16_t) (RecData[3]-48) * 10 +(uint16_t) (RecData[4]-48);
				if(xSemaphoreTake(usdistSemaphore , 0) == pdTRUE)
				{
					DistanceGlobal = DistInch * 254 + 200;
					xSemaphoreGive( usdistSemaphore );
				}
				MyIntToCharArray5(DistInch, &ultraSoundText);
				pushMessage(sendText);
				pushMessage(ultraSoundText);
				osDelay(1000);
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);

			}
			else
			{
				HAL_NVIC_DisableIRQ(UART7_IRQn);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
				pushMessage(noText);
				osDelay(800);
			}

		}

}


void Uart7SeperateCallback(void)
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

void MyIntToCharArray5(uint32_t number, char* buffer)
{
	uint8_t i = 5;

	buffer[i] = '\0';
	while(i > 0)
	{
		i--;
		buffer[i] = number % 10 + '0';
		number = number / 10;
	}
}
