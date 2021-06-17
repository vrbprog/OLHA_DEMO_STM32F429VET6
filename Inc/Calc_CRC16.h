/*
 * Calc_CRC16.h
 *
 *  Created on: Jun 1, 2021
 *      Author: User
 */

#ifndef CALC_CRC16_H_
#define CALC_CRC16_H_

#include  "stdint.h"

uint16_t modbus_rtu_calc_crc(const void* , uint8_t);
uint16_t calc_crc(uint8_t);
void reset_crc(void);

#endif /* CALC_CRC16_H_ */
