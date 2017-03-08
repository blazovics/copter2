#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "DebugTask.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"

static UART_HandleTypeDef * huart1 = NULL;

static QueueHandle_t queue;
static unsigned int messageCount = 0;

static volatile char rxBuf;
static char receiveBuffer[lineLength] = "";

static SemaphoreHandle_t xSemaphore;

void setDebugUartHandler(UART_HandleTypeDef * huart) {
	huart1 = huart;
}

bool pushMessage(const char text[256]) {
  if (xQueueSendToBackFromISR(queue, text, 0) == pdPASS) {
    messageCount++;
    return true;
  } else {
    return false;
  }
}

void WriteDebug(void const * argument) {
	if (huart1 == NULL) {
		return;
	}
  queue = xQueueCreate(bufferSize, sizeof(char) * lineLength);
  pushMessage("UART initialization complete!\r\n");
  HAL_UART_Receive_IT(huart1, &rxBuf, 1);

  for(;;) {
    osDelay(1000);
    HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
    if (messageCount > 0) {
      int count = messageCount;
      for (int i = 0; i < count; ++i) {
        char local[256];
        xQueueReceive(queue, &local, portMAX_DELAY);
        HAL_UART_Transmit(huart1, (uint8_t *) local, strlen(local), 120);
        messageCount--;
      }
    }
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART1) {
    if (rxBuf == '\n' || rxBuf == '\r') {
      strcat(receiveBuffer, "\r\n");
      pushMessage(receiveBuffer);
      strcpy(receiveBuffer, "");
    } else {
      int len = strlen(receiveBuffer);
      receiveBuffer[len] = rxBuf;
      receiveBuffer[len+1] = '\0';;
    }
    HAL_UART_Receive_IT(huart1, &rxBuf, 1);
  }
}
