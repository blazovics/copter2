/*
 * USDist.h
 *
 *  Created on: 2017. márc. 12.
 *      Author: Dani
 */

#ifndef USDIST_H_
#define USDIST_H_

#define NUMBER_RX_DATA		5

#include <stdint.h>

typedef enum{
	FREE,
	RX_START,
	RX_END
}ReceiveState;

void GetDistance(void const * argument);

void Uart7SeperateCallback(void);

void MyIntToCharArray5(uint32_t number, char* buffer);

#endif /* USDIST_H_ */
