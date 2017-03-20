/*
 * imu.c
 *
 *  Created on: 2017. m�rc. 4.
 *      Author: Balazs
 */
#include "imu.h"

extern I2C_HandleTypeDef hi2c1;

bool IMU_measReady;
bool IMU_init = false;


/*non public function definitions */

/* write a sensor register over I2C */
HAL_StatusTypeDef IMU_registerWrite(uint8_t slaveAddr, uint8_t regAddr, uint8_t data)
{
	uint16_t devAddr = (0x0000 | slaveAddr) << 1;
	uint16_t regAddrTx = (uint16_t) regAddr;
	HAL_StatusTypeDef retVal = HAL_OK;
	uint8_t txData = data;

	//retVal = HAL_I2C_Master_Transmit(&hi2c1, devAddr, &data2Transmit[0], 2, 0xFFFF);
	retVal = HAL_I2C_Mem_Write(&hi2c1, devAddr, regAddrTx, 1, &txData, 1, 0xFFFF);

	return retVal;
};

/* read a sensor register over I2C */
HAL_StatusTypeDef IMU_registerRead(uint8_t slaveAddr, uint8_t regAddr, uint8_t* data)
{
	uint16_t devAddr = (0x0000 | slaveAddr) << 1;
	HAL_StatusTypeDef retVal = HAL_OK;

	retVal = HAL_I2C_Mem_Read(&hi2c1, devAddr, regAddr, 1, data, 1, -1);

	return retVal;
};

/* burst read register over I2C */
HAL_StatusTypeDef IMU_burstRead(uint8_t slaveAddr, uint8_t regAddr, uint8_t* data, uint16_t length)
{
	uint16_t devAddr = (0x0000 | slaveAddr) << 1;
	HAL_StatusTypeDef retVal = HAL_OK;

	retVal = HAL_I2C_Mem_Read(&hi2c1, devAddr, regAddr, 1, data, length, -1);

	return retVal;
}

/* initializes IMU with handle for I2C1 as parameter*/
bool IMU_Init(void)
{
	bool errFlag = false;
	/* set ready flag to 0 */
	IMU_measReady = false;
	IMU_init = false;

	/* reset MPU6050 */
	if(IMU_registerWrite(IMU_I2C_ADDR, IMU_REG_POWER_MGMT, IMU_CMD_RESET) != HAL_OK)
	{
		errFlag = true;
	}
	HAL_Delay(100);

	/*wake up MPU6050 */
	if(IMU_registerWrite(IMU_I2C_ADDR, IMU_REG_POWER_MGMT, IMU_CMD_WAKEUP) != HAL_OK)
	{
		errFlag = true;
	}
	HAL_Delay(100);

	/* set sample rate to 100 Hz */
	if(IMU_registerWrite(IMU_I2C_ADDR, IMU_REG_SAMPLE_RATE_DIV, IMU_CMD_SAMPLE_RATE_SET)!= HAL_OK)
	{
		errFlag = true;
	}
	/* configure LPF */
	if(IMU_registerWrite(IMU_I2C_ADDR, IMU_REG_CONFIG, IMU_CMD_ENABLE_LPF)!= HAL_OK)
	{
		errFlag = true;
	}
	/* configure gyroscope */
	if(IMU_registerWrite(IMU_I2C_ADDR, IMU_REG_GYRO_CONFIG, IMU_CMD_GYRO_CONFIG)!= HAL_OK)
	{
		errFlag = true;
	}
	/* configure acceleration sensor */
	if(IMU_registerWrite(IMU_I2C_ADDR, IMU_REG_ACCEL_CONFIG, IMU_CMD_ACCEL_CONFIG)!= HAL_OK)
	{
		errFlag = true;
	}
	/* enable FIFO */
	if(IMU_registerWrite(IMU_I2C_ADDR, IMU_REG_FIFO_EN, IMU_CMD_FIFO_EN)!= HAL_OK)
	{
		errFlag = true;
	}
	/* interrupt is cleared by any read operation */
	if(IMU_registerWrite(IMU_I2C_ADDR, IMU_REG_INT_CONFIG, IMU_CMD_INT_CONFIG)!= HAL_OK)
	{
		errFlag = true;
	}
	/* interrupts are generated if every measurement is ready */
	if(IMU_registerWrite(IMU_I2C_ADDR, IMU_REG_INT_ENABLE, IMU_CMD_INT_SETUP)!= HAL_OK)
	{
		errFlag = true;
	}

	/* enable external interrupts from EXTI4 with the lowest priority */
	HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	if(!errFlag)
	{
		IMU_init = true;
	}
	return (!errFlag);
};

bool IMU_initReady(void)
{
	return IMU_init;
}

bool IMU_measurementReady(void)
{
	return IMU_measReady;
}

