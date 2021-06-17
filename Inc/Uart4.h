/*
 * Uart4.h
 *
 *  Created on: May 31, 2021
 *      Author: User
 */

#ifndef UART4_H_
#define UART4_H_

#define TX4_TRANS 1UL
#define TX4_IDLE  0UL

#define PA0	    0UL
#define PA1	    1UL

#define GPIOA_CLK_EN     0
#define UART4_CLK_EN    19
#define ALTF_UART4  	8

#include  "stdint.h"

void UART4_Config (void);
void UART4_Start_Message (const void*, uint8_t);

#endif /* UART4_H_ */
