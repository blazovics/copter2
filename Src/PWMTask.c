/*
 * PWMTask.c
 *
 *  Created on: 2017. febr. 14.
 *      Author: blazovics
 */

#include "PWMTask.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal.h"

/*
 * Test variable
 */
#define PWM_TEST

/*
 * External variable:
 * pwmSemaphore: value from the pwm duty-cycle calculation task (between 0 and 1)
 */
extern SemaphoreHandle_t pwmSemaphore;
extern TIM_HandleTypeDef htim2;


//Global variable

#ifndef PWM_TEST
	double PwmDuty[] = { 0 , 0 , 0 , 0 };
#endif

//Test variable (available when PWM_TEST constant is defined
#ifdef PWM_TEST
	double PwmDuty[] = { 0 , 0.3 , 0.7 , 1 };
#endif

//f=1kHz, when clock frequency is 16MHz and timer uses no prescaler
static int PwmPeriod = 15999;

//Copy of the common resource PwmDuty
volatile double PwmCopy[] = { 0 , 0 , 0 , 0 };


/*
 * Task function for managing PWM outputs
 */
void SetPWMOutput(void const * argument)
{
	int i = 0;

	__HAL_TIM_SET_AUTORELOAD(&htim2, PwmPeriod);

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

	/* Infinite loop */
	for(;;)
	{
		if(xSemaphoreTake(pwmSemaphore , 0) == pdTRUE)
		{
			while(i < 4)
			{
				PwmCopy[i] = PwmDuty[i];
				i++;
			}
			i = 0;
			xSemaphoreGive( pwmSemaphore );

			ModifyPwmDuty();
		}

		osDelay(1000);
		HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_7);
	}
}

void ModifyPwmDuty()
{
	int CompareValue[] = { 0 , 0 , 0 , 0 };
	int i = 0;

	while(i < 4)
	{
		CompareValue[i] = (int) (PwmCopy[i] * PwmPeriod);
		i++;
	}

	taskENTER_CRITICAL();

	htim2.Instance->CCR1 = CompareValue[0];
	htim2.Instance->CCR2 = CompareValue[1];
	htim2.Instance->CCR3 = CompareValue[2];
	htim2.Instance->CCR4 = CompareValue[3];

	taskEXIT_CRITICAL();

}
