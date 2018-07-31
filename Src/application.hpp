/**
  ******************************************************************************
  * @file           : application.hpp
  * @brief          : 	�������� �����, ������������ STL, � ������� 
  * 					��� ������������� ������� � ������ 
  *	@author			: rual
  * @date			:	18.07.2018 
  ******************************************************************************
*/

#ifndef __PROCESS_HPP_
#define __PROCESS_HPP_

#include "main.h"
#include "cmsis_os.h"
#include "stm32f3xx_hal.h"

#include <strstream>
#include <string>
#include <map>
#include <string.h>

using namespace std;

/**
* ��� �������������� ������ map ���������� STL ��� ���������� ������� 		*
* ��������������� #define USE_STL, ��� ������� � ������������� ���������,	*
* �� �� ��������� STM32F3Discovery �� ����������� ��-�� ���������� RAM 		*
* � ������� ������ ��������� ��������� ����������� ���������� string.h		*
*	� ����� ������ ������� strcmp()
*/
//#define USE_STL 

/// ����� ������ ����������� ��������
#define RX_SIMB_QUEUE_SZ 64

/// ����� ������� ���������
#define RX_MESSAGE_QUEUE_SZ 8

/// �������� ���������� �������� USART1
extern UART_HandleTypeDef huart1;
/**
**********************************************************************
 @class 	CommandParsing ��������� ������������� �������
 @param 	char* rx_message ����������� ������, ���������� �������
 @return 	���� �� �������� enCmd_t, �������������� �������, ����, 
			���� ������� �� ���������� - CMD_NULL
*/

class CommandParsing {
	
	
	public:
		/**
		�������� ������� */
		typedef enum {
			CMD_NULL,
			CMD_VOLTAGE,
			CMD_LED_ON,
			CMD_LED_OFF,
			CMD_RATE
		} enCmd_t;
		
		/**
		����������� - ��������� ����� � �������� ������
		*/
		CommandParsing(void) 
		{
			command["LED_ON"] 	=	CMD_LED_ON;
			command["LED_OFF"]	=	CMD_LED_OFF;
			command["GET_ADC_AVG_VOLTAGE"]	=	CMD_VOLTAGE;
			command["SET_ADC_SAMPLE_RATE"]	=	CMD_RATE; 
		}
		
		/**
		����� ����������� ������������� ������� � ������
		@param 	string key - ������ � ������-�������
		@return ��������� �������� �������, ���� ����������, CMD_NULL - ���� ���. 
		*/
		enCmd_t GetCommand(std::string key) 
		{
			return command[key];
		}
		
	private:
		/// ������ STL map - �������� ����������� �����-������ ������� � ��������
		std::map <std::string , enCmd_t> command;
		
};

/**
**********************************************************************
 @function 	GetCommand ��������� ������������� �������
 @param 	char* rx_message ����������� ������, ���������� �������
 @return 	���� �� �������� enCmd_t, �������������� �������, ����, 
			���� ������� �� ���������� - CMD_NULL
*/

CommandParsing::enCmd_t GetCommand(char* rx_message)
{
	if 		(strcmp(rx_message,"LED_ON")	== 0)	return CommandParsing::CMD_LED_ON;
	
	else if (strcmp(rx_message,"LED_OFF")	== 0)	return CommandParsing::CMD_LED_OFF;
	
	else if (strcmp(rx_message,"GET_ADC_AVG_VOLTAGE") == 0)	return  CommandParsing::CMD_VOLTAGE;
	
	else if (strncmp(rx_message,"SET_ADC_SAMPLE_RATE",18) == 0)	return  CommandParsing::CMD_RATE;
	
	return CommandParsing::CMD_NULL;	
}

#endif // __PROCESS_HPP_

