/*
 * IMUTask.c
 *
 *  Created on: 2017. febr. 14.
 *      Author: blazovics
 *
 */

#include <IMUTask.h>
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "imu.h"

/*
 * Task function for managing IMU data reading
 */
extern UART_HandleTypeDef huart3;
IMU_gyro_t gyroData;
IMU_accel_t accelData;

#ifdef DEBUG
IMU_gyroTest_t gyroTestData;
IMU_accelTest_t accelTestData;
#endif

void ReadIMU(void const * argument)
{
	//HAL_StatusTypeDef status;
	int currIndex;
	/* Infinite loop */

	if(!IMU_initReady())
	{
		if(!IMU_Init())
		{
#ifdef DEBUG
			while(1) {}
#endif
		}
#ifdef DEBUG
		else
		{
			/* init test structures */
			gyroTestData.index = 0;
			accelTestData.index = 0;
		}
#endif
	}
	for(;;)
	{
		osDelay(1000);
		//char test = 'k';
		if(!IMU_FIFOread(&accelData, &gyroData))
		{
#ifdef DEBUG
			while(1) {}
#endif
		}
#ifdef DEBUG
		else
		{
			/* save data in test buffer */
			currIndex = accelTestData.index;
			if(currIndex == 20)
			{
				//HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7, GPIO_PIN_SET);
				while(1){}
			}
			accelTestData.index++;
			accelTestData.accelTestArr[currIndex].x = accelData.x;
			accelTestData.accelTestArr[currIndex].y = accelData.y;
			accelTestData.accelTestArr[currIndex].z = accelData.z;

			currIndex = gyroTestData.index;
			gyroTestData.index++;
			gyroTestData.gyroTestArr[currIndex].x = gyroData.x;
			gyroTestData.gyroTestArr[currIndex].y = gyroData.y;
			gyroTestData.gyroTestArr[currIndex].z = gyroData.z;

		}
#endif
/*		status = HAL_UART_Transmit(&huart3, (uint8_t *)&test, 1, 120);
		if(status != HAL_OK)
		{
#ifdef DEBUG
			while(1)
			{
			}
#endif
		} */
	}
}
