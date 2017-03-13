#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "DebugTask.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "cmsis_os.h"
#include "queue.h"

static UART_HandleTypeDef * localUartHandler = NULL;

static QueueHandle_t queue;
static unsigned int messageCount = 0;

static QueueHandle_t debugQueue;
static unsigned int debugMessageCount = 0;

static volatile char rxBuf;
static char receiveBuffer[lineLength] = "";

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

bool pushDebugMessage(DebugMessage message) {
	  if (xQueueSend(debugQueue, &message, 0) == pdPASS) {
	    debugMessageCount++;
	    return true;
	  } else {
	    return false;
	  }
}

static void getDebugLine(char buffer[lineLength]) {
    int count = debugMessageCount;
    int index;
    for (index = 0; index < 4; ++index) {
    	buffer[index] = ~0;
    }
    for (int i = 0; i < count && index + 10 + 2 + 20 * 4 < lineLength ; ++i) {
      DebugMessage local;
      xQueueReceive(debugQueue, &local, portMAX_DELAY);
      // 10 karakter, mert 0xffffffff == 4294967295 (10 karakter)
      sprintf(&buffer[index], "%10lu", local.timestamp);
      index += 10;
      sprintf(&buffer[index], "%c%c", local.messageId, local.dataCount);
      index += 2;
      sprintf(&buffer[index], "%20.10f%20.10f%20.10f%20.10f", local.data[0], local.data[1], local.data[2], local.data[3]);
      index += 20 * 4;
      debugMessageCount--;
    }
    buffer[index] = '\0';
}

void WriteDebug(void const * argument) {
	if (localUartHandler == NULL) {
		return;
	}
  queue = xQueueCreate(bufferSize, sizeof(char) * lineLength);
  debugQueue = xQueueCreate(bufferSize, sizeof(DebugMessage));
  pushMessage("UART initialization complete!\r\n");
  HAL_UART_Receive_IT(localUartHandler, &rxBuf, 1);


  DebugMessage test;
  test.dataCount = '4';
  test.messageId = 'x';
  test.timestamp = xTaskGetTickCount();
  test.data[0] = 1.0;
  test.data[1] = 3.14;
  test.data[2] = 6.28;
  test.data[3] = 0;
  pushDebugMessage(test);

  for(;;) {
    osDelay(1000);
    HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);

    // send text messages
    if (messageCount > 0) {
      int count = messageCount;
      for (int i = 0; i < count; ++i) {
        char local[lineLength];
        xQueueReceive(queue, &local, portMAX_DELAY);
        HAL_UART_Transmit(localUartHandler, (uint8_t *) local, strlen(local), 120);
        messageCount--;
      }
    }

    // send debug messages
    if (debugMessageCount > 0) {
    	char buffer[lineLength];
    	getDebugLine(buffer);
    	HAL_UART_Transmit(localUartHandler, (uint8_t *) buffer, strlen(buffer), 120);
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
      receiveBuffer[len+1] = '\0';
    }
    HAL_UART_Receive_IT(localUartHandler, &rxBuf, 1);
  }
}
