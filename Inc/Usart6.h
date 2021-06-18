/*
 * Usart6.h
 *
 *  Created on: Jun 18, 2021
 *      Author: User
 */

#ifndef USART6_H_
#define USART6_H_

#define TX6_TRANS 1UL
#define TX6_IDLE  0UL

#define PC6	    6UL
#define PC7	    7UL

#define GPIOC_CLK_EN     2
#define USART6_CLK_EN    5
#define ALTF_UART6		 8

void USART6_Config (void);
void USART6_Start_Message(const void*, uint8_t);

#endif /* USART6_H_ */
