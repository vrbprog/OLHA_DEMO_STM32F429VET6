/*
 * Usart2.c
 *
 *  Created on: May 28, 2021
 *      Author: User
 */

#include  "stm32f4xx.h"
#include  "Usart2.h"
#include  "Led.h"
#include  "Calc_CRC16.h"

char resive_pack[32];
char *rxChar = (char*) &resive_pack;
char *txChar = (char*) &resive_pack;
char *txCharEnd = (char*) &resive_pack;
char *txCharCRC = (char*) &resive_pack;

uint8_t txState = 0;

void USART2_Config (void)
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
	RCC->APB1ENR |= (1 << USART2_CLK_EN); // Enable UART2 CLOCK
	RCC->AHB1ENR |= (1 << GPIOA_CLK_EN); // Enable GPIOA CLOCK

	// 2. Configure the UART PINs for ALternate Functions
	GPIOA->MODER |= (ALTF_MODE << PA2*2);  // Bits (5:4)= 1:0 --> Alternate Function for Pin PA2
	GPIOA->MODER |= (ALTF_MODE << PA3*2);  // Bits (7:6)= 1:0 --> Alternate Function for Pin PA3

	GPIOA->OSPEEDR |= (VERY_HIGH_SPEED << PA2*2);  // Bits (5:4)= 1:1  --> High Speed for PIN PA2
	GPIOA->OSPEEDR |= (VERY_HIGH_SPEED << PA3*2);  // Bits (7:6)= 1:1  --> High Speed for PIN PA3

	GPIOA->AFR[0] |= (ALTF_UART2 << PA2*4); // Bits (11:10:9:8)   = 0:1:1:1 --> AF7 Alt function USART2 at Pin PA2
	GPIOA->AFR[0] |= (ALTF_UART2 << PA3*4); // Bits (15:14:13:12) = 0:1:1:1 --> AF7 Alt function USART2 at Pin PA3

	GPIOA->PUPDR &= ~(DMASK   << PA2*2);
	GPIOA->PUPDR |=  (PULL_UP << PA2*2);

	// 3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
	USART2->CR1  = 0x00;  // clear all
	USART2->CR1 |= (1<<13);  // UE = 1... Enable USART2

	// 4. Program the M bit in USART_CR1 to define the word length.
	USART2->CR1 &= ~(1<<12);  // M =0; 8 bit word length

	// 5. Select the desired baud rate using the USART_BRR register.
	USART2->BRR = (7<<0) | (24<<4);   // Baud rate of 115200, PCLK1 at 45MHz

	// 6. Enable the Transmitter/Receiver by Setting the TE and RE bits in USART_CR1 Register
	USART2->CR1 |= (1<<2); // RE=1.. Enable the Receiver
	USART2->CR1 |= (1<<3); // TE=1.. Enable Transmitter

	// 7. Enable TX Interrupt
	//USART2->CR1 |= USART_CR1_TXEIE;
	USART2->CR1 |= USART_CR1_RXNEIE;
	txState = TX2_IDLE;
}

void USART2_IRQHandler(void)
{
	char temp;

	if(USART2->SR & USART_SR_RXNE)
	{
		temp = USART2->DR;

		if(temp == 46)
		{
			(*rxChar) = 0;
			rxChar = (char*) &resive_pack;

			//USART2_Start_Message();
		}
		else
		{
			(*rxChar++) = temp;
		}
	}

	if(txState == TX2_TRANS)
	{
		if(USART2->SR & USART_SR_TXE)
		{
			if(txChar != txCharEnd)
				USART2->DR = (*txChar++);
			else
			{
				USART2->CR1 &= ~USART_CR1_TXEIE;
				txState = TX2_IDLE;
			}
		}
	}

}

void USART2_Start_Message(const void* adress, uint8_t len)
{
	txChar = (char*)adress;
	txCharEnd = (char*)adress + len;
	USART2->DR = (*txChar++);
	USART2->CR1 |= USART_CR1_TXEIE;
	txState = TX2_TRANS;
}

void USART2_Start_Message_CRC(const void* adress, uint8_t len)
{
	txChar = (char*)adress;
	txCharEnd = (char*)adress + len;
	uint16_t crc = modbus_rtu_calc_crc(adress, len - 2);
	txCharCRC = (char*)&crc;
	USART2->DR = (*txCharCRC);
	USART2->CR1 |= USART_CR1_TXEIE;
	txState = TX2_TRANS;
}