uint16_t IMU_getSampleNum(void)
{
	uint16_t retVal = 0x0000;
	uint8_t sampleNumH, sampleNumL, tmp;

	if(!IMU_init)
	{
		if(!IMU_Init())
		{
#ifdef DEBUG
			while(1){};
#endif //DEBUG
		}
	}
	if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_FIFO_COUNT_H, &tmp) == HAL_OK)
	{
		sampleNumH = tmp;
		if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_FIFO_COUNT_L, &tmp) == HAL_OK)
		{
			sampleNumL = tmp;
			retVal = UINT8_TO_UINT16(sampleNumH, sampleNumL);
		}
		else
		{
			retVal = 0xFFFF;
		}
	}
	else
	{
		retVal = 0xFFFF;
	}
	return retVal;
}

bool IMU_FIFOread(IMU_accel_t* accelData, IMU_gyro_t* gyroData)
{
	uint16_t sampleNum;
	bool retVal = false;
	uint8_t data[12];

	sampleNum = IMU_getSampleNum();
	/*check if transaction was successful and buffer isn't empty */
	if(sampleNum != 0xFFFF && sampleNum != 0)
	{
		/* disable interrupts here */
		if(IMU_burstRead(IMU_I2C_ADDR, IMU_REG_FIFO_RW, data, 12) == HAL_OK)
		{
			accelData->x = UINT8_TO_UINT16(data[0], data[1]);
			accelData->y = UINT8_TO_UINT16(data[2], data[3]);
			accelData->z = UINT8_TO_UINT16(data[4], data[5]);
			gyroData->x  = UINT8_TO_UINT16(data[6], data[7]);
			gyroData->y  = UINT8_TO_UINT16(data[8], data[9]);
			gyroData->z  = UINT8_TO_UINT16(data[10], data[11]);

			retVal = true;
		}
		else
		{
			retVal = false;
		}
	}
	else
	{
		retVal = false;
	}
	return retVal;
}

bool IMU_sensRegRead(IMU_accel_t* accelData, IMU_gyro_t* gyroData)
{
	//uint8_t tempH, tempL;
	bool retVal = true;
	uint8_t data[14];

	if(IMU_burstRead(IMU_I2C_ADDR, IMU_REG_ACCEL_X_H, data, 14) == HAL_OK)
	{
		accelData->x = UINT8_TO_UINT16(data[0], data[1]);
		accelData->y = UINT8_TO_UINT16(data[2], data[3]);
		accelData->z = UINT8_TO_UINT16(data[4], data[5]);

		gyroData->x  = UINT8_TO_UINT16(data[8], data[9]);
		gyroData->y  = UINT8_TO_UINT16(data[10], data[11]);
		gyroData->z  = UINT8_TO_UINT16(data[12], data[13]);
	}
	else
	{
		retVal = false;
	}

	return retVal;
//	/* read gyro data */
//	/* X */
//	if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_GYRO_X_H, &tempH) == HAL_OK)
//	{
//		if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_GYRO_X_L, &tempL) == HAL_OK)
//		{
//			gyroData->x = UINT8_TO_UINT16(tempH, tempL);
//		}
//		else
//		{
//			retVal = false;
//		}
//	}
//	else
//	{
//		retVal = false;
//	}
//	/* Y */
//	if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_GYRO_Y_H, &tempH) == HAL_OK)
//	{
//		if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_GYRO_Y_L, &tempL) == HAL_OK)
//		{
//			gyroData->y = UINT8_TO_UINT16(tempH, tempL);
//		}
//		else
//		{
//			retVal = false;
//		}
//	}
//	else
//	{
//		retVal = false;
//	}
//	/* Z */
//	if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_GYRO_Z_H, &tempH) == HAL_OK)
//	{
//		if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_GYRO_Z_L, &tempL) == HAL_OK)
//		{
//			gyroData->z = UINT8_TO_UINT16(tempH, tempL);
//		}
//		else
//		{
//			retVal = false;
//		}
//	}
//	else
//	{
//		retVal = false;
//	}
//
//	/*read acceleration data */
//	/* X */
//	if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_ACCEL_X_H, &tempH) == HAL_OK)
//	{
//		if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_ACCEL_X_L, &tempL) == HAL_OK)
//		{
//			accelData->x = UINT8_TO_UINT16(tempH, tempL);
//		}
//		else
//		{
//			retVal = false;
//		}
//	}
//	else
//	{
//		retVal = false;
//	}
//	/* Y */
//	if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_ACCEL_Y_H, &tempH) == HAL_OK)
//	{
//		if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_ACCEL_Y_L, &tempL) == HAL_OK)
//		{
//				accelData->y = UINT8_TO_UINT16(tempH, tempL);
//		}
//		else
//		{
//			retVal = false;
//		}
//	}
//	else
//	{
//		retVal = false;
//	}
//	/* Z */
//	if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_ACCEL_Z_H, &tempH) == HAL_OK)
//	{
//		if(IMU_registerRead(IMU_I2C_ADDR, IMU_REG_ACCEL_Z_L, &tempL) == HAL_OK)
//		{
//			accelData->z = UINT8_TO_UINT16(tempH, tempL);
//		}
//		else
//		{
//			retVal = false;
//		}
//	}
//	else
//	{
//		retVal = false;
//	}


}
/* external interrupt handler function */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_4)
  {
    /* set global flag */
	IMU_measReady = true;
  }
}

