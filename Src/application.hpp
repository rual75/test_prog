/**
  ******************************************************************************
  * @file           : application.hpp
  * @brief          : 	содержит класс, использующий STL, и функцию 
  * 					для распознования команды в строке 
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
* для использоваания класса map библиотеки STL для дешифровки команды 		*
* раскоментируйте #define USE_STL, код рабочий и компилируется корректно,	*
* но на платформе STM32F3Discovery не запускается из-за недостатка RAM 		*
* В рабочей версии программы применена стандартная библиотека string.h		*
*	и более легкая функция strcmp()
*/
//#define USE_STL 

/// длина буфера принимаемых символов
#define RX_SIMB_QUEUE_SZ 64

/// длина очереди сообщений
#define RX_MESSAGE_QUEUE_SZ 8

/// прототип глобальной струтуры USART1
extern UART_HandleTypeDef huart1;
/**
**********************************************************************
 @class 	CommandParsing выполняет распознавание команды
 @param 	char* rx_message стандартная строка, содержащая команду
 @return 	одно из значений enCmd_t, соответсвующая команде, либо, 
			если команда не распознана - CMD_NULL
*/

class CommandParsing {
	
	
	public:
		/**
		значение команды */
		typedef enum {
			CMD_NULL,
			CMD_VOLTAGE,
			CMD_LED_ON,
			CMD_LED_OFF,
			CMD_RATE
		} enCmd_t;
		
		/**
		конструктор - заполняет ключи и значения команд
		*/
		CommandParsing(void) 
		{
			command["LED_ON"] 	=	CMD_LED_ON;
			command["LED_OFF"]	=	CMD_LED_OFF;
			command["GET_ADC_AVG_VOLTAGE"]	=	CMD_VOLTAGE;
			command["SET_ADC_SAMPLE_RATE"]	=	CMD_RATE; 
		}
		
		/**
		метод выполняющий распознование команды в строке
		@param 	string key - строка с ключом-строкой
		@return возращает значение команды, если распознана, CMD_NULL - если нет. 
		*/
		enCmd_t GetCommand(std::string key) 
		{
			return command[key];
		}
		
	private:
		/// объект STL map - содержит соответсвие ключа-строки команды и значение
		std::map <std::string , enCmd_t> command;
		
};

/**
**********************************************************************
 @function 	GetCommand выполняет распознавание команды
 @param 	char* rx_message стандартная строка, содержащая команду
 @return 	одно из значений enCmd_t, соответсвующая команде, либо, 
			если команда не распознана - CMD_NULL
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

