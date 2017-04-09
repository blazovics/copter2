#ifndef DEBUGTASK_H_
#define DEBUGTASK_H_

#define lineLength 256
#define bufferSize 100

#include <stdbool.h>
#include <stdint.h>

#include "stm32f7xx_hal.h"

typedef struct {
	char messageId;
	uint32_t timestamp;
	char dataCount;
	float data[4];
} DebugMessage;

void setDebugUartHandler(UART_HandleTypeDef * huart);
bool pushMessage(const char text[lineLength]);
void WriteDebug(void const * argument);

#endif
