/*
 * Inclinometer.c
 *
 *  Created on: Jun 4, 2021
 *      Author: User
 */

#include "Inclinometer.h"
#include  "Calc_CRC16.h"
#include  "Uart4.h"
#include  "Uart5.h"

extern 	char resive_pack_Uart4[32];
extern 	char *rxCharUart4;
extern 	uint32_t sysTime;
extern uint8_t ready_Inclinom_Handler;

static uint8_t current_zero_mode = NOT_INIT;
static char *nextCharPack = (char*) &resive_pack_Uart4;

const unsigned char query_zero_mode[] = { INC_ADR, 0x03, 0x00, 0x04, 0x00, 0x01, 0xC5, 0xCB };
const unsigned char read_xy_angle[]   = { INC_ADR, 0x03, 0x00, 0x01, 0x00, 0x02, 0x95, 0xCB };

static const unsigned char not_answer_query_zero[] 		= "2 - Error. No answer query zero";
static const unsigned char error_answer_query_zero[] 	= "2 - Error. Incorrect answer query zero";
static const unsigned char answer_query_zero[] 			= "0 - Answer query zero mode - OK";
static const unsigned char answer_xy_angles[] 			= "0 - Answer X, Y angles - OK";
static const unsigned char request_zero[] 				= "0 - Request zero mode";
static const unsigned char request_xy_angles[]			= "0 - Request X, Y angles";

static uint8_t state_Query_Pack = 0;
static uint8_t state_Read_Pack = 0;
static uint8_t error_rx_pack = 0;
static uint16_t crc = 0;
static uint16_t crc_rx = 0;
static uint16_t temp = 0;
static uint16_t temp_X = 0;
static uint16_t temp_Y = 0;
static uint16_t angle_X = 0;
static uint16_t angle_Y = 0;

uint32_t timeLastReq = 0;
uint32_t state_Inclinom_Handler = 0;

void Inclinometer_Request_Zero()
{
	UART4_Start_Message(&query_zero_mode, 8);
	timeLastReq = sysTime;
	state_Inclinom_Handler = QUERY_ZERO;

#ifdef DEBUG_MESSAGE
	UART5_Start_Message(&request_zero, sizeof(request_zero));
#endif
}

void Inclinometer_Read_XY()
{
	UART4_Start_Message(&read_xy_angle, 8);
	timeLastReq = sysTime;
	state_Inclinom_Handler = READ_X_Y;

#ifdef DEBUG_MESSAGE
	UART5_Start_Message(&request_xy_angles, sizeof(request_xy_angles));
#endif
}

void Over_Time_Inclinometer()
{
	if(error_rx_pack)
	{
		UART5_Start_Message(&error_answer_query_zero, sizeof(error_answer_query_zero));
		error_rx_pack = 0;
	}
	else
	{
		UART5_Start_Message(&not_answer_query_zero, sizeof(not_answer_query_zero));
	}

	Clear_State_Curr_Mode();
	state_Inclinom_Handler = INCL_IDLE;
	Reset_RX_bufer();
}

void Clear_State_Curr_Mode()
{
	switch(state_Inclinom_Handler)
	{
		case QUERY_ZERO:
		{
			state_Query_Pack = 0;
			break;
		}

		case READ_X_Y:
		{
			state_Read_Pack = 0;
			break;
		}

		default: ;
	}
}

void Inclinometer_Handler()
{

				while(nextCharPack != rxCharUart4)
				{
					switch(state_Inclinom_Handler)
					{
						case QUERY_ZERO:
						{
							Check_Answer_Query_Zero();
							break;
						}

						case READ_X_Y:
						{
							Check_Read_X_Y();
							break;
						}

						default: ;
					}
				}

				ready_Inclinom_Handler = 0;

}

