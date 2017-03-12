/*
 * imu.h
 *
 *  Created on: 2017. m�rc. 4.
 *      Author: Balazs
 */

#include "stm32f7xx_hal.h"
#include <stdbool.h>

#ifndef IMU_H_
#define IMU_H_

#define DEBUG

#define UINT8_TO_UINT16(H, L)			(uint16_t)(((0x0000 | (H)) << 8 ) | (L))

#define IMU_INT_PIN						GPIO_PIN_4 /* interrupt pin configured for MPU-6050 */

#define IMU_I2C_ADDR  					0x68 /*AD0 is GND */
/* Slave 0 addresses */
#define IMU_I2C_SLV0_ADDR 				0x25 /* I2C address of Slave 0 */
#define IMU_I2C_SLV0_REG  				0x26 /* internal register within Slave 0 */
#define IMU_I2C_SLV1_CTRL 				0x27 /* Slave 0 control register address */
#define IMU_I2C_SLV1_DO	  				0x63 /* Slave 0 data register address */

/*Slave 1 addresses */
#define I2C_SLV1_ADDR	  				0x28 /* I2C address of Slave 1 */
#define I2C_SLV1_REG    				0x29 /* internal register within Slave 1 */
#define I2C_SLAVE1_CTRL 	  			0x2A /* Slave 1 control register address */
#define I2C_SLAVE1_DO  	   				0x64 /* Slave 1 data register address */

/* register addresses and their command bytes */
#define IMU_REG_POWER_MGMT				0x6B
#define IMU_CMD_RESET					0x80 /* reset device */
#define IMU_CMD_WAKEUP					0x03 /* set gyro Z-axis CLK as clock reference  !!!*/

#define IMU_REG_SAMPLE_RATE_DIV			0x19
#define IMU_CMD_SAMPLE_RATE_SET			0x4F /* SR = 80kHz / (1 + IMU_CMD_SAMPLE_RATE) = 100 Hz */

#define IMU_REG_CONFIG					0x1A
#define IMU_CMD_ENABLE_LPF				0x04 /* Low Pass Filter accel: 21 Hz BW, 8.5 ms delay; gyro: 20 Hz BW, 8.3 ms delay */

#define IMU_REG_GYRO_CONFIG				0x1B
#define IMU_CMD_GYRO_CONFIG				0x10 /* full scale range +1000% w/o self test*/

#define IMU_REG_ACCEL_CONFIG			0x1C
#define IMU_CMD_ACCEL_CONFIG			0x10 /* +-8g FS range w/o self test */

#define IMU_REG_FIFO_EN					0x23
#define IMU_CMD_FIFO_EN					0x78 /* enables Gyro x,y,z and Accel measurements to be written into the FIFO */

#define IMU_REG_INT_CONFIG				0x37
#define IMU_CMD_INT_CONFIG				0x10 /* INT will be cleared by any read operation */

#define IMU_REG_INT_ENABLE				0x38
#define IMU_CMD_INT_SETUP				0x01 /* INT will be generated, when all the measurements are ready */

#define IMU_REG_FIFO_COUNT_H			0x72 /* FIFO length registers */
#define IMU_REG_FIFO_COUNT_L			0x73

#define IMU_REG_FIFO_RW					0x74 /* FIFO read/write register address */



/* type definitions */
typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
}IMU_accel_t;

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
}IMU_gyro_t;

#ifdef DEBUG
typedef struct{
	int index;
	IMU_gyro_t gyroTestArr[20];
}IMU_gyroTest_t;

typedef struct{
	int index;
	IMU_accel_t accelTestArr[20];
}IMU_accelTest_t;

#endif

/* API declaration */
bool IMU_Init(void);
bool IMU_FIFOread(IMU_accel_t* accelData, IMU_gyro_t* gyroData);
bool IMU_initReady(void);

#endif /* IMU_H_ */
