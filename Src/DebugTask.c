#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "DebugTask.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"

static UART_HandleTypeDef * localUartHandler = NULL;

static QueueHandle_t queue;
static unsigned int messageCount = 0;

static volatile char rxBuf;
static char receiveBuffer[lineLength] = "";

static SemaphoreHandle_t xSemaphore;

void setDebugUartHandler(UART_HandleTypeDef * huart) {
	localUartHandler = huart;
}

bool pushMessage(const char text[lineLength]) {
  if (xQueueSendToBackFromISR(queue, text, 0) == pdPASS) {
    messageCount++;
    return true;
  } else {
    return false;
  }
}

void WriteDebug(void const * argument) {
	if (localUartHandler == NULL) {
		return;
	}
  queue = xQueueCreate(bufferSize, sizeof(char) * lineLength);
  pushMessage("UART initialization complete!\r\n");
  HAL_UART_Receive_IT(localUartHandler, &rxBuf, 1);

  for(;;) {
    osDelay(1000);
    HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
    if (messageCount > 0) {
      int count = messageCount;
      for (int i = 0; i < count; ++i) {
        char local[256];
        xQueueReceive(queue, &local, portMAX_DELAY);
        HAL_UART_Transmit(localUartHandler, (uint8_t *) local, strlen(local), 120);
        messageCount--;
      }
    }
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == localUartHandler->Instance) {
    if (rxBuf == '\n' || rxBuf == '\r') {
      strcat(receiveBuffer, "\r\n");
      pushMessage(receiveBuffer);
      strcpy(receiveBuffer, "");
    } else {
      int len = strlen(receiveBuffer);
      receiveBuffer[len] = rxBuf;
      receiveBuffer[len+1] = '\0';;
    }
    HAL_UART_Receive_IT(localUartHandler, &rxBuf, 1);
  }
}
