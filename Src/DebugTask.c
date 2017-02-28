/*
 * DebugTask.c
 *
 *  Created on: 2017. febr. 14.
 *      Author: blazovics
 */

#include <string.h>
#include "DebugTask.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "cmsis_os.h"

UART_HandleTypeDef huart1;

char buffer[256] = "test";
volatile char rxBuf;
char receiveBuffer[256] = "";

/* USART1 init function */
void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_7B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

void setBuffer(const char text[256]) {
	strcpy(buffer, text);
}

/*
 * Task function for managing debug information
 */


void WriteDebug(void const * argument)
{
	HAL_UART_Receive_IT(&huart1, &rxBuf, 1);
	/* Infinite loop */
	for(;;)
	{
		osDelay(1000);
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
		if (strlen(buffer) > 0) {
			HAL_UART_Transmit(&huart1, (uint8_t *) buffer, strlen(buffer), 120);
			strcpy(buffer, "");
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    // Your buffer is full with your data process it here or set a flag
	  if (rxBuf == '\n' || rxBuf == '\r') {
		  strcat(receiveBuffer, "\r\n");
		  setBuffer(receiveBuffer);
		  strcpy(receiveBuffer, "");
	  } else {
		  int len = strlen(receiveBuffer);
		  receiveBuffer[len] = rxBuf;
		  receiveBuffer[len+1] = '\0';;
	  }
	  HAL_UART_Receive_IT(&huart1, &rxBuf, 1);
  }
}
