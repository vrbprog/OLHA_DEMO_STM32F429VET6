/*
 * Uart4.c
 *
 *  Created on: May 31, 2021
 *      Author: User
 */

#include  "stm32f4xx.h"
#include  "Uart4.h"
#include  "Led.h"

extern 	uint32_t state_Inclinom_Handler;
extern 	uint32_t sysTime;

volatile uint8_t ready_Inclinom_Handler = 0;

		char resive_pack_Uart4[32];
		char *rxCharUart4    = (char*) &resive_pack_Uart4;
static 	char *txCharUart4    = (char*) &resive_pack_Uart4;
static 	char *txCharEndUart4 = (char*) &resive_pack_Uart4;

uint8_t txStateUart4;

void UART4_Config (void)
{
		/******* STEPS FOLLOWED ********

	1. Enable the UART CLOCK and GPIO CLOCK
	2. Configure the UART PINs for ALternate Functions
	3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
	4. Program the M bit in USART_CR1 to define the word length.
	5. Select the desired baud rate using the USART_BRR register.
	6. Enable the Transmitter/Receiver by Setting the TE and RE bits in USART_CR1 Register

	********************************/

	// 1. Enable the UART CLOCK and GPIO CLOCK
		RCC->APB1ENR |= (1 << UART4_CLK_EN); // Enable UART2 CLOCK
		RCC->AHB1ENR |= (1 << GPIOA_CLK_EN); // Enable GPIOA CLOCK

		// 2. Configure the UART PINs for ALternate Functions
		GPIOA->MODER |= (ALTF_MODE << PA0*2);  // Bits (1:0)= 1:0 --> Alternate Function for Pin PA0
		GPIOA->MODER |= (ALTF_MODE << PA1*2);  // Bits (3:2)= 1:0 --> Alternate Function for Pin PA1

		GPIOA->OSPEEDR |= (VERY_HIGH_SPEED << PA0*2);  // Bits (1:0)= 1:1  --> High Speed for PIN PA0
		GPIOA->OSPEEDR |= (VERY_HIGH_SPEED << PA1*2);  // Bits (3:2)= 1:1  --> High Speed for PIN PA1

		GPIOA->AFR[0] |= (ALTF_UART4 << PA0*4); // Bits (3:2:1:0) = 1:0:0:0 --> AF7 Alt function UART4 at Pin PA0
		GPIOA->AFR[0] |= (ALTF_UART4 << PA1*4); // Bits (7:6:5:4) = 1:0:0:0 --> AF7 Alt function UART4 at Pin PA1

		GPIOA->PUPDR &= ~(DMASK   << PA0*2);
		GPIOA->PUPDR |=  (PULL_UP << PA0*2);

		// 3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
		UART4->CR1  = 0x00;  // clear all
		UART4->CR1 |= (1<<13);  // UE = 1... Enable UART4

		// 4. Program the M bit in USART_CR1 to define the word length.
		UART4->CR1 &= ~(1<<12);  // M =0; 8 bit word length

		// 5. Select the desired baud rate using the USART_BRR register.
		UART4->BRR = (7<<0) | (24<<4);   // Baud rate of 115200, PCLK1 at 45MHz

		// 6. Enable the Transmitter/Receiver by Setting the TE and RE bits in USART_CR1 Register
		UART4->CR1 |= (1<<2); // RE=1.. Enable the Receiver
		UART4->CR1 |= (1<<3); // TE=1.. Enable Transmitter

		// 7. Enable TX Interrupt
		//USART2->CR1 |= USART_CR1_TXEIE;
		UART4->CR1  |= USART_CR1_RXNEIE;
		txStateUart4 = TX4_IDLE;
}

void UART4_IRQHandler(void)
{
	char temp;

	if(UART4->SR & USART_SR_RXNE)
	{
		temp = UART4->DR;

		if(state_Inclinom_Handler)
		{
			(*rxCharUart4++) = temp;
			ready_Inclinom_Handler = 1;
		}
	}

	if(txStateUart4 == TX4_TRANS)
	{
		if(UART4->SR & USART_SR_TXE)
		{
			if(txCharUart4 != txCharEndUart4)
			{
				UART4->DR = (*txCharUart4++);
			}
			else
			{
				//timeLastReq = sysTime;
				UART4->CR1 &= ~USART_CR1_TXEIE;
				txStateUart4 = TX4_IDLE;
			}
		}
	}
}

void UART4_Start_Message(const void* adress, uint8_t len)
{
	txCharUart4 = (char*)adress;
	txCharEndUart4 = (char*)adress + len;
	UART4->DR = (*txCharUart4++);
	UART4->CR1 |= USART_CR1_TXEIE;
	txStateUart4 = TX4_TRANS;
}