void Check_Read_X_Y()
{
	switch(state_Read_Pack)
	{
		case 0:
		{

			if(*nextCharPack == INC_ADR)
			{
				state_Read_Pack = 1;
				reset_crc();
				calc_crc(*nextCharPack++);
			}
			else
			{
				Answer_Error();
			}
			break;
		}
		case 1:
		{
			if(*nextCharPack == READ_FUNC)
			{
				state_Read_Pack = 2;
				calc_crc(*nextCharPack++);
			}
			else
			{
				Answer_Error();
			}
			break;
		}
		case 2:
		{
			if(*nextCharPack == 0x04)
			{
				state_Read_Pack = 3;
				calc_crc(*nextCharPack++);
				temp_X = 0;
			}
			else
			{
				Answer_Error();
			}
			break;
		}
		case 3:
		{

			temp_X = *nextCharPack;
			temp_X = temp_X << 8;
			state_Read_Pack = 4;
			calc_crc(*nextCharPack++);

			break;
		}
		case 4:
		{

			temp_X |= *nextCharPack;
			state_Read_Pack = 5;
			calc_crc(*nextCharPack++);
			temp_Y = 0;

			break;
		}
		case 5:
		{

			temp_Y = *nextCharPack;
			temp_Y = temp_Y << 8;
			state_Read_Pack = 6;
			calc_crc(*nextCharPack++);

			break;
		}
		case 6:
		{

			temp_Y |= *nextCharPack;
			state_Read_Pack = 7;
			crc = calc_crc(*nextCharPack++);
			crc_rx = 0;

			break;
		}
		case 7:
		{

			crc_rx |= *nextCharPack++;
			state_Read_Pack = 8;

			break;
		}
		case 8:
		{
			temp = *nextCharPack;
			temp = temp << 8;
			crc_rx |= temp;
			if(crc_rx == crc)
			{
				angle_X = temp_X;
				angle_Y = temp_Y;
				Read_Ok();
			}
			else
			{
				Answer_Error();
			}
			break;
		}


		default: ;
	}
}

void Check_Answer_Query_Zero()
{
	switch(state_Query_Pack)
	{
		case 0:
		{
			if(*nextCharPack == INC_ADR)
			{
				state_Query_Pack = 1;
				reset_crc();
				calc_crc(*nextCharPack++);
			}
			else
			{
				Answer_Error();
			}
			break;
		}
		case 1:
		{
			if(*nextCharPack == READ_FUNC)
			{
				state_Query_Pack = 2;
				calc_crc(*nextCharPack++);
			}
			else
			{
				Answer_Error();
			}
				break;
		}
		case 2:
		{
			if(*nextCharPack == 0x02)
			{
				state_Query_Pack = 3;
				calc_crc(*nextCharPack++);
			}
			else
			{
				Answer_Error();
			}
				break;
		}
		case 3:
		{
			if(*nextCharPack == 0x00)
			{
				state_Query_Pack = 4;
				calc_crc(*nextCharPack++);
			}
			else
			{
				Answer_Error();
			}
				break;
		}
		case 4:
		{
			if(*nextCharPack == ABSOLUTE)
			{
				state_Query_Pack = 5;
				crc = calc_crc(*nextCharPack++);
			}
			else if(*nextCharPack == RELATIVE)
			{
				state_Query_Pack = 7;
				crc = calc_crc(*nextCharPack++);
			}
			else
			{
				Answer_Error();
			}
				break;
		}
		case 5:
		{
			if(*nextCharPack == 0xB8)
			{
				state_Query_Pack = 6;
				nextCharPack++;
			}
			else
			{
				Answer_Error();
			}
				break;
		}
		case 6:
		{
			if(*nextCharPack == 0x44)
			{
				current_zero_mode = ABSOLUTE;
				Answer_Ok();
			}
			else
			{
				Answer_Error();
			}
			break;
		}
		case 7:
		{
			if(*nextCharPack == 0x79)
			{
				state_Query_Pack = 8;
				nextCharPack++;
			}
			else
			{
				Answer_Error();
			}
			break;
		}
		case 8:
		{
			if(*nextCharPack == 0x84)
			{
				current_zero_mode = RELATIVE;
				Answer_Ok();
			}
			else
			{
				Answer_Error();
			}
			break;
		}

		default: ;
	}

}

void Reset_RX_bufer()
{
	nextCharPack	= (char*) &resive_pack_Uart4;
	rxCharUart4		= (char*) &resive_pack_Uart4;
}

void Read_Ok()
{

#ifdef DEBUG_MESSAGE
	UART5_Start_Message(&answer_xy_angles, sizeof(answer_xy_angles));
#endif

	error_rx_pack = 0;
	state_Read_Pack = 0;
	state_Inclinom_Handler = INCL_IDLE;

	if((rxCharUart4 - nextCharPack) > 1){
		nextCharPack++;
	}
	else {
		Reset_RX_bufer();
	}

}

void Answer_Ok()
{

#ifdef DEBUG_MESSAGE
	UART5_Start_Message(&answer_query_zero, sizeof(answer_query_zero));
#endif

	error_rx_pack = 0;
	state_Query_Pack = 0;
	state_Inclinom_Handler = INCL_IDLE;

	if((rxCharUart4 - nextCharPack) > 1){
		nextCharPack++;
	}
	else {
		Reset_RX_bufer();
	}
}

void Answer_Error()
{
	if((rxCharUart4 - nextCharPack) > 1)
		nextCharPack++;
	else
		Reset_RX_bufer();

	error_rx_pack = 1;
}
