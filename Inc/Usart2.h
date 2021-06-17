/*
 * Usart2.h
 *
 *  Created on: May 28, 2021
 *      Author: User
 */

#ifndef USART2_H_
#define USART2_H_

#define TX2_TRANS 1UL
#define TX2_IDLE  0UL

#define PA2	    2UL
#define PA3	    3UL

#define GPIOA_CLK_EN     0
#define USART2_CLK_EN    17
#define ALTF_UART2		 7

void USART2_Config (void);
void USART2_Start_Message(const void*, uint8_t);
void USART2_Start_Message_CRC(const void*, uint8_t);

#endif /* USART2_H_ */
