/*
 * Usart3.c
 *
 *  Created on: May 31, 2021
 *      Author: User
 */

#include  "stm32f4xx.h"
#include  "Usart3.h"
#include  "Led.h"

char resive_pack_Usart3[32];
char *rxCharUsart3 = (char*) &resive_pack_Usart3;
char *txCharUsart3 = (char*) &resive_pack_Usart3;

uint8_t txStateUsart3;

void USART3_Config (void)
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
	RCC->APB1ENR |= (1 << USART3_CLK_EN); // Enable UART2 CLOCK
	RCC->AHB1ENR |= (1 << GPIOD_CLK_EN); // Enable GPIOA CLOCK

	// 2. Configure the UART PINs for ALternate Functions
	GPIOD->MODER |= (ALTF_MODE << PD8*2);  // Bits (5:4)= 1:0 --> Alternate Function for Pin PD8
	GPIOD->MODER |= (ALTF_MODE << PD9*2);  // Bits (7:6)= 1:0 --> Alternate Function for Pin PD9

	GPIOD->OSPEEDR |= (VERY_HIGH_SPEED << PD8*2);  // Bits (5:4)= 1:1  --> High Speed for PIN PD8
	GPIOD->OSPEEDR |= (VERY_HIGH_SPEED << PD9*2);  // Bits (7:6)= 1:1  --> High Speed for PIN PD9

	GPIOD->AFR[1] |= (ALTF_UART3 << 0*4); // Bits (11:10:9:8)   = 0:1:1:1 --> AF7 Alt function USART3 at Pin PD8
	GPIOD->AFR[1] |= (ALTF_UART3 << 1*4); // Bits (15:14:13:12) = 0:1:1:1 --> AF7 Alt function USART3 at Pin PD9

	GPIOD->PUPDR &= ~(DMASK   << PD8*2);
	GPIOD->PUPDR |=  (PULL_UP << PD8*2);

	// 3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
	USART3->CR1  = 0x00;  // clear all
	USART3->CR1 |= (1<<13);  // UE = 1... Enable USART3

	// 4. Program the M bit in USART_CR1 to define the word length.
	USART3->CR1 &= ~(1<<12);  // M =0; 8 bit word length

	// 5. Select the desired baud rate using the USART_BRR register.
	USART3->BRR = (7<<0) | (24<<4);   // Baud rate of 115200, PCLK1 at 45MHz

	// 6. Enable the Transmitter/Receiver by Setting the TE and RE bits in USART_CR1 Register
	USART3->CR1 |= (1<<2); // RE=1.. Enable the Receiver
	USART3->CR1 |= (1<<3); // TE=1.. Enable Transmitter

	// 7. Enable TX Interrupt
	//USART2->CR1 |= USART_CR1_TXEIE;
	USART3->CR1 |= USART_CR1_RXNEIE;
	txStateUsart3 = TX3_IDLE;
}

void USART3_IRQHandler(void)
{
	char temp;

	if(USART3->SR & USART_SR_RXNE)
	{
		temp = USART3->DR;

		if(temp == 46)
		{
			(*rxCharUsart3++) = temp;
			(*rxCharUsart3) = 0;
			rxCharUsart3 = (char*) &resive_pack_Usart3;

			USART3_Start_Message();
		}
		else
		{
			(*rxCharUsart3++) = temp;
		}
	}

	if(txStateUsart3 == TX3_TRANS)
	{
		if(USART3->SR & USART_SR_TXE)
		{
			if(*txCharUsart3 != 0)
				USART3->DR = (*txCharUsart3++);
			else
			{
				USART3->CR1 &= ~USART_CR1_TXEIE;
				txStateUsart3 = TX3_IDLE;
			}
		}
	}

}

void USART3_Start_Message(void)
{
	txCharUsart3 = (char*) &resive_pack_Usart3;
	USART3->DR = (*txCharUsart3++);
	USART3->CR1 |= USART_CR1_TXEIE;
	txStateUsart3 = TX3_TRANS;
}


