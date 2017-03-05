#include <string.h>
#include <stdbool.h>
#include "DebugTask.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"
#include "cmsis_os.h"
#include "queue.h"

UART_HandleTypeDef huart1;

QueueHandle_t queue;
unsigned int messageCount = 0;

volatile char rxBuf;
char receiveBuffer[lineLength] = "";

void MX_USART1_UART_Init(void) {
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
  if (HAL_UART_Init(&huart1) != HAL_OK) {
    Error_Handler();
  }
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
  queue = xQueueCreate(bufferSize, sizeof(char) * lineLength);
  pushMessage("UART initialization complete!\r\n");
  HAL_UART_Receive_IT(&huart1, &rxBuf, 1);

  for(;;) {
    osDelay(1000);
    HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
    if (messageCount > 0) {
      int count = messageCount;
      for (int i = 0; i < count; ++i) {
        char local[256];
        xQueueReceive(queue, &local, portMAX_DELAY);
        HAL_UART_Transmit(&huart1, (uint8_t *) local, strlen(local), 120);
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
    HAL_UART_Receive_IT(&huart1, &rxBuf, 1);
  }
}
