/*
 * Usart3.h
 *
 *  Created on: May 31, 2021
 *      Author: User
 */

#ifndef USART3_H_
#define USART3_H_

#define TX3_TRANS 1UL
#define TX3_IDLE  0UL

#define PD8	    8UL
#define PD9	    9UL

#define GPIOD_CLK_EN     3
#define USART3_CLK_EN    18
#define ALTF_UART3 		 7

void USART3_Config (void);
void USART3_Start_Message(void);

#endif /* USART3_H_ */
