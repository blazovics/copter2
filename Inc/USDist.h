/*
 * USDist.h
 *
 *  Created on: 2017. márc. 12.
 *      Author: Dani
 */

#ifndef USDIST_H_
#define USDIST_H_

#define NUMBER_RX_DATA		5

typedef enum{
	FREE,
	RX_START,
	RX_END
}ReceiveState;

void GetDistance(void const * argument);

#endif /* USDIST_H_ */
