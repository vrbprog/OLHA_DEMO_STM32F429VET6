/*
 * Uart5.c
 *
 *  Created on: Jun 1, 2021
 *      Author: User
 */

#include  "stm32f4xx.h"
#include  "Uart5.h"
#include  "Led.h"

static char resive_pack_Uart5[32];
static char *rxCharUart5 = (char*) &resive_pack_Uart5;
static char *txCharUart5 = (char*) &resive_pack_Uart5;
static char *txCharEndUart5 = (char*) &resive_pack_Uart5;

static uint8_t txStateUart5;

typedef struct message
{
	char* adr;
	uint8_t lens;
} mess;

static mess queue[8];
static uint8_t index_r = 0;
static uint8_t index_w = 0;

static uint8_t num_mess_UART5 = 0;

void UART5_Config (void)
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
		RCC->APB1ENR |= (1 << UART5_CLK_EN); // Enable UART2 CLOCK
		RCC->AHB1ENR |= (1 << GPIOC_CLK_EN); // Enable GPIOC CLOCK
		RCC->AHB1ENR |= (1 << GPIOD_CLK_EN); // Enable GPIOD CLOCK

		// 2. Configure the UART PINs for ALternate Functions
		GPIOC->MODER |= (ALTF_MODE << PC12*2);  // Bits (1:0)= 1:0 --> Alternate Function for Pin PC12
		GPIOD->MODER |= (ALTF_MODE << PD2*2);   // Bits (3:2)= 1:0 --> Alternate Function for Pin PD2

		GPIOC->OSPEEDR |= (VERY_HIGH_SPEED << PC12*2);  // Bits (1:0)= 1:1  --> High Speed for PIN PC12
		GPIOD->OSPEEDR |= (VERY_HIGH_SPEED << PD2*2);   // Bits (3:2)= 1:1  --> High Speed for PIN PD2

		GPIOC->AFR[1] |= (ALTF_UART5 << (PC12-8)*4); // Bits (3:2:1:0) = 1:0:0:0 --> AF7 Alt function UART5 at Pin PC12
		GPIOD->AFR[0] |= (ALTF_UART5 << PD2*4);      // Bits (7:6:5:4) = 1:0:0:0 --> AF7 Alt function UART5 at Pin PD2

		GPIOC->PUPDR &= ~(DMASK   << PC12*2);
		GPIOC->PUPDR |=  (PULL_UP << PC12*2);

		// 3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
		UART5->CR1  = 0x00;  // clear all
		UART5->CR1 |= (1<<13);  // UE = 1... Enable UART4

		// 4. Program the M bit in USART_CR1 to define the word length.
		UART5->CR1 &= ~(1<<12);  // M =0; 8 bit word length

		// 5. Select the desired baud rate using the USART_BRR register.
		UART5->BRR = (7<<0) | (24<<4);   // Baud rate of 115200, PCLK1 at 45MHz

		// 6. Enable the Transmitter/Receiver by Setting the TE and RE bits in USART_CR1 Register
		UART5->CR1 |= (1<<2); // RE=1.. Enable the Receiver
		UART5->CR1 |= (1<<3); // TE=1.. Enable Transmitter

		// 7. Enable TX Interrupt
		//USART2->CR1 |= USART_CR1_TXEIE;
		UART5->CR1  |= USART_CR1_RXNEIE;
		txStateUart5 = TX5_IDLE;
}

void UART5_IRQHandler(void)
{
	char temp;

	if(UART5->SR & USART_SR_RXNE)
	{
		temp = UART5->DR;

		if(temp == 46)
		{
			(*rxCharUart5++) = temp;
			(*rxCharUart5) = 0;
			rxCharUart5 = (char*) &resive_pack_Uart5;

			//UART5_Start_Message();
		}
		else
		{
			(*rxCharUart5++) = temp;
		}
	}

	if(txStateUart5 == TX5_TRANS)
	{
		if(UART5->SR & USART_SR_TXE)
		{
			if(txCharUart5 != txCharEndUart5)
				UART5->DR = (*txCharUart5++);
			else
			{
				if(num_mess_UART5)
				{
					UART5_Queue_Handler();
				}
				else
				{
					UART5->CR1 &= ~USART_CR1_TXEIE;
					txStateUart5 = TX5_IDLE;
				}
			}
		}
	}

}

void UART5_Start_Message(const void* adress, uint8_t len)
{
	if(txStateUart5 == TX5_IDLE)
	{
		txCharUart5 = (char*)adress;
		txCharEndUart5 = (char*)adress + len - 1;
		UART5->DR = (*txCharUart5++);
		UART5->CR1 |= USART_CR1_TXEIE;
		txStateUart5 = TX5_TRANS;
	}
	else
	{
		queue[index_r].adr = (char*)adress;
		queue[index_r++].lens = len;
		index_r &= 0x07;
		num_mess_UART5++;
		// Check overflow
	}
}

void UART5_Queue_Handler()
{
	//if(txStateUart5 == TX5_IDLE)
	{
		txCharUart5 = queue[index_w].adr;
		txCharEndUart5 = txCharUart5 + queue[index_w++].lens - 1;
		num_mess_UART5--;
		index_w &= 0x07;
		UART5->DR = (*txCharUart5++);
		//UART5->CR1 |= USART_CR1_TXEIE;
		//txStateUart5 = TX5_TRANS;
	}
}



