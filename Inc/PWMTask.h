/*
 * PWMTask.h
 *
 *  Created on: 2017. febr. 14.
 *      Author: blazovics
 */

#ifndef PWMTASK_H_
#define PWMTASK_H_

/*
 * PWM Task function
 */
void SetPWMOutput(void const * argument);

/*
 * Calculate and set TIM2 CCRx values
 */
void ModifyPwmDuty();

#endif /* PWMTASK_H_ */
