
/********************************************************
*	Created on: May 28, 2021
*	by Verekh Roman
*
*   current version 1.0.0.1 from May 28, 2021
*********************************************************/

#include "stm32f4xx.h"
#include "Led.h"

void OnBoard_Leds_Initialize (void)
{
	/* Enable GPIOE clock */
	RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOEEN;

	GPIOE ->ODR |= (1 << LED1) | (1 << LED2);

	/* Configure LED (PE0.. PE1) pins as push-pull outputs */
	GPIOE -> MODER   &= ~((DMASK << (LED1*2)) | (DMASK << (LED2*2)));
	GPIOE -> MODER   |=   (OUTPUT_MODE << (0)) | (OUTPUT_MODE << (2));
	GPIOE -> OTYPER  &= ~((MASK << LED1) | (MASK << LED2));
	GPIOE -> OTYPER  |=   (PUSH_PULL << LED1) | (PUSH_PULL << LED2);
	GPIOE -> OSPEEDR |=   (VERY_HIGH_SPEED << LED1) | (VERY_HIGH_SPEED << LED2);
}

