#include "main.h"

volatile uint32_t sysTime = 0;
volatile uint32_t timeMask = BLINK_MAIN;

extern uint8_t ready_Inclinom_Handler;
extern uint32_t state_Inclinom_Handler;
extern uint32_t timeLastReq;

static const unsigned char init_mes[] = "0 - Successful initialization.";
static const unsigned char ver_mes[]  = "0 - Firmware ver. 1.0.01.";

void SysTick_Handler (void)
{
	sysTime++;

	if(sysTime == timeMask)
		System_Blink();

	if(state_Inclinom_Handler)
	{
		if(sysTime - timeLastReq > 9000)
		{
			Over_Time_Inclinometer();
		}
	}
}

int main(void)
{
	SysClockConfig();

	SysTick_Config(SystemCoreClock/1000);
	NVIC_EnableIRQ(SysTick_IRQn);

	USART2_Config();
	USART3_Config();
	UART4_Config();
	UART5_Config();
	USART6_Config();

	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_EnableIRQ(USART3_IRQn);
	NVIC_EnableIRQ(UART4_IRQn);
	NVIC_EnableIRQ(UART5_IRQn);
	NVIC_EnableIRQ(USART6_IRQn);

	OnBoard_Leds_Initialize();
	Init_TIM6();

	//UART5_Start_Message(&ver_mes, sizeof(ver_mes));
	//UART5_Start_Message(&init_mes, sizeof(init_mes));

	USART6_Start_Message(&init_mes, sizeof(init_mes));

	Start_TIM6();

	/* Loop forever */
	while(1)
	{

		if(ready_Inclinom_Handler)
		{
			Inclinometer_Handler();
		}

	}
}


