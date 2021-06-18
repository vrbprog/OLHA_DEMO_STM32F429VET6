/*
 * Usart6.c
 *
 *  Created on: Jun 18, 2021
 *      Author: User
 */


#include  "stm32f4xx.h"
#include  "USART6.h"
#include  "Led.h"

char resive_pack_Usart3[32];
char *rxCharUsart6 = (char*) &resive_pack_Usart3;
char *txCharUsart6 = (char*) &resive_pack_Usart3;
char *txCharEndUsart6 = (char*) &resive_pack_Usart3;

uint8_t txStateUsart6 = TX6_IDLE;

void USART6_Config (void)
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
	RCC->APB2ENR |= (1 << USART6_CLK_EN); // Enable UART2 CLOCK
	RCC->AHB1ENR |= (1 << GPIOC_CLK_EN); // Enable GPIOA CLOCK

	// 2. Configure the UART PINs for ALternate Functions
	GPIOC->MODER |= (ALTF_MODE << PC6*2);  // Bits (5:4)= 1:0 --> Alternate Function for Pin PC6
	GPIOC->MODER |= (ALTF_MODE << PC7*2);  // Bits (7:6)= 1:0 --> Alternate Function for Pin PC7

	GPIOC->OSPEEDR |= (VERY_HIGH_SPEED << PC6*2);  // Bits (5:4)= 1:1  --> High Speed for PIN PC6
	GPIOA->OSPEEDR |= (VERY_HIGH_SPEED << PC7*2);  // Bits (7:6)= 1:1  --> High Speed for PIN PC7

	GPIOC->AFR[0] |= (ALTF_UART6 << PC6*4); // Bits (11:10:9:8)   = 0:1:1:1 --> AF7 Alt function USART6 at Pin PC6
	GPIOC->AFR[0] |= (ALTF_UART6 << PC7*4); // Bits (15:14:13:12) = 0:1:1:1 --> AF7 Alt function USART6 at Pin PC7

	GPIOC->PUPDR &= ~(DMASK   << PC6*2);
	GPIOC->PUPDR |=  (PULL_UP << PC6*2);

	// 3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
	USART6->CR1  = 0x00;  // clear all
	USART6->CR1 |= (1<<13);  // UE = 1... Enable USART6

	// 4. Program the M bit in USART_CR1 to define the word length.
	USART6->CR1 &= ~(1<<12);  // M =0; 8 bit word length

	// 5. Select the desired baud rate using the USART_BRR register.
	USART6->BRR = (14<<0) | (48<<4);   // Baud rate of 115200, PCLK1 at 90MHz APB2

	// 6. Enable the Transmitter/Receiver by Setting the TE and RE bits in USART_CR1 Register
	USART6->CR1 |= (1<<2); // RE=1.. Enable the Receiver
	USART6->CR1 |= (1<<3); // TE=1.. Enable Transmitter

	// 7. Enable TX Interrupt
	//USART6->CR1 |= USART_CR1_TXEIE;
	USART6->CR1 |= USART_CR1_RXNEIE;
	txStateUsart6 = TX6_IDLE;
}

void USART6_IRQHandler(void)
{
	char temp;

	if(USART6->SR & USART_SR_RXNE)
	{
		temp = USART6->DR;

		//(*rxCharUsart6++) = temp;
	}

	if(txStateUsart6 == TX6_TRANS)
	{
		if(USART6->SR & USART_SR_TXE)
		{
			if(txCharUsart6 != txCharEndUsart6)
				USART6->DR = (*txCharUsart6++);
			else
			{
				USART6->CR1 &= ~USART_CR1_TXEIE;
				txStateUsart6 = TX6_IDLE;
			}
		}
	}

}

void USART6_Start_Message(const void* adress, uint8_t len)
{
	txCharUsart6 = (char*)adress;
	txCharEndUsart6 = (char*)adress + len;
	USART6->DR = (*txCharUsart6++);
	USART6->CR1 |= USART_CR1_TXEIE;
	txStateUsart6 = TX6_TRANS;
}




