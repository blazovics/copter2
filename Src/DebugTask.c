#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <machine/endian.h>
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

// copied for testing from http://stackoverflow.com/a/10621440
static float htonf(float val) {
    uint32_t rep;
    memcpy(&rep, &val, sizeof rep);
    rep = __htonl(rep);
    memcpy(&val, &rep, sizeof rep);
    return val;
}

static int getDebugLine(char buffer[lineLength]) {
    int count = debugMessageCount;
    int index;
    for (index = 0; index < 4; ++index) {
    	buffer[index] = ~0;
    }
    for (int i = 0; i < count && index + sizeof(uint32_t) + 2 * sizeof(char) + 4 * sizeof(float) < lineLength ; ++i) {
      DebugMessage local;
      xQueueReceive(debugQueue, &local, portMAX_DELAY);

      local.timestamp = __htonl(local.timestamp);
      memcpy(buffer + index, &local.timestamp, sizeof(uint32_t));
      index += sizeof(uint32_t);
      memcpy(buffer + index, &local.messageId, sizeof(char));
      index += sizeof(char);
      memcpy(buffer + index, &local.dataCount, sizeof(char));
      index += sizeof(char);
      for (int i = 0; i < 4; ++i) {
    	  local.data[i] = htonf(local.data[i]);
          memcpy(buffer + index, &local.data[i], sizeof(float));
          index += sizeof(float);
      }

      debugMessageCount--;
    }
    return index;
}

void WriteDebug(void const * argument) {
	if (localUartHandler == NULL) {
		return;
	}
  queue = xQueueCreate(bufferSize, sizeof(char) * lineLength);
  debugQueue = xQueueCreate(bufferSize, sizeof(DebugMessage));
  // pushMessage("UART initialization complete!\r\n");
  HAL_UART_Receive_IT(localUartHandler, &rxBuf, 1);

  /*
  DebugMessage test;
  test.dataCount = '4';
  test.messageId = 'x';
  test.timestamp = xTaskGetTickCount();
  test.data[0] = 1.0;
  test.data[1] = 3.14;
  test.data[2] = 6.28;
  test.data[3] = 0;
  pushDebugMessage(test);
  */

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
    	int len = getDebugLine(buffer);
    	HAL_UART_Transmit(localUartHandler, (uint8_t *) buffer, len, 120);
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
