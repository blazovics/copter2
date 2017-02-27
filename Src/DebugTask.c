/*
 * DebugTask.c
 *
 *  Created on: 2017. febr. 14.
 *      Author: blazovics
 */

#include "DebugTask.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_usart.h"
#include "cmsis_os.h"

USART_HandleTypeDef husart1;

/* USART1 init function */
void MX_USART1_Init(void)
{

  husart1.Instance = USART1;
  husart1.Init.BaudRate = 115200;
  husart1.Init.WordLength = USART_WORDLENGTH_7B;
  husart1.Init.StopBits = USART_STOPBITS_1;
  husart1.Init.Parity = USART_PARITY_NONE;
  husart1.Init.Mode = USART_MODE_TX_RX;
  husart1.Init.CLKPolarity = USART_POLARITY_LOW;
  husart1.Init.CLKPhase = USART_PHASE_1EDGE;
  husart1.Init.CLKLastBit = USART_LASTBIT_DISABLE;
  if (HAL_USART_Init(&husart1) != HAL_OK)
  {
    Error_Handler();
  }

}

/*
 * Task function for managing debug information
 */


void WriteDebug(void const * argument)
{
	/* Infinite loop */
	for(;;)
	{
		osDelay(1000);
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
	}
}
