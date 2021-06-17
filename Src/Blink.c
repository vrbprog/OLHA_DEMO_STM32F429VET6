/*
 * Blink.c
 *
 *  Created on: Jun 3, 2021
 *      Author: User
 */

#include  "stm32f4xx.h"
#include  "stdint.h"
#include  "Led.h"
#include  "Blink.h"

extern uint32_t timeMask;
static uint8_t ticks = 0;
uint8_t modeBlink = 2;

void System_Blink (void)
{

	//static uint8_t stateBlink = 0;

	switch(modeBlink)
	{
		case 1:
		{
			Blink_main();
			break;
		}

		case 2:
		{
			Blink_two();
			break;
		}


		default: ;
	}
}

void Blink_main(void)
{
	if(ticks == 1)
	{
		ticks = 0;

		GPIOE ->ODR &= ~(HIGH << LED1);
	}
	else
	{
		ticks = 1;

		GPIOE ->ODR |=   HIGH << LED1;
	}
	timeMask += BLINK_MAIN;
}

void Blink_two(void)
{
	static uint8_t count_two = 0;
	if(ticks == 1)
	{
		ticks = 0;

		GPIOE ->ODR |=   HIGH << LED1;

		if(count_two == 1) {
			count_two = 0;
			timeMask += BLINK_MAIN + BLINK_TWO;
		}
		else{
			count_two = 1;
			timeMask += BLINK_TWO;
		}
	}
	else
	{
		ticks = 1;

		GPIOE ->ODR &= ~(HIGH << LED1);

		timeMask += BLINK_TWO;
	}
}
