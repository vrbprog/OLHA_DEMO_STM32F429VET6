/*
 * Timer6.c
 *
 *  Created on: Jun 1, 2021
 *      Author: User
 */

#include  "stm32f4xx.h"
#include  "Led.h"
#include  "Usart2.h"
#include "Inclinometer.h"

static char flag_start = 0;

void Init_TIM6(void) {

	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	TIM6->PSC = 45000 - 1;
	TIM6->ARR = 20000 - 1;
	TIM6->DIER |= TIM_DIER_UIE;
	TIM6->CR1 =  TIM_CR1_ARPE;

	NVIC_EnableIRQ(TIM6_DAC_IRQn);

}

void Start_TIM6(void)
{
	TIM6->CR1 |= TIM_CR1_CEN;
}

void Stop_TIM6(void)
{
	TIM6->CR1 &= ~TIM_CR1_CEN;
}

void TIM6_DAC_IRQHandler(void)
{
		uint32_t temp = TIM6->SR;
		TIM6->SR = ~TIM_SR_UIF;
		temp &= TIM6->DIER;

		if(temp & TIM_SR_UIF){
			if(flag_start)
			{
				//Inclinometer_Request_Zero();
				Inclinometer_Read_XY();
			}
			else
			{
				flag_start = 1;
			}
		}

}
