#ifndef DEBUGTASK_H_
#define DEBUGTASK_H_

#define lineLength 256
#define bufferSize 100

#include <stdbool.h>

void MX_USART1_UART_Init(void);
bool pushMessage(const char text[lineLength]);
void WriteDebug(void const * argument);

#endif
