#include <string.h>
#include <stdbool.h>
#include "DebugTask.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "cmsis_os.h"

UART_HandleTypeDef huart1;

struct Queue {
	unsigned int messageCount;
	char buffer[lineLength][bufferSize];
};

struct Queue messageQueue;

volatile char rxBuf;
char receiveBuffer[lineLength] = "";

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

bool pushMessage(const char text[256]) {
	if (messageQueue.messageCount == bufferSize) {
		return false;
	}
	strcpy(messageQueue.buffer[messageQueue.messageCount++], text);
	return true;
}

/*
 * Task function for managing debug information
 */


void WriteDebug(void const * argument)
{
	messageQueue.messageCount = 0;
	HAL_UART_Receive_IT(&huart1, &rxBuf, 1);
	/* Infinite loop */
	for(;;)
	{
		osDelay(1000);
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
		if (messageQueue.messageCount > 0) {
			int count = messageQueue.messageCount;
			for (int i = 0; i < count; ++i) {
				HAL_UART_Transmit(&huart1, (uint8_t *) messageQueue.buffer[i], strlen(messageQueue.buffer[i]), 120);
			}
		}
		messageQueue.messageCount = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    // Your buffer is full with your data process it here or set a flag
	  if (rxBuf == '\n' || rxBuf == '\r') {
		  strcat(receiveBuffer, "\r\n");
		  pushMessage(receiveBuffer);
		  strcpy(receiveBuffer, "");
	  } else {
		  int len = strlen(receiveBuffer);
		  receiveBuffer[len] = rxBuf;
		  receiveBuffer[len+1] = '\0';;
	  }
	  HAL_UART_Receive_IT(&huart1, &rxBuf, 1);
  }
}
