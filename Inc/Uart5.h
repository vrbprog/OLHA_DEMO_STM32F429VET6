/*
 * Uart5.h
 *
 *  Created on: Jun 1, 2021
 *      Author: User
 */

#ifndef UART5_H_
#define UART5_H_

#define TX5_TRANS 1UL
#define TX5_IDLE  0UL

#define PC12    12UL
#define PD2	    2UL

#define GPIOC_CLK_EN     2
#define GPIOD_CLK_EN     3
#define UART5_CLK_EN    20
#define ALTF_UART5  	8

void UART5_Config (void);
void UART5_Start_Message(const void* adress, uint8_t len);
void UART5_Queue_Handler();

#endif /* UART5_H_ */
