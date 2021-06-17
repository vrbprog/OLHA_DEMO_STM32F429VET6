/*
 * Inclinometer.h
 *
 *  Created on: Jun 4, 2021
 *      Author: User
 */

#ifndef INCLINOMETER_H_
#define INCLINOMETER_H_

#define DEBUG_MESSAGE

#define INCL_IDLE  0UL
#define QUERY_ZERO 1UL
#define READ_X_Y   2UL
#define INC_ADR	   0x01
#define READ_FUNC 0x03

#define NOT_INIT 0xFF
#define ABSOLUTE 0x00
#define RELATIVE 0x01

void Inclinometer_Request_Zero();
void Inclinometer_Handler();
void Check_Answer_Query_Zero();
void Reset_RX_bufer();
void Answer_Ok();
void Answer_Error();
void Read_Ok();
void Check_Read_X_Y();
void Inclinometer_Read_XY();
void Over_Time_Inclinometer();
void Clear_State_Curr_Mode();

#endif /* INCLINOMETER_H_ */
